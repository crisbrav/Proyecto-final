#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "gameobject.h"

class Projectile : public GameObject
{
    Q_OBJECT

public:
    explicit Projectile(QGraphicsItem *parent = nullptr);

    void setAcceleration(const QPointF &a);
    QPointF acceleration() const;

    void update(double dt) override;

private:
    QPointF m_acceleration;
};



#endif // PROJECTILE_H
