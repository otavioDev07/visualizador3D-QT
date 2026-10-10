#ifndef MATRIZ_H
#define MATRIZ_H

#include <QPointF>
#include <cmath>

class Matriz {
private:
    double m[3][3];

public:

    Matriz() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? 1.0 : 0.0;
    }

    double& operator()(int linha, int coluna){
        return m[linha][coluna];
    }
    double operator()(int linha, int coluna) const{
        return m[linha][coluna];
    }

    Matriz operator*(const Matriz& outra) const{
        Matriz resultado;
        for (int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++){
                resultado.m[i][j] = 0.0;
                for (int k = 0; k < 3; k++)
                    resultado.m[i][j] += m[i][k] * outra.m[k][j];
            }
        }
        return resultado;
    }

    QPointF operator*(const QPointF& p)const {
        double x = m[0][0] * p.x() + m[0][1] * p.y() + m[0][2];
        double y = m[1][0] * p.x() + m[1][1] * p.y() + m[1][2];
        return QPointF(x, y);
    }

    static Matriz translacao(double dx, double dy) {
        Matriz t;
        t.m[0][2] = dx;
        t.m[1][2] = dy;
        return t;
    }

    static Matriz escala(double sx, double sy) {
        Matriz s;
        s.m[0][0] = sx;
        s.m[1][1] = sy;
        return s;
    }

    static Matriz rotacao(double graus) {
        double rad = graus * M_PI / 180.0;
        double c = std::cos(rad);
        double s = std::sin(rad);
        Matriz r;
        r.m[0][0] = c;  r.m[0][1] = -s;
        r.m[1][0] = s;  r.m[1][1] = c;
        return r;
    }

    static Matriz escalaEmTorno(double sx, double sy, const QPointF& centro) {
        return translacao(centro.x(), centro.y())
        * escala(sx, sy)
            * translacao(-centro.x(), -centro.y());
    }

    static Matriz rotacaoEmTorno(double graus, const QPointF& centro) {
        return translacao(centro.x(), centro.y())
        * rotacao(graus)
            * translacao(-centro.x(), -centro.y());
    }
};

#endif // MATRIZ_H
