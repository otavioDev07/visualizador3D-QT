#include "editorwindow.h"
#include "areavisualizacao.h"
#include "painelpropriedades.h"
#include <QHBoxLayout>
#include "matriz.h"
#include <QDebug>
#include <QStringList>
#include <QScrollArea>

EditorWindow::EditorWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    areaVisualizacao = new AreaVisualizacao(this);
    painelPropriedades = new PainelPropriedades(this);

    layout->addWidget(areaVisualizacao, 3);
    // O painel ficou alto: coloca-o numa área com rolagem
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidget(painelPropriedades);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(scroll, 1);
    layout->setContentsMargins(0,0,0,0);
    setCentralWidget(centralWidget);

    setWindowTitle("Entrega 03 - Visualizador Geométrico");
    resize(1024, 768);

    carregarCenaTeste();

    areaVisualizacao->setDisplayFile(&displayFile);

    // Estado inicial de window e viewport (painel e modelo começam iguais)
    painelPropriedades->definirWindow(janela.centro().x(), janela.centro().y(),
                                      janela.largura(), janela.altura(), janela.angulo());
    painelPropriedades->definirViewport(viewport.xmin(), viewport.ymin(),
                                        viewport.xmax(), viewport.ymax());
    areaVisualizacao->setViewport(viewport);
    atualizarSCN();   // calcula pela primeira vez as coordenadas normalizadas

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

    // Centroide "efetivo" do objeto: no modo Acumular as transformações ainda não foram
    // aplicadas aos pontos, então o centroide precisa passar pela matriz acumulada.
    auto centroideEfetivo = [this](const Objeto *obj) {
        const QPointF c = obj->centroide();
        return modoAcumular ? matrizAcumulada * c : c;
    };

    // Translação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::translacaoSolicitada,
            this, [this](double dx, double dy) {
                executar(Matriz::translacao(dx, dy));
            });

    // Escala do objeto selecionado, em torno do seu centroide
    connect(painelPropriedades, &PainelPropriedades::escalaSolicitada,
            this, [this, centroideEfetivo](double sx, double sy) {
                if (indiceSelecionado < 0) return;
                Objeto *obj = displayFile.obterObjeto(static_cast<size_t>(indiceSelecionado));
                if (!obj) return;
                executar(Matriz::escalaEmTorno(sx, sy, centroideEfetivo(obj)));
            });


    // Rotação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::rotacaoSolicitada,
            this, [this, centroideEfetivo](double graus, bool usarCentroide, double px, double py) {
                if (indiceSelecionado < 0) return;
                Objeto *obj = displayFile.obterObjeto(static_cast<size_t>(indiceSelecionado));
                if (!obj) return;
                QPointF centro = usarCentroide ? centroideEfetivo(obj) : QPointF(px, py);
                executar(Matriz::rotacaoEmTorno(graus, centro));   // y para cima: positivo = anti-horário
            });

    // Mudou algum parâmetro da window: refaz o SCN de todos os objetos
    connect(painelPropriedades, &PainelPropriedades::windowAlterada,
            this, [this](double cx, double cy, double largura, double altura, double angulo) {
                janela.definir(QPointF(cx, cy), largura, altura, angulo);
                atualizarSCN();
            });

    // Mudou algum parâmetro da viewport: refaz a transformada de viewport
    connect(painelPropriedades, &PainelPropriedades::viewportAlterada,
            this, [this](double xmin, double ymin, double xmax, double ymax) {
                if (xmax <= xmin || ymax <= ymin) return;   // viewport inválida
                viewport.definir(xmin, ymin, xmax, ymax);
                areaVisualizacao->setViewport(viewport);
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
                atualizarSCN();
            });

}

void EditorWindow::carregarCenaTeste() {
    // Coordenadas do MUNDO (eixo y para cima, origem no centro da window inicial).
    Objeto p1("Meus_Pontos", TipoObjeto::PONTO, {{-125, 300}, {-75, 300}});
    displayFile.adicionarObjeto(p1);

    Objeto r1("Reta", TipoObjeto::RETA, {{-175, 200}, {75, 200}});
    displayFile.adicionarObjeto(r1);

    Objeto pol1("Triangulo", TipoObjeto::POLIGONO, {{-25, 100}, {75, -50}, {-125, -50}});
    displayFile.adicionarObjeto(pol1);

    Objeto pol2("Quadrado", TipoObjeto::POLIGONO, {{-25, -100}, {175, -100}, {175, -300}, {-25, -300}});
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
    atualizarSCN();
}

void EditorWindow::atualizarSCN()
{
    displayFile.atualizarSCN(janela.matrizSCN());
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