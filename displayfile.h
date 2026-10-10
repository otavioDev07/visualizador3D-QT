#ifndef DISPLAYFILE_H
#define DISPLAYFILE_H

#include "objeto.h"
#include <vector>

class DisplayFile {
private:
    std::vector<Objeto> objetos;

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

    Objeto* obterObjeto(size_t indice) {
        if (indice >= objetos.size()) return nullptr;
        return &objetos[indice];
    }
};

#endif // DISPLAYFILE_H