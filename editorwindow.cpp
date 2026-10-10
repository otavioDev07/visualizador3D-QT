#include "editorwindow.h"
#include "areavisualizacao.h"
#include "painelpropriedades.h"
#include <QHBoxLayout>
#include "matriz.h"
#include <QDebug>
#include <QStringList>

EditorWindow::EditorWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    areaVisualizacao = new AreaVisualizacao(this);
    painelPropriedades = new PainelPropriedades(this);

    layout->addWidget(areaVisualizacao, 3);
    layout->addWidget(painelPropriedades, 1);
    layout->setContentsMargins(0,0,0,0);
    setCentralWidget(centralWidget);

    setWindowTitle("Entrega 01 - Visualizador Geométrico");
    resize(1024, 768);

    carregarCenaTeste();

    areaVisualizacao->setDisplayFile(&displayFile);

    QStringList nomes;
    for (const auto& obj : displayFile.obterObjetos())
        nomes << obj.nome;
    painelPropriedades->definirObjetos(nomes);

    // Quando o usuário seleciona um objeto no painel
    connect(painelPropriedades, &PainelPropriedades::objetoSelecionado,
            this, [this](int indice) {
                indiceSelecionado = indice;
                areaVisualizacao->setIndiceSelecionado(indice);
            });

    // Translação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::translacaoSolicitada,
            this, [this](double dx, double dy) {
                executar(Matriz::translacao(dx, dy));
            });

    // Escala do objeto selecionado, em torno do seu centroide
    connect(painelPropriedades, &PainelPropriedades::escalaSolicitada,
            this, [this](double sx, double sy) {
                if (indiceSelecionado < 0) return;
                Objeto *obj = displayFile.obterObjeto(static_cast<size_t>(indiceSelecionado));
                if (!obj) return;
                executar(Matriz::escalaEmTorno(sx, sy, obj->centroide()));
            });


    // Rotação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::rotacaoSolicitada,
            this, [this](double graus, bool usarCentroide, double px, double py) {
                if (indiceSelecionado < 0) return;
                Objeto *obj = displayFile.obterObjeto(static_cast<size_t>(indiceSelecionado));
                if (!obj) return;
                QPointF centro = usarCentroide ? obj->centroide() : QPointF(px, py);
                executar(Matriz::rotacaoEmTorno(-graus, centro));
            });

    connect(painelPropriedades, &PainelPropriedades::modoAcumularAlterado,
            this, [this](bool ativo) {
                modoAcumular = ativo;
                matrizAcumulada = Matriz();   // reinicia ao trocar de modo
                atualizarTextoMatriz();
            });

    connect(painelPropriedades, &PainelPropriedades::limparComposicaoSolicitada,
            this, [this]() {
                matrizAcumulada = Matriz();
                atualizarTextoMatriz();
            });

    connect(painelPropriedades, &PainelPropriedades::aplicarComposicaoSolicitada,
            this, [this]() {
                if (indiceSelecionado < 0) return;
                Objeto *obj = displayFile.obterObjeto(static_cast<size_t>(indiceSelecionado));
                if (!obj) return;
                obj->transformar(matrizAcumulada);   // UMA multiplicação por vértice
                matrizAcumulada = Matriz();
                atualizarTextoMatriz();
                areaVisualizacao->update();
            });

}

void EditorWindow::carregarCenaTeste() {
    Objeto p1("Meus_Pontos", TipoObjeto::PONTO, {{100, 100}, {150, 100}});
    displayFile.adicionarObjeto(p1);

    Objeto r1("Reta", TipoObjeto::RETA, {{50, 200}, {300, 200}});
    displayFile.adicionarObjeto(r1);

    Objeto pol1("Triangulo", TipoObjeto::POLIGONO, {{200, 300}, {300, 450}, {100, 450}});
    displayFile.adicionarObjeto(pol1);

    Objeto pol2("Quadrado", TipoObjeto::POLIGONO, {{200, 500}, {400, 500}, {400, 700}, {200, 700}});
    displayFile.adicionarObjeto(pol2);
}

void EditorWindow::executar(const Matriz &m)
{
    if (modoAcumular) {
        // A operação mais recente entra à esquerda: M = nova * acumulada
        matrizAcumulada = m * matrizAcumulada;
        atualizarTextoMatriz();
        return;
    }
    if (indiceSelecionado < 0) return;
    Objeto *obj = displayFile.obterObjeto(static_cast<size_t>(indiceSelecionado));
    if (!obj) return;
    obj->transformar(m);
    areaVisualizacao->update();
}

void EditorWindow::atualizarTextoMatriz()
{
    QString t;
    for (int i = 0; i < 3; i++) {
        t += QString("[%1 %2 %3]\n")
        .arg(matrizAcumulada(i, 0), 8, 'f', 2)
            .arg(matrizAcumulada(i, 1), 8, 'f', 2)
            .arg(matrizAcumulada(i, 2), 8, 'f', 2);
    }
    painelPropriedades->mostrarMatriz(t);
}

EditorWindow::~EditorWindow() = default;