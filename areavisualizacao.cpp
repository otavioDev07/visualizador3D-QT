#include "areavisualizacao.h"
#include <QPainter>

AreaVisualizacao::AreaVisualizacao(QWidget *parent) : QWidget{parent} {}
void AreaVisualizacao::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
}