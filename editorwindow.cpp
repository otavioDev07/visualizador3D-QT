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

    setWindowTitle("Entrega 0");
    resize(1024, 768);
}

EditorWindow::~EditorWindow() = default;




