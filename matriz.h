#ifndef MATRIZ_H
#define MATRIZ_H

#include <QPointF>
#include <cmath>

struct Ponto3D {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Ponto3D() = default;
    Ponto3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

enum class Eixo { X = 0, Y = 1, Z = 2 };

class Matriz {
private:
    double m[4][4];

public:

    Matriz() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0 : 0.0;
    }

    double& operator()(int linha, int coluna) {
        return m[linha][coluna];
    }
    double operator()(int linha, int coluna) const {
        return m[linha][coluna];
    }

    Matriz operator*(const Matriz& outra) const{
        Matriz resultado;
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                resultado.m[i][j] = 0.0;
                for (int k = 0; k < 4; k++)
                    resultado.m[i][j] += m[i][k] * outra.m[k][j];
            }
        }
        return resultado;
    }

    Ponto3D operator*(const Ponto3D& p) const {
        const double x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
        const double y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
        const double z = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
        return Ponto3D(x, y, z);
    }

    QPointF operator*(const QPointF& p) const {
        const double x = m[0][0] * p.x() + m[0][1] * p.y() + m[0][3];
        const double y = m[1][0] * p.x() + m[1][1] * p.y() + m[1][3];
        return QPointF(x, y);
    }

    static Matriz translacao(double dx, double dy, double dz = 0.0) {
        Matriz t;
        t.m[0][3] = dx;
        t.m[1][3] = dy;
        t.m[2][3] = dz;
        return t;
    }
    
    static Matriz escala(double sx, double sy, double sz = 1.0) {
        Matriz s;
        s.m[0][0] = sx;
        s.m[1][1] = sy;
        s.m[2][2] = sz;
        return s;
    }

    static Matriz rotacao(Eixo eixo, double graus) {
        const double rad = graus * M_PI / 180.0;
        const double c = std::cos(rad);
        const double s = std::sin(rad);
        Matriz r;
        switch (eixo) {
        case Eixo::X:
            r.m[1][1] = c;  r.m[1][2] = -s;
            r.m[2][1] = s;  r.m[2][2] = c;
            break;
        case Eixo::Y:
            r.m[0][0] = c;  r.m[0][2] = s;
            r.m[2][0] = -s; r.m[2][2] = c;
            break;
        case Eixo::Z:
            r.m[0][0] = c;  r.m[0][1] = -s;
            r.m[1][0] = s;  r.m[1][1] = c;
            break;
        }
        return r;
    }

    static Matriz rotacao(double graus) { return rotacao(Eixo::Z, graus); }

    static Matriz escalaEmTorno(double sx, double sy, double sz, const Ponto3D& centro) {
        return translacao(centro.x, centro.y, centro.z)
        * escala(sx, sy, sz)
            * translacao(-centro.x, -centro.y, -centro.z);
    }
    
    static Matriz rotacaoEmTorno(Eixo eixo, double graus, const Ponto3D& centro) {
        return translacao(centro.x, centro.y, centro.z)
        * rotacao(eixo, graus)
            * translacao(-centro.x, -centro.y, -centro.z);
    }

    static Matriz rotacaoEmTorno(double graus, const QPointF& centro) {
        return rotacaoEmTorno(Eixo::Z, graus, Ponto3D(centro.x(), centro.y(), 0.0));
    }

    static Matriz projecaoOrtogonal() {
        Matriz p;
        p.m[2][2] = 0.0;
        return p;
    }
};

#endif // MATRIZ_H
