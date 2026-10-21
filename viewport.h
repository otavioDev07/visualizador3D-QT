#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QRectF>
#include "matriz.h"

// Viewport em pixels, no sistema do widget (origem no canto superior esquerdo, y para baixo).
class Viewport {
private:
    double xmin_ = 20.0, ymin_ = 20.0;
    double xmax_ = 700.0, ymax_ = 700.0;

    Matriz vp_;                 // cache da transformada de viewport

    void recalcular() {
        // SCN [-1,1] -> viewport:
        //   xv = (xn + 1)/2 * (xmax - xmin) + xmin
        //   yv = (1 - (yn + 1)/2) * (ymax - ymin) + ymin    <- inversão do eixo y
        const double w = xmax_ - xmin_;
        const double h = ymax_ - ymin_;
        vp_ = Matriz();
        vp_(0, 0) =  w / 2.0;  vp_(0, 2) = xmin_ + w / 2.0;
        vp_(1, 1) = -h / 2.0;  vp_(1, 2) = ymin_ + h / 2.0;
    }

public:
    Viewport() { recalcular(); }

    void definir(double xmin, double ymin, double xmax, double ymax) {
        xmin_ = xmin; ymin_ = ymin; xmax_ = xmax; ymax_ = ymax;
        recalcular();           // a cada mudança, a transformada é refeita
    }

    QRectF retangulo() const { return QRectF(xmin_, ymin_, xmax_ - xmin_, ymax_ - ymin_); }
    double xmin() const { return xmin_; }
    double ymin() const { return ymin_; }
    double xmax() const { return xmax_; }
    double ymax() const { return ymax_; }

    const Matriz &matriz() const { return vp_; }
};

#endif // VIEWPORT_H
