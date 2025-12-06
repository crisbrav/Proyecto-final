#ifndef BOMB_H
#define BOMB_H

#include "gameobject.h"

class Bomb : public GameObject
{
    Q_OBJECT

public:
    explicit Bomb(QGraphicsItem *parent = nullptr);

    void setGravity(double g);
    double gravity() const;

    void update(double dt) override;

private:
    double m_gravity;        // aceleración vertical (px/s^2)
    double m_verticalSpeed;  // velocidad vertical actual (px/s)
};

#endif // BOMB_H
