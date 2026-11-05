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
    painter.setClipRect(viewport.retangulo());

    // Transformada de viewport: SCN -> pixels (já calculada em Viewport::definir)
    const Matriz &vp = viewport.matriz();

    // Converte as coordenadas SCN de um objeto para pixels
    auto paraTela = [&vp](const std::vector<QPointF> &scn) {
        std::vector<QPointF> tela;
        tela.reserve(scn.size());
        for (const auto &p : scn)
            tela.push_back(vp * p);
        return tela;
    };

    const auto &objs = displayFile->obterObjetos();

    for (const auto &obj : objs) {
        const std::vector<QPointF> pts = paraTela(obj.pontosSCN);

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
        else if (obj.tipo == TipoObjeto::POLIGONO) {
            painter.setPen(QPen(black, 2));
            painter.setBrush(yellow);
            QPolygonF poli;
            for (const auto &pt : pts)
                poli << pt;
            painter.drawPolygon(poli);
        }
    }

    // Retângulo tracejado em volta do objeto selecionado
    if (indiceSelecionado >= 0 && indiceSelecionado < static_cast<int>(objs.size())) {
        QPolygonF poly;
        for (const auto &pt : paraTela(objs[indiceSelecionado].pontosSCN))
            poly << pt;
        if (!poly.isEmpty()) {
            painter.setBrush(NoBrush);
            painter.setPen(QPen(darkGreen, 1, DashLine));
            painter.drawRect(poly.boundingRect().adjusted(-8, -8, 8, 8));
        }
    }

    painter.restore();
    painter.setPen(QPen(darkGray, 1));
    painter.setBrush(NoBrush);
    painter.drawRect(viewport.retangulo());
}
