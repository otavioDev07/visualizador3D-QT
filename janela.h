#ifndef JANELA_H
#define JANELA_H

#include <QPointF>
#include <cmath>
#include "matriz.h"

enum class TipoProjecao { Paralela, Perspectiva };

class Janela {
private:
    QPointF centro_ {0.0, 0.0};
    double largura_ = 700.0;
    double altura_  = 700.0;
    double angulo_  = 0.0;

    TipoProjecao projecao_ = TipoProjecao::Paralela;
    Ponto3D vpn_ {0.0, 0.0, 1.0};   // View Plane Normal
    double d_ = 800.0;              // distância do COP até o plano de projeção

    Matriz scn_;
    Matriz visao_;
    Matriz scn2D_;

    void recalcular() {
        scn_ = Matriz::escala(2.0 / largura_, 2.0 / altura_)
               * Matriz::rotacao(-angulo_)
               * Matriz::translacao(-centro_.x(), -centro_.y());

        // VPN normalizado
        const double len = std::sqrt(vpn_.x*vpn_.x + vpn_.y*vpn_.y + vpn_.z*vpn_.z);
        const double vx = vpn_.x / len, vy = vpn_.y / len, vz = vpn_.z / len;

        const Ponto3D cw(centro_.x(), centro_.y(), 0.0);
        const Ponto3D cop(cw.x - d_ * vx, cw.y - d_ * vy, cw.z - d_ * vz);

        // Angulos do VPN em torno de Y e de X
        const double r = std::sqrt(vx*vx + vz*vz);
        const double grausY = -std::atan2(vx, vz) * 180.0 / M_PI;
        const double grausX =  std::atan2(vy, r)  * 180.0 / M_PI;

        visao_ = Matriz::rotacao(Eixo::X, grausX)
                 * Matriz::rotacao(Eixo::Y, grausY)
                 * Matriz::translacao(-cop.x, -cop.y, -cop.z);

        scn2D_ = Matriz::escala(2.0 / largura_, 2.0 / altura_)
                 * Matriz::rotacao(-angulo_);
    }

public:
    Janela() { recalcular(); }

    void definir(const QPointF &centro, double largura, double altura, double angulo) {
        centro_ = centro;
        largura_ = largura;
        altura_ = altura;
        angulo_ = angulo;
        recalcular();
    }

    void transladar(double dx, double dy) {
        centro_ += QPointF(dx, dy);
        recalcular();
    }

    void escalar(double sx, double sy) {
        largura_ *= sx;
        altura_ *= sy;
        recalcular();
    }

    void rotacionar(double graus, const QPointF& pivo) {
        centro_ = Matriz::rotacaoEmTorno(graus, pivo) * centro_;
        angulo_ += graus;
        recalcular();
    }

    // Configuração da câmera
    void definirProjecao(TipoProjecao p) { projecao_ = p; }

    void definirDistancia(double d) {
        if (d <= 1.0) return;
        d_ = d;
        recalcular();
    }

    void definirVPN(const Ponto3D &v) {
        if (std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z) < 1e-9) return;
        vpn_ = v;
        recalcular();
    }

    void rotacionar3D(Eixo eixo, double graus) {
        vpn_ = Matriz::rotacao(eixo, graus) * vpn_;
        recalcular();
    }

    QPointF centro() const { return centro_; }
    double largura() const { return largura_; }
    double altura() const { return altura_; }
    double angulo() const { return angulo_; }

    TipoProjecao projecao() const { return projecao_; }
    double distancia() const { return d_; }
    Ponto3D vpn() const { return vpn_; }

    const Matriz &matrizSCN() const { return scn_; }
    const Matriz &matrizVisao() const { return visao_; }
    const Matriz &matrizSCN2D() const { return scn2D_; }
};

#endif // JANELA_H
