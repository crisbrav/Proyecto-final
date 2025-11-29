#include "laneobstacle.h"

LaneObstacle::LaneObstacle(double speed, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    m_speed(speed)
{
}

void LaneObstacle::setSpeed(double speed)
{
    m_speed = speed;
}

double LaneObstacle::speed() const
{
    return m_speed;
}

void LaneObstacle::update(double dt)
{
    // Se desplaza hacia la izquierda
    setX(x() - m_speed * dt);
}
