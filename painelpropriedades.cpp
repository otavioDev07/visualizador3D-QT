#include "painelpropriedades.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>

PainelPropriedades::PainelPropriedades(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Lista de objetos ---
    layout->addWidget(new QLabel("Objetos", this));
    listaObjetos = new QListWidget(this);
    listaObjetos->setMinimumHeight(140);
    layout->addWidget(listaObjetos);

    connect(listaObjetos, &QListWidget::currentRowChanged,
            this, &PainelPropriedades::objetoSelecionado);

    // --- Translação ---
    QGroupBox *grupoTrans = new QGroupBox("Translação", this);
    QFormLayout *formTrans = new QFormLayout(grupoTrans);

    spinDx = new QDoubleSpinBox(grupoTrans);
    spinDy = new QDoubleSpinBox(grupoTrans);
    spinDz = new QDoubleSpinBox(grupoTrans);
    for (QDoubleSpinBox *s : {spinDx, spinDy, spinDz}) {
        s->setRange(-10000.0, 10000.0);
        s->setDecimals(1);
        s->setValue(0.0);
    }
    formTrans->addRow("dx:", spinDx);
    formTrans->addRow("dy:", spinDy);
    formTrans->addRow("dz:", spinDz);

    QPushButton *btnTransladar = new QPushButton("Transladar", grupoTrans);
    formTrans->addRow(btnTransladar);
    layout->addWidget(grupoTrans);

    connect(btnTransladar, &QPushButton::clicked, this, [this]() {
        emit translacaoSolicitada(spinDx->value(), spinDy->value(), spinDz->value());
    });

    // --- Escalar ---
    QGroupBox *grupoEscala = new QGroupBox("Escala (no centroide)", this);
    QFormLayout *formEscala = new QFormLayout(grupoEscala);

    spinSx = new QDoubleSpinBox(grupoEscala);
    spinSy = new QDoubleSpinBox(grupoEscala);
    spinSz = new QDoubleSpinBox(grupoEscala);
    for (QDoubleSpinBox *s : {spinSx, spinSy, spinSz}) {
        s->setRange(0.1, 100.0);
        s->setDecimals(2);
        s->setSingleStep(0.1);
        s->setValue(1.0);
    }
    formEscala->addRow("sx:", spinSx);
    formEscala->addRow("sy:", spinSy);
    formEscala->addRow("sz:", spinSz);

    QPushButton *btnEscalar = new QPushButton("Escalar", grupoEscala);
    formEscala->addRow(btnEscalar);
    layout->addWidget(grupoEscala);

    connect(btnEscalar, &QPushButton::clicked, this, [this]() {
        emit escalaSolicitada(spinSx->value(), spinSy->value(), spinSz->value());
    });

    // --- Rotação ---
    QGroupBox *grupoRot = new QGroupBox("Rotação", this);
    QFormLayout *formRot = new QFormLayout(grupoRot);

    comboEixo = new QComboBox(grupoRot);
    comboEixo->addItem("X (horizontal)");     // índice 0 = Eixo::X
    comboEixo->addItem("Y (vertical)");       // índice 1 = Eixo::Y
    comboEixo->addItem("Z (profundidade)");   // índice 2 = Eixo::Z
    comboEixo->setCurrentIndex(2);
    formRot->addRow("Eixo:", comboEixo);

    spinAngulo = new QDoubleSpinBox(grupoRot);
    spinAngulo->setRange(-360.0, 360.0);
    spinAngulo->setDecimals(1);
    spinAngulo->setValue(0.0);
    spinAngulo->setSuffix("°");
    formRot->addRow("Ângulo:", spinAngulo);

    radioCentroide = new QRadioButton("Em torno do centroide", grupoRot);
    radioPonto = new QRadioButton("Em torno de um ponto", grupoRot);
    radioCentroide->setChecked(true);
    formRot->addRow(radioCentroide);
    formRot->addRow(radioPonto);

    spinPx = new QDoubleSpinBox(grupoRot);
    spinPy = new QDoubleSpinBox(grupoRot);
    spinPz = new QDoubleSpinBox(grupoRot);
    for (QDoubleSpinBox *s : {spinPx, spinPy, spinPz}) {
        s->setRange(-10000.0, 10000.0);
        s->setDecimals(1);
        s->setValue(0.0);
        s->setEnabled(false);
    }
    formRot->addRow("x:", spinPx);
    formRot->addRow("y:", spinPy);
    formRot->addRow("z:", spinPz);

    // Habilita x/y/z apenas quando "ponto" estiver marcado
    connect(radioPonto, &QRadioButton::toggled, this, [this](bool marcado) {
        spinPx->setEnabled(marcado);
        spinPy->setEnabled(marcado);
        spinPz->setEnabled(marcado);
    });

    QPushButton *btnRotacionar = new QPushButton("Rotacionar", grupoRot);
    formRot->addRow(btnRotacionar);
    layout->addWidget(grupoRot);

    connect(btnRotacionar, &QPushButton::clicked, this, [this]() {
        emit rotacaoSolicitada(comboEixo->currentIndex(),
                               spinAngulo->value(),
                               radioCentroide->isChecked(),
                               spinPx->value(), spinPy->value(), spinPz->value());
    });

    // --- Viewport ---
    QGroupBox *grupoVp = new QGroupBox("Viewport (pixels)", this);
    QFormLayout *formVp = new QFormLayout(grupoVp);

    spinVxmin = new QDoubleSpinBox(grupoVp);
    spinVymin = new QDoubleSpinBox(grupoVp);
    spinVxmax = new QDoubleSpinBox(grupoVp);
    spinVymax = new QDoubleSpinBox(grupoVp);
    for (QDoubleSpinBox *s : {spinVxmin, spinVymin, spinVxmax, spinVymax}) {
        s->setRange(0.0, 10000.0);
        s->setDecimals(0);
        s->setSingleStep(10.0);
    }
    formVp->addRow("xmin:", spinVxmin);
    formVp->addRow("ymin:", spinVymin);
    formVp->addRow("xmax:", spinVxmax);
    formVp->addRow("ymax:", spinVymax);
    layout->addWidget(grupoVp);

    for (QDoubleSpinBox *s : {spinVxmin, spinVymin, spinVxmax, spinVymax})
        connect(s, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this]() { emitirViewport(); });

    // --- Composição ---
    QGroupBox *grupoComp = new QGroupBox("Composição de matrizes (4x4)", this);
    QVBoxLayout *vComp = new QVBoxLayout(grupoComp);

    QCheckBox *chkAcumular = new QCheckBox("Acumular (não aplica na hora)", grupoComp);
    vComp->addWidget(chkAcumular);

    QPushButton *btnAplicar = new QPushButton("Aplicar composição", grupoComp);
    QPushButton *btnLimpar = new QPushButton("Limpar", grupoComp);
    vComp->addWidget(btnAplicar);
    vComp->addWidget(btnLimpar);

    labelMatriz = new QLabel("Matriz: identidade", grupoComp);
    labelMatriz->setStyleSheet("font-family: monospace;");
    vComp->addWidget(labelMatriz);
    layout->addWidget(grupoComp);

    connect(chkAcumular, &QCheckBox::toggled, this, &PainelPropriedades::modoAcumularAlterado);
    connect(btnAplicar, &QPushButton::clicked, this, &PainelPropriedades::aplicarComposicaoSolicitada);
    connect(btnLimpar, &QPushButton::clicked, this, &PainelPropriedades::limparComposicaoSolicitada);

    // --- Projeção (informativo) ---
    QLabel *labelProjecao = new QLabel("Projeção: ortogonal (plano XY; a coordenada z é descartada).", this);
    labelProjecao->setWordWrap(true);
    layout->addWidget(labelProjecao);
}

void PainelPropriedades::definirObjetos(const QStringList &nomes)
{
    listaObjetos->clear();
    listaObjetos->addItems(nomes);
}

void PainelPropriedades::selecionarObjeto(int indice)
{
    listaObjetos->setCurrentRow(indice);
}

void PainelPropriedades::mostrarMatriz(const QString &texto)
{
    labelMatriz->setText(texto);
}

void PainelPropriedades::emitirViewport()
{
    emit viewportAlterada(spinVxmin->value(), spinVymin->value(),
                          spinVxmax->value(), spinVymax->value());
}

void PainelPropriedades::definirViewport(double xmin, double ymin, double xmax, double ymax)
{
    const QList<QDoubleSpinBox *> spins = {spinVxmin, spinVymin, spinVxmax, spinVymax};
    for (QDoubleSpinBox *s : spins) s->blockSignals(true);
    spinVxmin->setValue(xmin);
    spinVymin->setValue(ymin);
    spinVxmax->setValue(xmax);
    spinVymax->setValue(ymax);
    for (QDoubleSpinBox *s : spins) s->blockSignals(false);
}