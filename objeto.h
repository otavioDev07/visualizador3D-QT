#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QPointF>
#include <vector>

enum class TipoObjeto {
    PONTO,
    RETA,
    POLIGONO
};

struct Objeto {
    QString nome;
    TipoObjeto tipo;
    std::vector<QPointF> pontos;

    Objeto(QString n, TipoObjeto t, std::vector<QPointF> p)
        : nome(n), tipo(t), pontos(p) {}
};

#endif // OBJETO_H