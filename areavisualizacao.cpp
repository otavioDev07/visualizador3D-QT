#include "areavisualizacao.h"
#include <QPainter>
#include "clipping.h"
using namespace Qt;

AreaVisualizacao::AreaVisualizacao(QWidget *parent) : QWidget{parent} {}

void AreaVisualizacao::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), white);
    painter.fillRect(viewport.retangulo(), white);

    if (!displayFile) return;

    painter.save();

    // Transformada de viewport: SCN -> pixels (já calculada em Viewport::definir)
    const Matriz &vp = viewport.matriz();

    const auto &objs = displayFile->obterObjetos();

    for (const auto &obj : objs) {

        if (obj.tipo == TipoObjeto::PONTO) {
            painter.setPen(QPen(red, 6, SolidLine, RoundCap));
            //desenho com clipping de pontos
            for (const auto& pontoSCN : obj.pontosSCN){
                if (Clipping::pontoDentro(pontoSCN)){
                    painter.drawPoint(vp * pontoSCN);
                }
            }
        }
        else if (obj.tipo == TipoObjeto::RETA) {
            painter.setPen(QPen(blue, 2));
            //desenho com clipping de reta
            for(size_t i = 0; i + 1 < obj.pontosSCN.size(); i += 2){
                QPointF a = obj.pontosSCN[i];
                QPointF b = obj.pontosSCN[i+1];

                if (Clipping::recortarReta(a, b)){
                    painter.drawLine(vp * a, vp *b);
                }
            }
        }
    }

    painter.restore();
    painter.setPen(QPen(darkGray, 1));
    painter.setBrush(NoBrush);
    painter.drawRect(viewport.retangulo());
}
