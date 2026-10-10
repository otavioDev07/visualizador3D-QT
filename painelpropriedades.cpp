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

PainelPropriedades::PainelPropriedades(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Lista de objetos ---
    layout->addWidget(new QLabel("Objetos", this));
    listaObjetos = new QListWidget(this);
    layout->addWidget(listaObjetos);

    connect(listaObjetos, &QListWidget::currentRowChanged,
            this, &PainelPropriedades::objetoSelecionado);

    // --- Translação ---
    QGroupBox *grupoTrans = new QGroupBox("Translação", this);
    QFormLayout *formTrans = new QFormLayout(grupoTrans);

    spinDx = new QDoubleSpinBox(grupoTrans);
    spinDy = new QDoubleSpinBox(grupoTrans);
    for (QDoubleSpinBox *s : {spinDx, spinDy}) {
        s->setRange(-10000.0, 10000.0);
        s->setDecimals(1);
        s->setValue(0.0);
    }
    formTrans->addRow("dx:", spinDx);
    formTrans->addRow("dy:", spinDy);

    QPushButton *btnTransladar = new QPushButton("Transladar", grupoTrans);
    formTrans->addRow(btnTransladar);
    layout->addWidget(grupoTrans);

    connect(btnTransladar, &QPushButton::clicked, this, [this]() {
        emit translacaoSolicitada(spinDx->value(), spinDy->value());
    });

    // --- Escalar ---
    QGroupBox *grupoEscala = new QGroupBox("Escala (no centroide)", this);
    QFormLayout *formEscala = new QFormLayout(grupoEscala);

    spinSx = new QDoubleSpinBox(grupoEscala);
    spinSy = new QDoubleSpinBox(grupoEscala);
    for (QDoubleSpinBox *s : {spinSx, spinSy}) {
        s->setRange(0.1, 100.0);
        s->setDecimals(2);
        s->setSingleStep(0.1);
        s->setValue(1.0);
    }
    formEscala->addRow("sx:", spinSx);
    formEscala->addRow("sy:", spinSy);

    QPushButton *btnEscalar = new QPushButton("Escalar", grupoEscala);
    formEscala->addRow(btnEscalar);
    layout->addWidget(grupoEscala);

    connect(btnEscalar, &QPushButton::clicked, this, [this]() {
        emit escalaSolicitada(spinSx->value(), spinSy->value());
    });

    // --- Rotação ---
    QGroupBox *grupoRot = new QGroupBox("Rotação", this);
    QFormLayout *formRot = new QFormLayout(grupoRot);

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
    for (QDoubleSpinBox *s : {spinPx, spinPy}) {
        s->setRange(-10000.0, 10000.0);
        s->setDecimals(1);
        s->setValue(0.0);
        s->setEnabled(false);
    }
    formRot->addRow("x:", spinPx);
    formRot->addRow("y:", spinPy);

    // Habilita x/y apenas quando "ponto" estiver marcado
    connect(radioPonto, &QRadioButton::toggled, this, [this](bool marcado) {
        spinPx->setEnabled(marcado);
        spinPy->setEnabled(marcado);
    });

    QPushButton *btnRotacionar = new QPushButton("Rotacionar", grupoRot);
    formRot->addRow(btnRotacionar);
    layout->addWidget(grupoRot);

    connect(btnRotacionar, &QPushButton::clicked, this, [this]() {
        emit rotacaoSolicitada(spinAngulo->value(),
                               radioCentroide->isChecked(),
                               spinPx->value(), spinPy->value());
    });

    // --- Composição ---
    QGroupBox *grupoComp = new QGroupBox("Composição de matrizes", this);
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


}

void PainelPropriedades::definirObjetos(const QStringList &nomes)
{
    listaObjetos->clear();
    listaObjetos->addItems(nomes);
}

void PainelPropriedades::mostrarMatriz(const QString &texto)
{
    labelMatriz->setText(texto);
}