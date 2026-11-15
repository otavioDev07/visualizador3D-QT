#ifndef LEITOROBJ_H
#define LEITOROBJ_H

#include <QString>
#include <vector>
#include <utility>
#include "matriz.h"

namespace LeitorOBJ {

struct Malha {
    std::vector<Ponto3D> vertices;
    std::vector<std::pair<int, int>> arestas;
};

bool ler(const QString &caminho, Malha &malha, QString &erro);

}

#endif // LEITOROBJ_H
