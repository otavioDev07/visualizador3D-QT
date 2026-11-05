#ifndef CLIPPING_H
#define CLIPPING_H

#include <QPointF>

namespace Clipping {
// window ja normalizada
constexpr double XMIN = -1.0;
constexpr double XMAX = 1.0;
constexpr double YMIN = -1.0;
constexpr double YMAX = 1.0;


constexpr int DENTRO = 0; //0000
constexpr int ESQUERDA = 1; //0001
constexpr int DIREITA = 2; //0010
constexpr int ABAIXO = 4; //0100
constexpr int ACIMA = 8; //1000


//funções "inline" para tentar evitar overhed de chamada de funções
inline int regionCode(const QPointF& p){
    int codigo = DENTRO; // começa como 0000

    if (p.x() < XMIN) codigo |= ESQUERDA; // faz OU logico e ja guarda na variável
    else if (p.x() > XMAX) codigo |= DIREITA;

    if (p.y() < YMIN) codigo |= ABAIXO;
    else if (p.y() > YMAX) codigo |= ACIMA;

    return codigo;
}

inline bool pontoDentro(const QPointF& p){
    return regionCode(p) == DENTRO;
}

inline bool recortarReta(QPointF& a, QPointF& b){
    int codeA = regionCode(a);
    int codeB = regionCode(b);

    while (true){
        if ((codeA | codeB) ==  DENTRO){
            return true;
        }
        if ((codeA & codeB) != DENTRO){
            return false;
        }

        const int codigoFora = codeA != DENTRO ? codeA : codeB;

        const double dx = b.x() - a.x();
        const double dy = b.y() - a.y();

        double x = 0.0;
        double y = 0.0;

        //manipulação das equações da reta
        if (codigoFora & ACIMA) {
            y = YMAX;
            x = a.x() + dx * (YMAX - a.y()) / dy;
        }
        else if (codigoFora & ABAIXO) {
            y = YMIN;
            x = a.x() + dx * (YMIN - a.y()) / dy;
        }
        else if (codigoFora & DIREITA) {
            x = XMAX;
            y = a.y() + dy * (XMAX - a.x()) / dx;
        }
        else if (codigoFora & ESQUERDA) {
            x = XMIN;
            y = a.y() + dy * (XMIN - a.x()) / dx;
        }

        if (codigoFora == codeA){
            a = QPointF(x, y);
            codeA = regionCode(a);
        } else {
            b = QPointF(x,y);
            codeB = regionCode(b);
        }
    }

}

}

#endif // CLIPPING_H