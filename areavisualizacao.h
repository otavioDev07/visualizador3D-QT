#ifndef AREAVISUALIZACAO_H
#define AREAVISUALIZACAO_H

#include <QWidget>

class AreaVisualizacao : public QWidget
{
    Q_OBJECT
public:
    explicit AreaVisualizacao(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
};

#endif // AREAVISUALIZACAO_H
