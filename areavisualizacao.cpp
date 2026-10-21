#include "areavisualizacao.h"
#include <QPainter>

AreaVisualizacao::AreaVisualizacao(QWidget *parent) : QWidget{parent} {}

void AreaVisualizacao::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fundo do widget e a viewport propriamente dita
    painter.fillRect(rect(), QColor(210, 210, 210));
    painter.fillRect(viewport.retangulo(), Qt::white);
    painter.setPen(QPen(Qt::darkGray, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(viewport.retangulo());

    if (!displayFile) return;

    // Ainda não há clipping geométrico (entrega seguinte): por ora o Qt apenas
    // impede que o desenho vaze para fora da viewport.
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
            painter.setPen(QPen(Qt::red, 6, Qt::SolidLine, Qt::RoundCap));
            for (const auto &pt : pts)
                painter.drawPoint(pt);
        }
        else if (obj.tipo == TipoObjeto::RETA) {
            painter.setPen(QPen(Qt::blue, 2));
            for (size_t i = 0; i + 1 < pts.size(); i += 2)
                painter.drawLine(pts[i], pts[i + 1]);
        }
        else if (obj.tipo == TipoObjeto::POLIGONO) {
            painter.setPen(QPen(Qt::black, 2));
            painter.setBrush(Qt::yellow);
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
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::darkGreen, 1, Qt::DashLine));
            painter.drawRect(poly.boundingRect().adjusted(-8, -8, 8, 8));
        }
    }
}
