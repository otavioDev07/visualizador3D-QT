#ifndef PAINELPROPRIEDADES_H
#define PAINELPROPRIEDADES_H

#include <QWidget>
#include <QStringList>

class QListWidget;
class QDoubleSpinBox;
class QRadioButton;
class QCheckBox;
class QLabel;

class PainelPropriedades : public QWidget
{
    Q_OBJECT
private:
    QListWidget *listaObjetos = nullptr;
    QDoubleSpinBox *spinDx = nullptr;
    QDoubleSpinBox *spinDy = nullptr;
    QDoubleSpinBox *spinSx = nullptr;
    QDoubleSpinBox *spinSy = nullptr;
    QDoubleSpinBox *spinAngulo = nullptr;
    QDoubleSpinBox *spinPx = nullptr;
    QDoubleSpinBox *spinPy = nullptr;
    QRadioButton *radioCentroide = nullptr;
    QRadioButton *radioPonto = nullptr;
    QLabel *labelMatriz = nullptr;

public:
    explicit PainelPropriedades(QWidget *parent = nullptr);

    void definirObjetos(const QStringList &nomes);
    void mostrarMatriz(const QString &texto);

signals:
    void objetoSelecionado(int indice);
    void translacaoSolicitada(double dx, double dy);
    void escalaSolicitada(double sx, double sy);
    void rotacaoSolicitada(double graus, bool usarCentroide, double px, double py);
    void modoAcumularAlterado(bool ativo);
    void aplicarComposicaoSolicitada();
    void limparComposicaoSolicitada();

};

#endif // PAINELPROPRIEDADES_H
