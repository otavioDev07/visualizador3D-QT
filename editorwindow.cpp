#include "editorwindow.h"
#include "areavisualizacao.h"
#include "painelpropriedades.h"
#include "leitorobj.h"
#include <QHBoxLayout>
#include "matriz.h"
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

EditorWindow::EditorWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    areaVisualizacao = new AreaVisualizacao(this);
    painelPropriedades = new PainelPropriedades(this);

    layout->addWidget(areaVisualizacao, 3);
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

    setWindowTitle("Visualizador Geométrico 3D");

    resize(1024, 768);

    carregarCenaTeste();

    areaVisualizacao->setDisplayFile(&displayFile);

    connect(areaVisualizacao, &AreaVisualizacao::tamanhoAlterado,
            this, &EditorWindow::atualizarViewport);
    atualizarViewport(areaVisualizacao->size());
    atualizarSCN();   // calcula pela primeira vez as coordenadas normalizadas

    atualizarListaObjetos();

    connect(painelPropriedades, &PainelPropriedades::objetoSelecionado,
            this, [this](const QString &nome) {
                nomeSelecionado = nome;
                painelPropriedades->definirModoCamera(windowSelecionada());
                atualizarTextoMatriz();
            });

    connect(painelPropriedades, &PainelPropriedades::transformacaoAlterada,
            this, &EditorWindow::atualizarTextoMatriz);
    connect(painelPropriedades, &PainelPropriedades::limparComposicaoSolicitada,
            painelPropriedades, &PainelPropriedades::limparTransformacao);
    connect(painelPropriedades, &PainelPropriedades::aplicarComposicaoSolicitada,
            this, &EditorWindow::aplicarComposicao);
    atualizarTextoMatriz();

    // Mudou algum parâmetro da viewport: refaz a transformada de viewport
    connect(painelPropriedades, &PainelPropriedades::viewportAlterada,
            this, [this](double xmin, double ymin, double xmax, double ymax) {
                viewportSolicitada.definir(xmin, ymin, xmax, ymax);
                atualizarViewport(areaVisualizacao->size());
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

    const auto criarCasa = [](const QString &nome, double x) {
        Objeto casa(nome, TipoObjeto::RETA,
                    {{0, 0, 0}, {150, 0, 0}, {150, 110, 0}, {75, 170, 0}, {0, 110, 0},
                     {60, 0, 0}, {60, 60, 0}, {90, 60, 0}, {90, 0, 0},
                     {20, 50, 0}, {45, 50, 0}, {45, 80, 0}, {20, 80, 0},
                     {105, 50, 0}, {130, 50, 0}, {130, 80, 0}, {105, 80, 0}},
                    {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}, {2, 4},
                     {5, 6}, {6, 7}, {7, 8},
                     {9, 10}, {10, 11}, {11, 12}, {12, 9},
                     {13, 14}, {14, 15}, {15, 16}, {16, 13}});
        casa.transformar(Matriz::translacao(x, -225.0));
        return casa;
    };
    displayFile.adicionarObjeto(criarCasa("Casa 1", -320.0));
    displayFile.adicionarObjeto(criarCasa("Casa 2", -145.0));
    displayFile.adicionarObjeto(Objeto("Polígono", TipoObjeto::POLIGONO,
                                       {{-265, -292}, {-230, -270}, {-195, -292}, {-230, -314}}));
    displayFile.adicionarObjeto(Objeto("Ponto", TipoObjeto::PONTO, {{0, -292}}));

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
    carregarModelo(":/modelos/cubo.obj", "Cubo", 160.0,
                   Ponto3D(0.0, -80.0, 0.0), QColor(0, 170, 220));
}

QString EditorWindow::carregarModelo(const QString &caminho, const QString &nome, double tamanho,
                                  const Ponto3D &destino, const QColor &cor)
{
    LeitorOBJ::Malha malha;
    QString erro;
    if (!LeitorOBJ::ler(caminho, malha, erro)) {
        QMessageBox::warning(this, "Erro ao carregar OBJ", erro);
        return {};
    }

    const size_t nVertices = malha.vertices.size();
    const size_t nArestas = malha.arestas.size();

    Objeto obj(nome, TipoObjeto::MODELO, std::move(malha.vertices), std::move(malha.arestas));
    obj.cor = cor;
    if (!obj.normalizar(tamanho, destino)) {
        QMessageBox::warning(this, "Erro ao carregar OBJ",
                             tr("O modelo não tem dimensões válidas para ser exibido."));
        return {};
    }
    const Objeto &inserido = displayFile.adicionarObjeto(std::move(obj));

    statusBar()->showMessage(QString("%1: %2 vértices, %3 arestas")
                                 .arg(inserido.nome).arg(nVertices).arg(nArestas), 6000);
    return inserido.nome;
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

    const QString nome = carregarModelo(caminho, QFileInfo(caminho).completeBaseName(),
                                       tamanho, destino, cor);
    if (nome.isEmpty())
        return;

    atualizarSCN();
    atualizarListaObjetos(nome);
}

