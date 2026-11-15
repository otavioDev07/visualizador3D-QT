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

        if (obj.pontosSCN.size() != obj.vertices.size()) continue;

        if (obj.tipo == TipoObjeto::PONTO) {
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(QPen(obj.cor, 6, SolidLine, RoundCap));
            //desenho com clipping de pontos
            for (const auto& pontoSCN : obj.pontosSCN){
                if (Clipping::pontoDentro(pontoSCN)){
                    painter.drawPoint(vp * pontoSCN);
                }
            }
        }
        else {
            const bool modelo = (obj.tipo == TipoObjeto::MODELO);
            painter.setRenderHint(QPainter::Antialiasing, !modelo);
            painter.setPen(QPen(obj.cor, modelo ? 1 : 2));

            QVector<QLineF> linhas;
            linhas.reserve(static_cast<qsizetype>(obj.arestas.size()));

            for (const auto &aresta : obj.arestas) {
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
