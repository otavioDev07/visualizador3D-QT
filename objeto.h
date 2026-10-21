#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QPointF>
#include <vector>
#include "matriz.h"

enum class TipoObjeto {
    PONTO,
    RETA,
    POLIGONO
};

struct Objeto {
    QString nome;
    TipoObjeto tipo;
    std::vector<QPointF> pontos;      // coordenadas do mundo (originais)
    std::vector<QPointF> pontosSCN;   // coordenadas normalizadas, derivadas de 'pontos'

    Objeto(QString n, TipoObjeto t, std::vector<QPointF> p)
        : nome(n), tipo(t), pontos(p) {}


    QPointF centroide() const {
        if (pontos.empty())
            return QPointF(0.0, 0.0);

        double somaX = 0.0;
        double somaY = 0.0;
        for (const auto& p : pontos) {
            somaX += p.x();
            somaY += p.y();
        }
        const double n = static_cast<double>(pontos.size());
        return QPointF(somaX / n, somaY / n);
    }

    // Recalcula as coordenadas normalizadas a partir das do mundo.
    void atualizarSCN(const Matriz& scn) {
        pontosSCN.resize(pontos.size());
        for (size_t i = 0; i < pontos.size(); ++i)
            pontosSCN[i] = scn * pontos[i];
    }

    void transformar(const Matriz& m) {
        for (auto& p : pontos)
            p = m * p;
    }
};

#endif // OBJETO_H