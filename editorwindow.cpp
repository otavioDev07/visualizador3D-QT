#include "editorwindow.h"
#include "areavisualizacao.h"
#include "painelpropriedades.h"
#include <QHBoxLayout>

EditorWindow::EditorWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    AreaVisualizacao *areaVisualizacao = new AreaVisualizacao(this);
    PainelPropriedades *painelPropriedades = new PainelPropriedades(this);

    layout->addWidget(areaVisualizacao, 3);
    layout->addWidget(painelPropriedades, 1);
    layout->setContentsMargins(0,0,0,0);
    setCentralWidget(centralWidget);

    setWindowTitle("Entrega 01 - Visualizador Geométrico");
    resize(1024, 768);

    carregarCenaTeste();

    areaVisualizacao->setDisplayFile(&displayFile);
}

void EditorWindow::carregarCenaTeste() {
    Objeto p1("Meus_Pontos", TipoObjeto::PONTO, {{100, 100}, {150, 100}});
    displayFile.adicionarObjeto(p1);

    Objeto r1("Eixo", TipoObjeto::RETA, {{50, 200}, {300, 200}});
    displayFile.adicionarObjeto(r1);

    Objeto pol1("Triangulo", TipoObjeto::POLIGONO, {
                                                       {200, 300}, {300, 450}, {100, 450}
                                                   });
    displayFile.adicionarObjeto(pol1);
}

EditorWindow::~EditorWindow() = default;