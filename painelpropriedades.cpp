#include "painelpropriedades.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QSignalBlocker>
#include <QFontDatabase>
#include <QHBoxLayout>
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

    // --- Composição ---
    QGroupBox *grupoComp = new QGroupBox("Composição de matrizes (4x4)", this);
    QVBoxLayout *vComp = new QVBoxLayout(grupoComp);

    labelMatriz = new QLabel(grupoComp);
    labelMatriz->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    vComp->addWidget(labelMatriz);

    QHBoxLayout *botoes = new QHBoxLayout;
    QPushButton *btnAplicar = new QPushButton("Aplicar", grupoComp);
    QPushButton *btnLimpar = new QPushButton("Limpar", grupoComp);
    botoes->addWidget(btnAplicar);
    botoes->addWidget(btnLimpar);
    vComp->addLayout(botoes);
    layout->addWidget(grupoComp);

    connect(btnAplicar, &QPushButton::clicked, this, &PainelPropriedades::aplicarComposicaoSolicitada);
    connect(btnLimpar, &QPushButton::clicked, this, &PainelPropriedades::limparComposicaoSolicitada);

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

    layout->addWidget(grupoTrans);

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

    layout->addWidget(grupoEscala);

    // --- Rotação ---
    QGroupBox *grupoRot = new QGroupBox("Rotação", this);
    QFormLayout *formRot = new QFormLayout(grupoRot);

    spinAnguloX = new QDoubleSpinBox(grupoRot);
    spinAnguloY = new QDoubleSpinBox(grupoRot);
    spinAnguloZ = new QDoubleSpinBox(grupoRot);
    for (QDoubleSpinBox *s : {spinAnguloX, spinAnguloY, spinAnguloZ}) {
        s->setRange(-360.0, 360.0);
        s->setDecimals(1);
        s->setSuffix("°");
    }
    formRot->addRow("Ângulo X:", spinAnguloX);
    formRot->addRow("Ângulo Y:", spinAnguloY);
    formRot->addRow("Ângulo Z:", spinAnguloZ);

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

    connect(radioPonto, &QRadioButton::toggled,
            this, &PainelPropriedades::atualizarCamposPivo);

    layout->addWidget(grupoRot);

    for (QDoubleSpinBox *s : {spinDx, spinDy, spinDz, spinSx, spinSy, spinSz,
                              spinAnguloX, spinAnguloY, spinAnguloZ, spinPx, spinPy, spinPz}) {
        connect(s, &QDoubleSpinBox::valueChanged, this, &PainelPropriedades::transformacaoAlterada);
    }
    connect(radioPonto, &QRadioButton::toggled, this, &PainelPropriedades::transformacaoAlterada);

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
        s->setKeyboardTracking(false);
    }
    formVp->addRow("xmin:", spinVxmin);
    formVp->addRow("ymin:", spinVymin);
    formVp->addRow("xmax:", spinVxmax);
    formVp->addRow("ymax:", spinVymax);
    avisoViewport = new QLabel(tr("Os limites máximos devem ser maiores que os mínimos. "
                                   "O desenho mantém a última viewport válida."), grupoVp);
    avisoViewport->setWordWrap(true);
    avisoViewport->hide();
    formVp->addRow(avisoViewport);
    layout->addWidget(grupoVp);

    for (QDoubleSpinBox *s : {spinVxmin, spinVymin, spinVxmax, spinVymax})
        connect(s, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this]() { emitirViewport(); });

    // Projeção
    QGroupBox *grupoProj = new QGroupBox("Projeção", this);
    QFormLayout *formProj = new QFormLayout(grupoProj);

    comboProjecao = new QComboBox(grupoProj);
    comboProjecao->addItem("Paralela (ortogonal)");
    comboProjecao->addItem("Perspectiva");
    formProj->addRow("Tipo:", comboProjecao);

    spinDistancia = new QDoubleSpinBox(grupoProj);
    spinDistancia->setRange(50.0, 5000.0);
    spinDistancia->setDecimals(0);
    spinDistancia->setSingleStep(50.0);
    spinDistancia->setValue(800.0);
    spinDistancia->setEnabled(false);
    formProj->addRow("Distância d:", spinDistancia);

    layout->addWidget(grupoProj);

    connect(comboProjecao, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int indice) {
                spinDistancia->setEnabled(indice == 1);
                emit projecaoAlterada(indice);
            });

    connect(spinDistancia, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &PainelPropriedades::distanciaAlterada);
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

void PainelPropriedades::definirModoCamera(bool ativo)
{
    modoCamera = ativo;
    spinDz->setEnabled(!ativo);
    spinSz->setEnabled(!ativo);
    radioCentroide->setText(ativo ? tr("Em torno do centro da câmera")
                                  : tr("Em torno do centroide"));
    radioPonto->setText(ativo ? tr("Pivô XY para a rotação em Z")
                              : tr("Em torno de um ponto"));
    atualizarCamposPivo();
}

void PainelPropriedades::atualizarCamposPivo()
{
    const bool usarPonto = radioPonto->isChecked();
    spinPx->setEnabled(usarPonto);
    spinPy->setEnabled(usarPonto);
    spinPz->setEnabled(usarPonto && !modoCamera);
}

Transformacao PainelPropriedades::transformacao() const
{
    return {
        {spinDx->value(), spinDy->value(), spinDz->value()},
        {spinSx->value(), spinSy->value(), spinSz->value()},
        {spinAnguloX->value(), spinAnguloY->value(), spinAnguloZ->value()},
        {spinPx->value(), spinPy->value(), spinPz->value()},
        radioCentroide->isChecked()
    };
}

void PainelPropriedades::limparTransformacao()
{
    for (QDoubleSpinBox *s : {spinDx, spinDy, spinDz, spinAnguloX, spinAnguloY, spinAnguloZ}) {
        const QSignalBlocker blocker(s);
        s->setValue(0.0);
    }
    for (QDoubleSpinBox *s : {spinSx, spinSy, spinSz}) {
        const QSignalBlocker blocker(s);
        s->setValue(1.0);
    }
    emit transformacaoAlterada();
}

void PainelPropriedades::emitirViewport()
{
    const double xmin = spinVxmin->value(), ymin = spinVymin->value();
    const double xmax = spinVxmax->value(), ymax = spinVymax->value();
    const bool valida = xmax > xmin && ymax > ymin;
    avisoViewport->setVisible(!valida);
    if (valida)
        emit viewportAlterada(xmin, ymin, xmax, ymax);
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
    avisoViewport->hide();
}
