#include "areavisualizacao.h"
#include <QPainter>

AreaVisualizacao::AreaVisualizacao(QWidget *parent) : QWidget{parent} {}

void AreaVisualizacao::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    if (!displayFile) return;

    for (const auto& obj : displayFile->obterObjetos()) {
        if (obj.tipo == TipoObjeto::PONTO) {
            QPen pen(Qt::red, 6, Qt::SolidLine, Qt::RoundCap);
            painter.setPen(pen);
            for (const auto& pt : obj.pontos) {
                painter.drawPoint(pt);
            }
        }
        else if (obj.tipo == TipoObjeto::RETA) {
            QPen pen(Qt::blue, 2);
            painter.setPen(pen);
            if (obj.pontos.size() >= 2) {
                for (size_t i = 0; i < obj.pontos.size() - 1; i += 2) {
                    painter.drawLine(obj.pontos[i], obj.pontos[i+1]);
                }
            }
        }
        else if (obj.tipo == TipoObjeto::POLIGONO) {
            QPen pen(Qt::black, 2);
            painter.setPen(pen);
            painter.setBrush(Qt::yellow);

            QPolygonF poli;
            for (const auto& pt : obj.pontos) {
                poli << pt;
            }
            painter.drawPolygon(poli);
        }
    }
}