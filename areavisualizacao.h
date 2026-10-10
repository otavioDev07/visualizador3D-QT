#ifndef AREAVISUALIZACAO_H
#define AREAVISUALIZACAO_H

#include <QWidget>
#include "displayfile.h"

class AreaVisualizacao : public QWidget
{
    Q_OBJECT
private:
    DisplayFile *displayFile = nullptr;    // Ponteiro para a base de dados
    int indiceSelecionado = -1;

public:
    explicit AreaVisualizacao(QWidget *parent = nullptr);

    void setDisplayFile(DisplayFile *df) {
        displayFile = df;
        update();
    }
    void setIndiceSelecionado(int i) {
        indiceSelecionado = i;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // AREAVISUALIZACAO_H