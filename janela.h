#ifndef JANELA_H
#define JANELA_H

#include <QPointF>
#include "matriz.h"

// "Window" do mundo (retângulo de recorte/visão no sistema de coordenadas do mundo).
// Chamada de Janela para não confundir com QWindow/QMainWindow do Qt.
class Janela {
private:
    QPointF centro_ {0.0, 0.0};
    double largura_ = 700.0;
    double altura_  = 700.0;
    double angulo_  = 0.0;      // graus, anti-horário (y para cima)

    Matriz scn_;                // cache da transformada de normalização

    void recalcular() {
        // SCN = S(2/W, 2/H) * R(-ângulo) * T(-cx, -cy)
        //  1) leva o centro da window para a origem
        //  2) desfaz a rotação da window (ela passa a ficar alinhada aos eixos)
        //  3) escala para que a window vire o quadrado [-1,1] x [-1,1]
        scn_ = Matriz::escala(2.0 / largura_, 2.0 / altura_)
               * Matriz::rotacao(-angulo_)
               * Matriz::translacao(-centro_.x(), -centro_.y());
    }

public:
    Janela() { recalcular(); }

    void definir(const QPointF &centro, double largura, double altura, double angulo) {
        centro_ = centro;
        largura_ = largura;
        altura_ = altura;
        angulo_ = angulo;
        recalcular();           // a cada mudança, o SCN é refeito
    }

    QPointF centro() const { return centro_; }
    double largura() const { return largura_; }
    double altura() const { return altura_; }
    double angulo() const { return angulo_; }

    const Matriz &matrizSCN() const { return scn_; }
};

#endif // JANELA_H
