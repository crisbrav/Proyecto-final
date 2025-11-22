#ifndef MOVINGTRAP_H
#define MOVINGTRAP_H


#include "gameobject.h"

class MovingTrap : public GameObject
{
    Q_OBJECT

public:
    explicit MovingTrap(QGraphicsItem *parent = nullptr);

    void setCenter(const QPointF &center);
    void setAmplitude(double amplitude);
    void setOmega(double omega); // frecuencia angular

    void update(double dt) override;

private:
    QPointF m_center;
    double m_amplitude;
    double m_omega;
    double m_t; // tiempo acumulado
};



#endif // MOVINGTRAP_H
