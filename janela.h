#ifndef JANELA_H
#define JANELA_H

#include "matriz.h"

enum class TipoProjecao { Paralela, Perspectiva };

class Janela {
private:
    // Colunas: u * largura, v * altura, n (VPN) e CW (centro da Window).
    Matriz quadro_ = Matriz::escala(700.0, 700.0);
    Matriz orientacao_;
    Matriz orientacaoInversa_;
    Matriz visaoParalela_;
    Matriz visaoPerspectiva_;
    Matriz scn2D_;
    double largura_ = 700.0;
    double altura_ = 700.0;

    TipoProjecao projecao_ = TipoProjecao::Paralela;
    double d_ = 800.0;

    static Ponto3D produtoVetorial(const Ponto3D &a, const Ponto3D &b) {
        return Ponto3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    double comprimentoEixo(int eixo) const {
        return std::hypot(quadro_(0, eixo), quadro_(1, eixo), quadro_(2, eixo));
    }

    void recalcular() {
        largura_ = comprimentoEixo(0);
        altura_ = comprimentoEixo(1);
        const double comprimentos[] = {largura_, altura_, comprimentoEixo(2)};
        for (int linha = 0; linha < 3; ++linha) {
            for (int coluna = 0; coluna < 3; ++coluna) {
                orientacao_(linha, coluna) = quadro_(linha, coluna) / comprimentos[coluna];
                orientacaoInversa_(coluna, linha) = orientacao_(linha, coluna);
            }
        }

        // O projeto usa CW como VRP. Na perspectiva, COP = VRP - d * VPN.
        const Ponto3D vrp(quadro_(0, 3), quadro_(1, 3), quadro_(2, 3));
        const Ponto3D vpn(orientacao_(0, 2), orientacao_(1, 2), orientacao_(2, 2));
        const Ponto3D cop(vrp.x - d_ * vpn.x, vrp.y - d_ * vpn.y, vrp.z - d_ * vpn.z);

        // Translada a origem de projecao e alinha os eixos u, v, n a X, Y, Z.
        visaoParalela_ = orientacaoInversa_ * Matriz::translacao(-vrp.x, -vrp.y, -vrp.z);
        visaoPerspectiva_ = orientacaoInversa_ * Matriz::translacao(-cop.x, -cop.y, -cop.z);
        scn2D_ = Matriz::escala(2.0 / largura_, 2.0 / altura_);
    }

public:
    Janela() { recalcular(); }

    Matriz comporTransformacao(const Ponto3D &deslocamento, const Ponto3D &escala,
                               const Ponto3D &angulos, const QPointF &pivo) const {
        const QPointF centroDeslocado = centro() + QPointF(deslocamento.x, deslocamento.y);
        const QPointF novoCentro = Matriz::rotacaoEmTorno(angulos.z, pivo) * centroDeslocado;

        // X e Y inclinam nos eixos de mundo. Z gira em torno da normal da camera.
        const Matriz orientacaoNova = Matriz::rotacao(Eixo::Y, angulos.y)
                                      * Matriz::rotacao(Eixo::X, angulos.x)
                                      * orientacao_
                                      * Matriz::rotacao(Eixo::Z, angulos.z);

        // Expressa o zoom nos eixos atuais da camera, inclusive apos girar.
        return Matriz::translacao(novoCentro.x(), novoCentro.y())
               * orientacaoNova
               * Matriz::escala(escala.x, escala.y)
               * orientacaoInversa_
               * Matriz::translacao(-quadro_(0, 3), -quadro_(1, 3));
    }

    bool aplicarComposicao(const Matriz &composta) {
        Matriz novoQuadro = composta * quadro_;
        const double largura = std::hypot(novoQuadro(0, 0), novoQuadro(1, 0), novoQuadro(2, 0));
        const double altura = std::hypot(novoQuadro(0, 1), novoQuadro(1, 1), novoQuadro(2, 1));
        if (largura <= 0.0 || altura <= 0.0 || !std::isfinite(largura) || !std::isfinite(altura)
            || !std::isfinite(2.0 / largura) || !std::isfinite(2.0 / altura)
            || !std::isfinite(novoQuadro(0, 3)) || !std::isfinite(novoQuadro(1, 3))
            || !std::isfinite(novoQuadro(2, 3)))
            return false;

        const Ponto3D direita(novoQuadro(0, 0) / largura, novoQuadro(1, 0) / largura,
                             novoQuadro(2, 0) / largura);
        const Ponto3D cima(novoQuadro(0, 1) / altura, novoQuadro(1, 1) / altura,
                          novoQuadro(2, 1) / altura);

        // Reconstroi uma base ortonormal antes de usar a transposta como inversa.
        Ponto3D normal = produtoVetorial(direita, cima);
        const double comprimento = std::hypot(normal.x, normal.y, normal.z);
        if (!(comprimento > 0.0) || !std::isfinite(comprimento))
            return false;
        normal = Ponto3D(normal.x / comprimento, normal.y / comprimento, normal.z / comprimento);
        const Ponto3D cimaOrtonormal = produtoVetorial(normal, direita);

        novoQuadro(0, 1) = cimaOrtonormal.x * altura;
        novoQuadro(1, 1) = cimaOrtonormal.y * altura;
        novoQuadro(2, 1) = cimaOrtonormal.z * altura;
        novoQuadro(0, 2) = normal.x;
        novoQuadro(1, 2) = normal.y;
        novoQuadro(2, 2) = normal.z;
        quadro_ = novoQuadro;
        recalcular();
        return true;
    }

    void definirProjecao(TipoProjecao p) { projecao_ = p; }

    void definirDistancia(double d) {
        if (d <= 1.0) return;
        d_ = d;
        recalcular();
    }

    QPointF centro() const { return QPointF(quadro_(0, 3), quadro_(1, 3)); }
    double largura() const { return largura_; }
    double altura() const { return altura_; }

    TipoProjecao projecao() const { return projecao_; }
    double distancia() const { return d_; }

    const Matriz &matrizVisao() const {
        return projecao_ == TipoProjecao::Paralela ? visaoParalela_ : visaoPerspectiva_;
    }
    const Matriz &matrizSCN2D() const { return scn2D_; }
};

#endif // JANELA_H
