#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include "displayfile.h"
#include "matriz.h"

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