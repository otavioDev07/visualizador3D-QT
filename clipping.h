#ifndef CLIPPING_H
#define CLIPPING_H

#include <QPointF>
#include <algorithm>
#include <cmath>
#include "matriz.h"

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

namespace Volumetrico {

constexpr double Z_MINIMO = 1e-3;

inline bool pontoDentro(const Ponto3D& p, double largura, double altura, double d)
{
    if (p.z < Z_MINIMO || d <= 0.0)
        return false;

    const double limiteX = p.z * (largura * 0.5) / d;
    const double limiteY = p.z * (altura  * 0.5) / d;
    return p.x >= -limiteX && p.x <= limiteX &&
           p.y >= -limiteY && p.y <= limiteY;
}

// Liang-Barsky generalizado
inline bool recortarReta(Ponto3D& a, Ponto3D& b,double largura, double altura, double d){
    if (d <= 0.0 || largura <= 0.0 || altura <= 0.0)
        return false;

    const double fatorX = (largura * 0.5) / d;
    const double fatorY = (altura  * 0.5) / d;
    const auto valorPlano = [fatorX, fatorY](const Ponto3D& p, int plano) {
        switch (plano) {
        case 0: return p.z - Z_MINIMO;       // frente do COP
        case 1: return p.x + fatorX * p.z;   // esquerda
        case 2: return fatorX * p.z - p.x;   // direita
        case 3: return p.y + fatorY * p.z;   // baixo
        default: return fatorY * p.z - p.y;  // cima
        }
    };

    double inicio = 0.0;
    double fim = 1.0;

    for (int plano = 0; plano < 5; ++plano) {
        const double fa = valorPlano(a, plano);
        const double fb = valorPlano(b, plano);
        const bool aFora = fa < 0.0;
        const bool bFora = fb < 0.0;
        if (aFora && bFora)
            return false;
        if (!aFora && !bFora)
            continue;

        const double t = fa / (fa - fb);
        if (aFora)
            inicio = std::max(inicio, t);
        else
            fim = std::min(fim, t);

        if (inicio > fim)
            return false;
    }

    const Ponto3D originalA = a;
    const Ponto3D direcao(b.x - a.x, b.y - a.y, b.z - a.z);
    a = Ponto3D(originalA.x + inicio * direcao.x, originalA.y + inicio * direcao.y, originalA.z + inicio * direcao.z);
    b = Ponto3D(originalA.x + fim * direcao.x, originalA.y + fim * direcao.y, originalA.z + fim * direcao.z);

    // Protege a divisão perspectiva contra arredondamento na interseção.
    a.z = std::max(a.z, Z_MINIMO);
    b.z = std::max(b.z, Z_MINIMO);
    return true;
}

}

}

#endif // CLIPPING_H
