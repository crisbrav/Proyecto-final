#include "laneobstacle.h"

LaneObstacle::LaneObstacle(double speed, int laneIndex, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    m_speed(speed),
    m_laneIndex(laneIndex)
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

void LaneObstacle::setLaneIndex(int laneIndex)
{
    m_laneIndex = laneIndex;
}

int LaneObstacle::laneIndex() const
{
    return m_laneIndex;
}

void LaneObstacle::update(double dt)
{
    setX(x() - m_speed * dt);
}
