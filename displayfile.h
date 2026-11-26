#ifndef DISPLAYFILE_H
#define DISPLAYFILE_H

#include "objeto.h"
#include "janela.h"
#include <algorithm>
#include <list>
#include <utility>

class DisplayFile {
private:
    std::list<Objeto> objetos;
    Janela janela;

public:
    DisplayFile() = default;

    static QString nomeJanela() { return QStringLiteral("Window"); }

    const Objeto& adicionarObjeto(Objeto obj) {
        const QString base = obj.nome.trimmed().isEmpty() ? QStringLiteral("Objeto") : obj.nome;
        obj.nome = base;
        for (int sufixo = 2; obj.nome == nomeJanela() || obterObjeto(obj.nome); ++sufixo)
            obj.nome = QStringLiteral("%1 (%2)").arg(base).arg(sufixo);

        objetos.push_back(std::move(obj));
        return objetos.back();
    }

    const std::list<Objeto>& obterObjetos() const {
        return objetos;
    }

    size_t quantidade() const {
        return objetos.size();
    }

    void atualizarSCN(const Janela& j) {
        // Mundo -> coordenadas da camera -> projecao -> SCN.
        // Em perspectiva, o recorte volumetrico precede a divisao homogenea.
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

    Objeto* obterObjeto(const QString &nome) {
        const auto encontrado = std::find_if(objetos.begin(), objetos.end(), [&](const Objeto &obj) {
            return obj.nome == nome;
        });
        return encontrado == objetos.end() ? nullptr : &*encontrado;
    }

    Janela& obterJanela() {
        return janela;
    }

    const Janela& obterJanela() const {
        return janela;
    }


};

#endif // DISPLAYFILE_H
