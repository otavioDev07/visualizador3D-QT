#include "areavisualizacao.h"
#include <QPainter>
#include <QLineF>
#include <QVector>
#include "clipping.h"
using namespace Qt;

AreaVisualizacao::AreaVisualizacao(QWidget *parent) : QWidget{parent} {}

void AreaVisualizacao::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.fillRect(rect(), white);
    painter.fillRect(viewport.retangulo(), white);

    if (!displayFile) return;

    painter.save();

    // Transformada de viewport: SCN -> pixels (já calculada em Viewport::definir)
    const Matriz &vp = viewport.matriz();

    const auto &objs = displayFile->obterObjetos();

    for (const auto &obj : objs) {

        if (obj.pontosSCN.size() != obj.vertices.size() ||
            obj.visivel.size()   != obj.vertices.size()) continue;

        if (obj.tipo == TipoObjeto::PONTO) {
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(QPen(obj.cor, 6, SolidLine, RoundCap));
            //desenho com clipping de pontos
            for (size_t i = 0; i < obj.pontosSCN.size(); ++i) {
                if (!obj.visivel[i]) continue;
                if (Clipping::pontoDentro(obj.pontosSCN[i]))
                    painter.drawPoint(vp * obj.pontosSCN[i]);
            }
        }
        else {
            const bool modelo = (obj.tipo == TipoObjeto::MODELO);
            painter.setRenderHint(QPainter::Antialiasing, !modelo);
            painter.setPen(QPen(obj.cor, modelo ? 1 : 2));

            QVector<QLineF> linhas;
            linhas.reserve(static_cast<qsizetype>(obj.arestas.size()));

            for (const auto &aresta : obj.arestas) {
                if (!obj.visivel[static_cast<size_t>(aresta.first)] ||
                    !obj.visivel[static_cast<size_t>(aresta.second)]) continue;
                QPointF a = obj.pontosSCN[static_cast<size_t>(aresta.first)];
                QPointF b = obj.pontosSCN[static_cast<size_t>(aresta.second)];

                if (Clipping::recortarReta(a, b)){
                    linhas.append(QLineF(vp * a, vp * b));
                }
            }
            painter.drawLines(linhas);
        }
    }

    painter.restore();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(darkGray, 1));
    painter.setBrush(NoBrush);
    painter.drawRect(viewport.retangulo());
}
