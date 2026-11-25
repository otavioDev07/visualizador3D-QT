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
#include "clipping.h"

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
    std::vector<std::pair<QPointF, QPointF>> linhasSCN;
    std::vector<bool> visivel;
    bool usaClippingVolumetrico = false;
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
        linhasSCN.clear();
        usaClippingVolumetrico = false;
        visivel.assign(vertices.size(), true);
        for (size_t i = 0; i < vertices.size(); ++i) {
            const Ponto3D p = pipeline * vertices[i];
            pontosSCN[i] = QPointF(p.x, p.y);
        }
    }

    void transformar(const Matriz& m) {
        for (auto& p : vertices)
            p = m * p;
    }

    void atualizarSCNPerspectiva(const Matriz& visao, const Matriz& mper, const Matriz& scn2D,
                                 double larguraJanela, double alturaJanela, double distancia) {
        pontosSCN.resize(vertices.size());
        visivel.resize(vertices.size());
        linhasSCN.clear();
        linhasSCN.reserve(arestas.size());
        usaClippingVolumetrico = true;

        std::vector<Ponto3D> pontosVisao(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i)
            pontosVisao[i] = visao * vertices[i];

        for (size_t i = 0; i < vertices.size(); ++i) {
            double w = 0.0;
            const Ponto3D p = mper.aplicarHomogeneo(pontosVisao[i], w);

            if (!Clipping::Volumetrico::pontoDentro(pontosVisao[i], larguraJanela,
                                                     alturaJanela, distancia)) {
                visivel[i] = false;
                pontosSCN[i] = QPointF();
                continue;
            }
            visivel[i] = true;
            pontosSCN[i] = scn2D * QPointF(p.x, p.y);
        }

        // Os extremos gerados pelo recorte podem não ser vértices originais;
        // por isso as arestas projetadas são guardadas separadamente.
        for (const auto& aresta : arestas) {
            if (aresta.first < 0 || aresta.second < 0 ||
                static_cast<size_t>(aresta.first) >= pontosVisao.size() ||
                static_cast<size_t>(aresta.second) >= pontosVisao.size())
                continue;

            Ponto3D a = pontosVisao[static_cast<size_t>(aresta.first)];
            Ponto3D b = pontosVisao[static_cast<size_t>(aresta.second)];
            if (!Clipping::Volumetrico::recortarReta(a, b, larguraJanela,
                                                      alturaJanela, distancia))
                continue;

            double wa = 0.0, wb = 0.0;
            const Ponto3D pa = mper.aplicarHomogeneo(a, wa);
            const Ponto3D pb = mper.aplicarHomogeneo(b, wb);
            linhasSCN.emplace_back(scn2D * QPointF(pa.x, pa.y),
                                   scn2D * QPointF(pb.x, pb.y));
        }
    }
};

#endif // OBJETO_H
