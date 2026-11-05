#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include "displayfile.h"
#include "matriz.h"
#include "janela.h"
#include "viewport.h"

class AreaVisualizacao;
class PainelPropriedades;

class EditorWindow : public QMainWindow
{
    Q_OBJECT

private:
    DisplayFile displayFile;
    AreaVisualizacao *areaVisualizacao = nullptr;
    PainelPropriedades *painelPropriedades = nullptr;
    int indiceSelecionado = -1;

    bool windowSelecionada() const {
        return indiceSelecionado == 0;
    }

    Objeto* objetoSelecionado() {
        if (indiceSelecionado <= 0)
            return nullptr;

        return displayFile.obterObjeto(
            static_cast<size_t>(indiceSelecionado - 1));
    }

    Viewport viewport;      // viewport em pixels
    void atualizarSCN();    // recalcula o SCN de todos os objetos e redesenha

    bool modoAcumular = false;
    Matriz matrizAcumulada;
    void atualizarTextoMatriz();
    void executar(const Matriz &m);
    void carregarCenaTeste();

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow() override;
};
#endif // EDITORWINDOW_H