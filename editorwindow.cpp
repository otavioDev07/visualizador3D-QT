#include "editorwindow.h"
#include "areavisualizacao.h"
#include "painelpropriedades.h"
#include "leitorobj.h"
#include <QHBoxLayout>
#include "matriz.h"
#include <QDebug>
#include <QStringList>
#include <QScrollArea>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <algorithm>
#include <iostream>

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

    QMenu *menuArquivo = menuBar()->addMenu("&Arquivo");
    QAction *acaoAbrir = menuArquivo->addAction("Carregar &OBJ...");
    acaoAbrir->setShortcut(QKeySequence::Open);
    connect(acaoAbrir, &QAction::triggered, this, &EditorWindow::abrirOBJ);

    setWindowTitle("Entrega 06 - Visualizador Geométrico");

    resize(1024, 768);

    carregarCenaTeste();

    areaVisualizacao->setDisplayFile(&displayFile);

    // Estado inicial da viewport.
    painelPropriedades->definirViewport(viewport.xmin(), viewport.ymin(),
                                        viewport.xmax(), viewport.ymax());
    areaVisualizacao->setViewport(viewport);
    atualizarSCN();   // calcula pela primeira vez as coordenadas normalizadas

    atualizarListaObjetos();

    // Quando o usuário seleciona um objeto no painel
    connect(painelPropriedades, &PainelPropriedades::objetoSelecionado,
            this, [this](int indice) {
                indiceSelecionado = indice;
            });

    auto centroideEfetivo = [this](const Objeto *obj) {
        const Ponto3D c = obj->centroide();
        return modoAcumular ? matrizAcumulada * c : c;
    };

    // Translação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::translacaoSolicitada,
            this, [this](double dx, double dy, double dz) {
                if (windowSelecionada()) {
                    displayFile.obterJanela().transladar(dx, dy);
                    atualizarSCN();
                    return;
                }
                executar(Matriz::translacao(dx, dy, dz));
            });

    // Escala do objeto selecionado, em torno do seu centroide
    connect(painelPropriedades, &PainelPropriedades::escalaSolicitada,
           this, [this, centroideEfetivo](double sx, double sy, double sz) {
                if (indiceSelecionado < 0) return;
                if (windowSelecionada()) {
                    displayFile.obterJanela().escalar(sx, sy);
                    atualizarSCN();
                    return;
                }
                Objeto *obj = objetoSelecionado();
                if (!obj) return;
                executar(Matriz::escalaEmTorno(sx, sy, sz, centroideEfetivo(obj)));
            });


    // Rotação do objeto selecionado
    connect(painelPropriedades, &PainelPropriedades::rotacaoSolicitada,
            this, [this, centroideEfetivo](int eixo, double graus, bool usarCentroide,
                                     double px, double py, double pz) {
                if (indiceSelecionado < 0) return;
                if (windowSelecionada()) {
                    if (eixo != static_cast<int>(Eixo::Z)) {
                        displayFile.obterJanela().rotacionar3D(static_cast<Eixo>(eixo), graus);
                        atualizarSCN();
                        return;
                    }
                    const QPointF pivo = usarCentroide
                                             ? displayFile.obterJanela().centro()
                                             : QPointF(px, py);
                    displayFile.obterJanela().rotacionar(graus, pivo);
                    atualizarSCN();
                    return;
                }
                Objeto *obj = objetoSelecionado();
                if (!obj) return;
                const Ponto3D centro = usarCentroide ? centroideEfetivo(obj) : Ponto3D(px, py, pz);
                executar(Matriz::rotacaoEmTorno(static_cast<Eixo>(eixo), graus, centro));
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
    // Troca de projeção e distância d
    connect(painelPropriedades, &PainelPropriedades::projecaoAlterada,
            this, [this](int tipo) {
                displayFile.obterJanela().definirProjecao(
                    tipo == 1 ? TipoProjecao::Perspectiva : TipoProjecao::Paralela);
                atualizarSCN();
            });

    connect(painelPropriedades, &PainelPropriedades::distanciaAlterada,
            this, [this](double d) {
                displayFile.obterJanela().definirDistancia(d);
                atualizarSCN();
            });

}

void EditorWindow::carregarCenaTeste() {
    // Coordenadas do MUNDO (eixo y para cima, origem no centro da window inicial).
    // Os objetos 2D da cena ficam no plano z = 0.

    Objeto casa("Casa", TipoObjeto::RETA, {{-320, -250}, {-100, -250},{-100, -250}, {-100, 0},{-100, 0}, {-210, 150},{-210, 150}, {-320, 0},{-320, 0}, {-320, -250}});
    displayFile.adicionarObjeto(casa);

    Objeto porta("Porta", TipoObjeto::RETA, {{-265, -250}, {-265, -115},{-175, -250}, {-175, -115},{-265, -115}, {-175, -115}});
    displayFile.adicionarObjeto(porta);

    Objeto janelaCasa("Janela da casa", TipoObjeto::RETA, {{-300, -35}, {-250, -35},{-250, -35}, {-250, 15},{-250, 15}, {-300, 15},{-300, 15}, {-300, -35}});
    displayFile.adicionarObjeto(janelaCasa);

    Objeto carro("Carro", TipoObjeto::RETA, {{70, -220}, {320, -220},{320, -220}, {320, -145},{320, -145}, {250, -145},{250, -145}, {210, -75},{210, -75}, {140, -75},{140, -75}, {100, -145},{100, -145}, {70, -145},{70, -145}, {70, -220}});
    displayFile.adicionarObjeto(carro);

    Objeto rodaEsquerda("Roda esquerda", TipoObjeto::RETA, {{115, -260}, {160, -260},{160, -260}, {160, -220},{160, -220}, {115, -220},{115, -220}, {115, -260}});
    displayFile.adicionarObjeto(rodaEsquerda);

    Objeto rodaDireita("Roda direita", TipoObjeto::RETA, {{245, -260}, {290, -260},{290, -260}, {290, -220},{290, -220}, {245, -220},{245, -220}, {245, -260}});
    displayFile.adicionarObjeto(rodaDireita);

    carregarModelo(":/modelos/charizard.obj", "Charizard", 260.0,
                   Ponto3D(200.0, 110.0, 0.0), QColor(220, 90, 0));
    carregarModelo(":/modelos/alien_animal.obj", "Alien Animal", 280.0,
                   Ponto3D(-210.0, 243.0, 0.0), QColor(120, 40, 160));
    carregarModelo(":/modelos/Pikachu OBJ.obj", "Pikachu", 220.0, Ponto3D(0.0, 100.0, 0.0), QColor(255, 220, 0));
}

bool EditorWindow::carregarModelo(const QString &caminho, const QString &nome, double tamanho,
                                  const Ponto3D &destino, const QColor &cor)
{
    LeitorOBJ::Malha malha;
    QString erro;
    if (!LeitorOBJ::ler(caminho, malha, erro)) {
        QMessageBox::warning(this, "Erro ao carregar OBJ", erro);
        return false;
    }

    const size_t nVertices = malha.vertices.size();
    const size_t nArestas = malha.arestas.size();

    Objeto obj(nome, TipoObjeto::MODELO, std::move(malha.vertices), std::move(malha.arestas));
    obj.cor = cor;
    obj.normalizar(tamanho, destino);
    displayFile.adicionarObjeto(obj);

    statusBar()->showMessage(QString("%1: %2 vértices, %3 arestas")
                                 .arg(nome).arg(nVertices).arg(nArestas), 6000);
    return true;
}

void EditorWindow::abrirOBJ()
{
    const QString caminho = QFileDialog::getOpenFileName(
        this, "Carregar modelo OBJ", QString(), "Modelos Wavefront (*.obj)");
    if (caminho.isEmpty())
        return;

    const Janela &janela = displayFile.obterJanela();
    const double tamanho = 0.4 * std::min(janela.largura(), janela.altura());
    const Ponto3D destino(janela.centro().x(), janela.centro().y(), 0.0);

    static const QColor paleta[] = { QColor(0, 120, 60), QColor(200, 30, 120),
                                    QColor(0, 110, 190), QColor(150, 100, 0) };
    const QColor cor = paleta[displayFile.quantidade() % 4];

    if (!carregarModelo(caminho, QFileInfo(caminho).completeBaseName(), tamanho, destino, cor))
        return;

    atualizarSCN();
    atualizarListaObjetos(static_cast<int>(displayFile.quantidade()));
}

void EditorWindow::atualizarListaObjetos(int selecionar)
{
    QStringList nomes;
    nomes << "Window";
    for (const auto& obj : displayFile.obterObjetos())
        nomes << obj.nome;
    painelPropriedades->definirObjetos(nomes);
    if (selecionar >= 0)
        painelPropriedades->selecionarObjeto(selecionar);
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
        displayFile.atualizarSCN(displayFile.obterJanela());;
    areaVisualizacao->update();
}

void EditorWindow::atualizarTextoMatriz()
{
    QString t;
    for (int i = 0; i < 4; i++) {
        t += QString("[%1 %2 %3 %4]\n")
        .arg(matrizAcumulada(i, 0), 8, 'f', 2)
            .arg(matrizAcumulada(i, 1), 8, 'f', 2)
            .arg(matrizAcumulada(i, 2), 8, 'f', 2)
            .arg(matrizAcumulada(i, 3), 8, 'f', 2);
    }
    painelPropriedades->mostrarMatriz(t);
}

EditorWindow::~EditorWindow() = default;
