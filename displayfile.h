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

    void atualizarSCN(const Matriz& scn) {
        for (auto& obj : objetos)
            obj.atualizarSCN(scn);
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