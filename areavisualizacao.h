#ifndef AREAVISUALIZACAO_H
#define AREAVISUALIZACAO_H

#include <QWidget>
#include "displayfile.h"
#include "viewport.h"

class AreaVisualizacao : public QWidget
{
    Q_OBJECT
private:
    DisplayFile *displayFile = nullptr;    // Ponteiro para a base de dados
    Viewport viewport;

public:
    explicit AreaVisualizacao(QWidget *parent = nullptr);

    void setDisplayFile(DisplayFile *df) {
        displayFile = df;
        update();
    }
    void setViewport(const Viewport &v) {
        viewport = v;
        update();
    }
    const Viewport &getViewport() const { return viewport; }

signals:
    void tamanhoAlterado(const QSize &tamanho);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
};

#endif // AREAVISUALIZACAO_H
