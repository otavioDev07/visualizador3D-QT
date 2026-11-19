#ifndef PAINELPROPRIEDADES_H
#define PAINELPROPRIEDADES_H

#include <QWidget>
#include <QStringList>

class QListWidget;
class QDoubleSpinBox;
class QRadioButton;
class QCheckBox;
class QComboBox;
class QLabel;

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
    QComboBox *comboEixo = nullptr;
    QDoubleSpinBox *spinAngulo = nullptr;
    QDoubleSpinBox *spinPx = nullptr;
    QDoubleSpinBox *spinPy = nullptr;
    QDoubleSpinBox *spinPz = nullptr;
    QRadioButton *radioCentroide = nullptr;
    QRadioButton *radioPonto = nullptr;
    QLabel *labelMatriz = nullptr;

    // Viewport
    QDoubleSpinBox *spinVxmin = nullptr;
    QDoubleSpinBox *spinVymin = nullptr;
    QDoubleSpinBox *spinVxmax = nullptr;
    QDoubleSpinBox *spinVymax = nullptr;

    QComboBox *comboProjecao = nullptr;
    QDoubleSpinBox *spinDistancia = nullptr;

    void emitirViewport();

public:
    explicit PainelPropriedades(QWidget *parent = nullptr);

    void definirObjetos(const QStringList &nomes);
    void selecionarObjeto(int indice);
    void mostrarMatriz(const QString &texto);

    void definirViewport(double xmin, double ymin, double xmax, double ymax);

signals:
    void objetoSelecionado(int indice);
    void translacaoSolicitada(double dx, double dy, double dz);
    void escalaSolicitada(double sx, double sy, double sz);
    void rotacaoSolicitada(int eixo, double graus, bool usarCentroide, double px, double py, double pz);
    void modoAcumularAlterado(bool ativo);
    void aplicarComposicaoSolicitada();
    void limparComposicaoSolicitada();
    void viewportAlterada(double xmin, double ymin, double xmax, double ymax);
    void projecaoAlterada(int tipo);
    void distanciaAlterada(double d);

};

#endif // PAINELPROPRIEDADES_H
