#ifndef MOVINGTRAP_H
#define MOVINGTRAP_H

#include "gameobject.h"
#include <QPointF>

class MovingTrap : public GameObject
{
    Q_OBJECT

public:
    explicit MovingTrap(QGraphicsItem *parent = 0);

    void setCenter(const QPointF &center);
    void setAmplitude(double amplitude); // en píxeles
    void setOmega(double omega);         // frecuencia angular (rad/s)

    void update(double dt) override;

private:
    QPointF m_center;   // posición base (esquina superior izquierda)
    double m_amplitude; // cuánto se mueve a cada lado
    double m_omega;     // velocidad angular
    double m_t;         // tiempo acumulado
};

#endif // MOVINGTRAP_H
