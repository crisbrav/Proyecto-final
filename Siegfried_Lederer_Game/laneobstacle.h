#ifndef LANEOBSTACLE_H
#define LANEOBSTACLE_H

#include <QGraphicsPixmapItem>

class LaneObstacle : public QGraphicsPixmapItem
{
public:
    explicit LaneObstacle(double speed = 0.0,
                          int laneIndex = 0,
                          QGraphicsItem *parent = 0);

    void setSpeed(double speed);
    double speed() const;

    void setLaneIndex(int laneIndex);
    int laneIndex() const;

    // actualiza la posición en X (se mueve hacia la izquierda)
    void update(double dt);

private:
    double m_speed;   // píxeles/segundo hacia la izquierda
    int    m_laneIndex;
};

#endif // LANEOBSTACLE_H
