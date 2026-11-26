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
    QString nomeSelecionado;

    bool windowSelecionada() const {
        return nomeSelecionado == DisplayFile::nomeJanela();
    }

    Objeto* objetoSelecionado() {
        return displayFile.obterObjeto(nomeSelecionado);
    }

    Viewport viewportSolicitada;
    void atualizarViewport(const QSize &tamanho);
    void atualizarSCN();    // recalcula o SCN de todos os objetos e redesenha

    void atualizarTextoMatriz();
    Matriz comporTransformacao(const Transformacao &t);
    void aplicarComposicao();
    void carregarCenaTeste();

    QString carregarModelo(const QString &caminho, const QString &nome, double tamanho,
                        const Ponto3D &destino, const QColor &cor);
    void abrirOBJ();
    void atualizarListaObjetos(const QString &selecionar = {});

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow() override;
};
#endif // EDITORWINDOW_H
