#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QPointF>
#include <vector>
#include <QColor>
#include <vector>
#include <utility>
#include <algorithm>
#include <limits>
#include "matriz.h"

enum class TipoObjeto {
    PONTO,
    RETA,
    MODELO
};

struct Objeto {
    QString nome;
    TipoObjeto tipo;

    std::vector<Ponto3D> vertices;
    std::vector<std::pair<int, int>> arestas;
    std::vector<QPointF> pontosSCN;
    QColor cor;

    Objeto(QString n, TipoObjeto t, const std::vector<QPointF>& pontos2D)
        : nome(std::move(n)), tipo(t), cor(corPadrao(t)) {
        vertices.reserve(pontos2D.size());
        for (const auto& p : pontos2D)
            vertices.emplace_back(p.x(), p.y(), 0.0);
        if (t != TipoObjeto::PONTO) {
            for (size_t i = 0; i + 1 < vertices.size(); i += 2)
                arestas.emplace_back(static_cast<int>(i), static_cast<int>(i + 1));
        }
    }

    Objeto(QString n, TipoObjeto t, std::vector<Ponto3D> v, std::vector<std::pair<int, int>> a)
        : nome(std::move(n)), tipo(t), vertices(std::move(v)), arestas(std::move(a)), cor(corPadrao(t)) {}


    static QColor corPadrao(TipoObjeto t) {
        switch (t) {
        case TipoObjeto::PONTO:  return QColor(Qt::red);
        case TipoObjeto::RETA:   return QColor(Qt::blue);
        case TipoObjeto::MODELO: return QColor(Qt::darkGreen);
        }
        return QColor(Qt::black);
    }

    Ponto3D centroide() const {
        if (vertices.empty())
            return Ponto3D(0.0, 0.0, 0.0);

        double somaX = 0.0, somaY = 0.0, somaZ = 0.0;
        for (const auto& p : vertices) {
            somaX += p.x;
            somaY += p.y;
            somaZ += p.z;
        }
        const double n = static_cast<double>(vertices.size());
        return Ponto3D(somaX / n, somaY / n, somaZ / n);
    }

    void limites(Ponto3D& minimo, Ponto3D& maximo) const {
        constexpr double inf = std::numeric_limits<double>::infinity();
        minimo = Ponto3D(inf, inf, inf);
        maximo = Ponto3D(-inf, -inf, -inf);
        for (const auto& p : vertices) {
            minimo.x = std::min(minimo.x, p.x);  maximo.x = std::max(maximo.x, p.x);
            minimo.y = std::min(minimo.y, p.y);  maximo.y = std::max(maximo.y, p.y);
            minimo.z = std::min(minimo.z, p.z);  maximo.z = std::max(maximo.z, p.z);
        }
    }

    void normalizar(double tamanhoAlvo, const Ponto3D& destino) {
        if (vertices.empty())
            return;

        Ponto3D mn, mx;
        limites(mn, mx);
        const double maior = std::max({mx.x - mn.x, mx.y - mn.y, mx.z - mn.z});
        if (maior <= 0.0)
            return;

        const Ponto3D centro((mn.x + mx.x) / 2.0, (mn.y + mx.y) / 2.0, (mn.z + mx.z) / 2.0);
        const double f = tamanhoAlvo / maior;

        transformar(Matriz::translacao(destino.x, destino.y, destino.z)
                    * Matriz::escala(f, f, f)
                    * Matriz::translacao(-centro.x, -centro.y, -centro.z));
    }

    void atualizarSCN(const Matriz& pipeline) {
        pontosSCN.resize(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            const Ponto3D p = pipeline * vertices[i];
            pontosSCN[i] = QPointF(p.x, p.y);
        }
    }

    void transformar(const Matriz& m) {
        for (auto& p : vertices)
            p = m * p;
    }
};

#endif // OBJETO_H