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
};

#endif // DISPLAYFILE_H