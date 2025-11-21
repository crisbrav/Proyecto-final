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
    explicit Guard(QGraphicsItem *parent = nullptr);

    void setBaseSpeed(double speed);
    void setGrid(MazeGrid *grid);

    void perceive(const Player *player);
    void planPath(const QPoint &startCell, const QPoint &targetCell);
    void act(double dt);
    void learn(); // aumenta velocidad según veces que ha visto al jugador

    int timesSeenPlayer() const;

    void update(double dt) override;

private:
    double m_baseSpeed;
    double m_currentSpeed;
    int m_timesSeenPlayer;

    MazeGrid *m_grid;
    QVector<QPoint> m_route; // camino de celdas
    int m_currentRouteIndex;
};



#endif // GUARD_H
