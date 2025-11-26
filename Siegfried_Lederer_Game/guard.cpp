#include "guard.h"
#include "player.h"
#include "mazegrid.h"

#include <QtMath>

Guard::Guard(MazeGrid *grid, QGraphicsItem *parent)
    : GameObject(parent),
    m_grid(grid),
    m_state(Patrol),
    m_baseSpeed(70.0),
    m_currentSpeed(70.0),
    m_visionRange(6 * grid->cellSize()),
    m_timesSeenPlayer(0),
    m_patrolIndex(0),
    m_routeIndex(0)
{
}

void Guard::setBaseSpeed(double speed)
{
    m_baseSpeed = speed;
    m_currentSpeed = speed;
}

void Guard::setVisionRange(double rangePixels)
{
    m_visionRange = rangePixels;
}

void Guard::setPatrolPath(const QVector<QPoint> &cells)
{
    m_patrolPath = cells;
    m_patrolIndex = 0;
}

void Guard::setCurrentCell(const QPoint &cell)
{
    m_currentCell = cell;
    if (m_grid) {
        QPointF center = m_grid->cellCenter(cell);
        // centramos el sprite en la celda
        setPos(center.x() - pixmap().width() / 2.0,
               center.y() - pixmap().height() / 2.0);
    }
}

int Guard::timesSeenPlayer() const
{
    return m_timesSeenPlayer;
}

bool Guard::canSeePlayer(const Player *player) const
{
    if (!player)
        return false;

    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);
    QPointF playerCenter = player->pos() + QPointF(player->pixmap().width() / 2.0,
                                                   player->pixmap().height() / 2.0);

    double dx = playerCenter.x() - guardCenter.x();
    double dy = playerCenter.y() - guardCenter.y();
    double dist = qSqrt(dx * dx + dy * dy);

    return dist <= m_visionRange;
}

void Guard::learn()
{
    ++m_timesSeenPlayer;
    // aumenta la velocidad un 15 % cada vez
    m_currentSpeed = m_baseSpeed * (1.0 + 0.15 * m_timesSeenPlayer);
}

void Guard::moveTowardsCell(const QPoint &cell, double dt)
{
    if (!m_grid)
        return;

    QPointF targetCenter = m_grid->cellCenter(cell);
    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);

    QPointF dir = targetCenter - guardCenter;
    double len = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
    if (len < 1.0) {
        // ya casi en el centro
        m_currentCell = cell;
        return;
    }

    QPointF step = (dir / len) * (m_currentSpeed * dt);
    setPos(pos() + step);
    m_currentCell = m_grid->sceneToCell(guardCenter + step);
}

void Guard::followRoute(double dt)
{
    if (m_route.isEmpty())
        return;
    if (m_routeIndex >= m_route.size())
        return;

    QPoint targetCell = m_route[m_routeIndex];
    moveTowardsCell(targetCell, dt);

    // si está cerca del centro, pasa al siguiente
    QPointF center = m_grid->cellCenter(targetCell);
    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);

    double dx = guardCenter.x() - center.x();
    double dy = guardCenter.y() - center.y();
    double dist = qSqrt(dx * dx + dy * dy);

    if (dist < 4.0) {
        ++m_routeIndex;
        if (m_routeIndex >= m_route.size()) {
            // ruta terminada: si patrullaba, vuelve al inicio
            if (m_state == Patrol && !m_patrolPath.isEmpty()) {
                m_patrolIndex = (m_patrolIndex + 1) % m_patrolPath.size();
                m_route.clear();
                m_routeIndex = 0;
            }
        }
    }
}

void Guard::updateAI(const Player *player, double dt)
{
    if (!m_grid)
        return;

    // si ve al jugador, inicia persecución
    if (canSeePlayer(player)) {
        if (m_state != Chase) {
            m_state = Chase;
            learn();
        }

        QPoint guardCell = m_grid->sceneToCell(
            pos() + QPointF(pixmap().width() / 2.0,
                            pixmap().height() / 2.0));

        QPoint playerCell = m_grid->sceneToCell(
            player->pos() + QPointF(player->pixmap().width() / 2.0,
                                    player->pixmap().height() / 2.0));

        m_route = m_grid->bfs(guardCell, playerCell);
        m_routeIndex = 0;
    } else {
        // si no ve al jugador y estaba persiguiendo, vuelve a patrulla
        if (m_state == Chase && !m_patrolPath.isEmpty()) {
            m_state = Patrol;
            m_route.clear();
            m_routeIndex = 0;
        }
    }

    if (m_state == Chase && !m_route.isEmpty()) {
        followRoute(dt);
    } else if (m_state == Patrol && !m_patrolPath.isEmpty()) {
        // patrulla: si no tiene ruta activa, crea ruta de la celda actual
        if (m_route.isEmpty()) {
            QPoint guardCell = m_grid->sceneToCell(
                pos() + QPointF(pixmap().width() / 2.0,
                                pixmap().height() / 2.0));
            QPoint nextPatrolCell = m_patrolPath[m_patrolIndex];
            m_route = m_grid->bfs(guardCell, nextPatrolCell);
            m_routeIndex = 0;
        }
        followRoute(dt);
    }
}