void EditorWindow::atualizarListaObjetos(const QString &selecionar)
{
    QStringList nomes;
    nomes << DisplayFile::nomeJanela();
    for (const auto& obj : displayFile.obterObjetos())
        nomes << obj.nome;
    painelPropriedades->definirObjetos(nomes);
    if (!selecionar.isEmpty())
        painelPropriedades->selecionarObjeto(selecionar);
}

Matriz EditorWindow::comporTransformacao(const Transformacao &t)
{
    if (windowSelecionada()) {
        const Janela &janela = displayFile.obterJanela();
        const QPointF pivo = t.usarCentroide
            ? janela.centro() + QPointF(t.deslocamento.x, t.deslocamento.y)
            : QPointF(t.pivo.x, t.pivo.y);
        return janela.comporTransformacao(t.deslocamento, t.escala, t.angulos, pivo);
    }

    const Objeto *obj = objetoSelecionado();
    if (!obj)
        return Matriz();

    const Matriz translacao = Matriz::translacao(t.deslocamento.x, t.deslocamento.y,
                                                t.deslocamento.z);
    const Ponto3D centroDeslocado = translacao * obj->centroide();
    const Matriz escala = Matriz::escalaEmTorno(t.escala.x, t.escala.y,
                                               t.escala.z, centroDeslocado);
    const Ponto3D pivo = t.usarCentroide ? centroDeslocado : t.pivo;
    const Matriz rotacao = Matriz::translacao(pivo.x, pivo.y, pivo.z)
                            * Matriz::rotacao(Eixo::Z, t.angulos.z)
                            * Matriz::rotacao(Eixo::Y, t.angulos.y)
                            * Matriz::rotacao(Eixo::X, t.angulos.x)
                            * Matriz::translacao(-pivo.x, -pivo.y, -pivo.z);
    return rotacao * escala * translacao;
}

void EditorWindow::aplicarComposicao()
{
    Objeto *obj = objetoSelecionado();
    if (!windowSelecionada() && !obj)
        return;

    const Matriz composta = comporTransformacao(painelPropriedades->transformacao());
    if (obj) {
        obj->transformar(composta);
    } else {
        if (!displayFile.obterJanela().aplicarComposicao(composta)) {
            statusBar()->showMessage(tr("A transformação excede os limites numéricos da câmera."), 6000);
            return;
        }
    }
    atualizarTextoMatriz();
    atualizarSCN();
}

void EditorWindow::atualizarSCN()
{
    displayFile.atualizarSCN(displayFile.obterJanela());
    areaVisualizacao->update();
}

void EditorWindow::atualizarViewport(const QSize &tamanho)
{
    // Mantem a moldura dentro do ultimo pixel disponivel em cada eixo.
    const double limiteX = tamanho.width() - 1;
    const double limiteY = tamanho.height() - 1;
    const double xmin = std::min(viewportSolicitada.xmin(), limiteX - 1);
    const double ymin = std::min(viewportSolicitada.ymin(), limiteY - 1);
    const double xmax = std::min(viewportSolicitada.xmax(), limiteX);
    const double ymax = std::min(viewportSolicitada.ymax(), limiteY);
    Viewport efetiva;
    efetiva.definir(xmin, ymin, xmax, ymax);
    areaVisualizacao->setViewport(efetiva);
    painelPropriedades->definirViewport(xmin, ymin, xmax, ymax);
}

void EditorWindow::atualizarTextoMatriz()
{
    const Matriz composta = comporTransformacao(painelPropriedades->transformacao());
    QString t;
    for (int i = 0; i < 4; ++i) {
        t += '[';
        for (int j = 0; j < 4; ++j) {
            const double valor = composta(i, j);
            t += QString("%1").arg(std::abs(valor) < 0.005 ? 0.0 : valor, 8, 'f', 2);
        }
        t += i == 3 ? "]" : "]\n";
    }
    painelPropriedades->mostrarMatriz(t);
}

EditorWindow::~EditorWindow() = default;
