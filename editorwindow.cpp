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

    setWindowTitle("Entrega 04 - Visualizador Geométrico");
    resize(1024, 768);

    carregarCenaTeste();

    areaVisualizacao->setDisplayFile(&displayFile);

    // Estado inicial da viewport.
    painelPropriedades->definirViewport(viewport.xmin(), viewport.ymin(),
                                        viewport.xmax(), viewport.ymax());
    areaVisualizacao->setViewport(viewport);
    atualizarSCN();   // calcula pela primeira vez as coordenadas normalizadas

    QStringList nomes;
    nomes << "Window";
    for (const auto& obj : displayFile.obterObjetos())
        nomes << obj.nome;
    painelPropriedades->definirObjetos(nomes);

    // Quando o usuário seleciona um objeto no painel
    connect(painelPropriedades, &PainelPropriedades::objetoSelecionado,
            this, [this](int indice) {
                indiceSelecionado = indice;
            });

    auto centroideEfetivo = [this](const Objeto *obj) {
        const QPointF c = obj->centroide();
        return modoAcumular ? matrizAcumulada * c : c;
    };

    // Translação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::translacaoSolicitada,
            this, [this](double dx, double dy) {
                if (windowSelecionada()) {
                    displayFile.obterJanela().transladar(dx, dy);
                    atualizarSCN();
                    return;
                }
                executar(Matriz::translacao(dx, dy));
            });

    // Escala do objeto selecionado, em torno do seu centroide
    connect(painelPropriedades, &PainelPropriedades::escalaSolicitada,
            this, [this, centroideEfetivo](double sx, double sy) {
                if (indiceSelecionado < 0) return;
                if (windowSelecionada()) {
                    displayFile.obterJanela().escalar(sx, sy);
                    atualizarSCN();
                    return;
                }
                Objeto *obj = objetoSelecionado();
                if (!obj) return;
                executar(Matriz::escalaEmTorno(sx, sy, centroideEfetivo(obj)));
            });


    // Rotação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::rotacaoSolicitada,
            this, [this, centroideEfetivo](double graus, bool usarCentroide, double px, double py) {
                if (indiceSelecionado < 0) return;
                if (windowSelecionada()) {
                    const QPointF pivo = usarCentroide
                        ? displayFile.obterJanela().centro()
                        : QPointF(px, py);
                    displayFile.obterJanela().rotacionar(graus, pivo);
                    atualizarSCN();
                    return;
                }
                Objeto *obj = objetoSelecionado();
                if (!obj) return;
                QPointF centro = usarCentroide ? centroideEfetivo(obj) : QPointF(px, py);
                executar(Matriz::rotacaoEmTorno(graus, centro));   // y para cima: positivo = anti-horário
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
                Objeto *obj = objetoSelecionado();
                if (!obj) return;
                obj->transformar(matrizAcumulada);   // UMA multiplicação por vértice
                matrizAcumulada = Matriz();
                atualizarTextoMatriz();
                atualizarSCN();
            });

}

void EditorWindow::carregarCenaTeste() {
    // Coordenadas do MUNDO (eixo y para cima, origem no centro da window inicial).

    Objeto casa("Casa", TipoObjeto::RETA, {{-320, -250}, {-100, -250},{-100, -250}, {-100, 0},{-100, 0}, {-210, 150},{-210, 150}, {-320, 0},{-320, 0}, {-320, -250}});
    displayFile.adicionarObjeto(casa);

    Objeto porta("Porta", TipoObjeto::RETA, {{-265, -250}, {-265, -115},{-175, -250}, {-175, -115},{-265, -115}, {-175, -115}});
    displayFile.adicionarObjeto(porta);

    Objeto janelaCasa("Janela da casa", TipoObjeto::RETA, {{-300, -35}, {-250, -35},{-250, -35}, {-250, 15},{-250, 15}, {-300, 15},{-300, 15}, {-300, -35}});
    displayFile.adicionarObjeto(janelaCasa);

    Objeto tronco("Tronco da palmeira", TipoObjeto::RETA, {{-25, -250}, {-25, 70}});
    displayFile.adicionarObjeto(tronco);

    Objeto folhas("Folhas da palmeira", TipoObjeto::RETA, {{-25, 70}, {-125, 120},{-25, 70}, {-85, 185},{-25, 70}, {-25, 210},{-25, 70}, {40, 185},{-25, 70}, {85, 120},{-25, 70}, {105, 60}});
    displayFile.adicionarObjeto(folhas);

    Objeto carro("Carro", TipoObjeto::RETA, {{70, -220}, {320, -220},{320, -220}, {320, -145},{320, -145}, {250, -145},{250, -145}, {210, -75},{210, -75}, {140, -75},{140, -75}, {100, -145},{100, -145}, {70, -145},{70, -145}, {70, -220}});
    displayFile.adicionarObjeto(carro);

    Objeto rodaEsquerda("Roda esquerda", TipoObjeto::RETA, {{115, -260}, {160, -260},{160, -260}, {160, -220},{160, -220}, {115, -220},{115, -220}, {115, -260}});
    displayFile.adicionarObjeto(rodaEsquerda);

    Objeto rodaDireita("Roda direita", TipoObjeto::RETA, {{245, -260}, {290, -260},{290, -260}, {290, -220},{290, -220}, {245, -220},{245, -220}, {245, -260}});
    displayFile.adicionarObjeto(rodaDireita);
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
    Objeto *obj = objetoSelecionado();
    if (!obj) return;
    obj->transformar(m);
    atualizarSCN();
}

void EditorWindow::atualizarSCN()
{
    displayFile.atualizarSCN(displayFile.obterJanela().matrizSCN());
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
