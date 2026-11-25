#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include "displayfile.h"
#include "matriz.h"
#include "janela.h"
#include "viewport.h"

class AreaVisualizacao;
class PainelPropriedades;
struct Transformacao;

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

    Viewport viewportSolicitada;
    void atualizarViewport(const QSize &tamanho);
    void atualizarSCN();    // recalcula o SCN de todos os objetos e redesenha

    void atualizarTextoMatriz();
    Matriz comporTransformacao(const Transformacao &t);
    void aplicarComposicao();
    void carregarCenaTeste();

    bool carregarModelo(const QString &caminho, const QString &nome, double tamanho,
                        const Ponto3D &destino, const QColor &cor);
    void abrirOBJ();
    void atualizarListaObjetos(int selecionar = -1);

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow() override;
};
#endif // EDITORWINDOW_H
