#ifndef PAINELPROPRIEDADES_H
#define PAINELPROPRIEDADES_H

#include <QWidget>
#include <QStringList>
#include "matriz.h"

class QListWidget;
class QDoubleSpinBox;
class QRadioButton;
class QComboBox;
class QLabel;

struct Transformacao {
    Ponto3D deslocamento;
    Ponto3D escala;
    Ponto3D angulos;
    Ponto3D pivo;
    bool usarCentroide;
};

class PainelPropriedades : public QWidget
{
    Q_OBJECT
private:
    QListWidget *listaObjetos = nullptr;
    QDoubleSpinBox *spinDx = nullptr;
    QDoubleSpinBox *spinDy = nullptr;
    QDoubleSpinBox *spinDz = nullptr;
    QDoubleSpinBox *spinSx = nullptr;
    QDoubleSpinBox *spinSy = nullptr;
    QDoubleSpinBox *spinSz = nullptr;
    QDoubleSpinBox *spinAnguloX = nullptr;
    QDoubleSpinBox *spinAnguloY = nullptr;
    QDoubleSpinBox *spinAnguloZ = nullptr;
    QDoubleSpinBox *spinPx = nullptr;
    QDoubleSpinBox *spinPy = nullptr;
    QDoubleSpinBox *spinPz = nullptr;
    QRadioButton *radioCentroide = nullptr;
    QRadioButton *radioPonto = nullptr;
    QLabel *labelMatriz = nullptr;
    QLabel *avisoViewport = nullptr;
    bool modoCamera = false;

    // Viewport
    QDoubleSpinBox *spinVxmin = nullptr;
    QDoubleSpinBox *spinVymin = nullptr;
    QDoubleSpinBox *spinVxmax = nullptr;
    QDoubleSpinBox *spinVymax = nullptr;

    QComboBox *comboProjecao = nullptr;
    QDoubleSpinBox *spinDistancia = nullptr;

    void emitirViewport();
    void atualizarCamposPivo();

public:
    explicit PainelPropriedades(QWidget *parent = nullptr);

    void definirObjetos(const QStringList &nomes);
    void selecionarObjeto(const QString &nome);
    void mostrarMatriz(const QString &texto);
    void definirModoCamera(bool ativo);
    void limparTransformacao();
    Transformacao transformacao() const;

    void definirViewport(double xmin, double ymin, double xmax, double ymax);

signals:
    void objetoSelecionado(const QString &nome);
    void transformacaoAlterada();
    void aplicarComposicaoSolicitada();
    void limparComposicaoSolicitada();
    void viewportAlterada(double xmin, double ymin, double xmax, double ymax);
    void projecaoAlterada(int tipo);
    void distanciaAlterada(double d);

};

#endif // PAINELPROPRIEDADES_H
