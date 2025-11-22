// guard.cpp
#include "guard.h"
#include "player.h"
//#include "mazegrid.h"

Guard::Guard(QGraphicsItem *parent)
    : GameObject(parent),
    m_baseSpeed(120.0),
    m_currentSpeed(120.0),
    m_timesSeenPlayer(0),
    m_grid(nullptr),
    m_currentRouteIndex(0)
{
}

void Guard::setBaseSpeed(double speed)
{
    m_baseSpeed = speed;
    m_currentSpeed = speed;
}

void Guard::setGrid(MazeGrid *grid)
{
    m_grid = grid;
}

void Guard::perceive(const Player *player)
{
    Q_UNUSED(player);

}

void Guard::planPath(const QPoint &startCell, const QPoint &targetCell)
{
    Q_UNUSED(startCell);
    Q_UNUSED(targetCell);
    m_route.clear();
    m_currentRouteIndex = 0;
}

void Guard::act(double dt)
{
    Q_UNUSED(dt);

}

void Guard::learn()
{
    ++m_timesSeenPlayer;
    // Ejemplo: incrementar velocidad un 10% cada vez
    m_currentSpeed = m_baseSpeed * (1.0 + 0.1 * m_timesSeenPlayer);
}

int Guard::timesSeenPlayer() const
{
    return m_timesSeenPlayer;
}

void Guard::update(double dt)
{
    Q_UNUSED(dt);

}
