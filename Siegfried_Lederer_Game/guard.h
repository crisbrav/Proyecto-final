#ifndef GUARD_H
#define GUARD_H

#include "gameobject.h"
#include <QVector>
#include <QPoint>

class Player;
class MazeGrid;

class Guard : public GameObject
{
    Q_OBJECT

public:
    explicit Guard(MazeGrid *grid, QGraphicsItem *parent = 0);

    void setBaseSpeed(double speed);
    void setVisionRange(double rangePixels);
    void setPatrolPath(const QVector<QPoint> &cells);
    void setCurrentCell(const QPoint &cell);

    int timesSeenPlayer() const;

    // IA: se llama desde el nivel en cada frame
    void updateAI(const Player *player, double dt);

private:
    enum State {
        Patrol,
        Chase
    };

    MazeGrid *m_grid;
    State m_state;

    double m_baseSpeed;
    double m_currentSpeed;
    double m_visionRange;
    int m_timesSeenPlayer;

    QVector<QPoint> m_patrolPath;
    int m_patrolIndex;

    QVector<QPoint> m_route;
    int m_routeIndex;

    QPoint m_currentCell;

    bool canSeePlayer(const Player *player) const;
    void followRoute(double dt);
    void moveTowardsCell(const QPoint &cell, double dt);
    void learn();
};

#endif // GUARD_H
