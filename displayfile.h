#ifndef DISPLAYFILE_H
#define DISPLAYFILE_H

#include "objeto.h"
#include "janela.h"
#include <vector>

class DisplayFile {
private:
    std::vector<Objeto> objetos;
    Janela janela;

public:
    DisplayFile() = default;

    void adicionarObjeto(const Objeto& obj) {
        objetos.push_back(obj);
    }

    const std::vector<Objeto>& obterObjetos() const {
        return objetos;
    }

    size_t quantidade() const {
        return objetos.size();
    }

    void atualizarSCN(const Janela& j) {
        if (j.projecao() == TipoProjecao::Perspectiva) {
            const Matriz mper = Matriz::perspectiva(j.distancia());
            for (auto& obj : objetos)
                obj.atualizarSCNPerspectiva(j.matrizVisao(), mper, j.matrizSCN2D(),
                                             j.largura(), j.altura(), j.distancia());
        } else {
            const Matriz pipeline = j.matrizSCN2D() * Matriz::projecaoOrtogonal() * j.matrizVisao();
            for (auto& obj : objetos)
                obj.atualizarSCN(pipeline);
        }
    }

    Objeto* obterObjeto(size_t indice) {
        if (indice >= objetos.size()) return nullptr;
        return &objetos[indice];
    }

    Janela& obterJanela() {
        return janela;
    }

    const Janela& obterJanela() const {
        return janela;
    }


};

#endif // DISPLAYFILE_H
