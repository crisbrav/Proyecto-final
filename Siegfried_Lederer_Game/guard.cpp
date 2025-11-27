#include "guard.h"
#include "player.h"
#include "mazegrid.h"

#include <QtMath>

Guard::Guard(MazeGrid *grid, QGraphicsItem *parent)
    : GameObject(parent),
    m_grid(grid),
    m_state(Patrol),
    m_dir(DirDown),
    m_baseSpeed(70.0),
    m_currentSpeed(70.0),
    m_visionRange(6 * grid->cellSize()),
    m_timesSeenPlayer(0),
    m_patrolIndex(0),
    m_routeIndex(0),
    m_animTimer(0.0),
    m_frameDuration(0.10),
    m_animIndex(0),
    m_isMoving(false)
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
        setPos(center.x() - pixmap().width() / 2.0,
               center.y() - pixmap().height() / 2.0);
    }
}

int Guard::timesSeenPlayer() const
{
    return m_timesSeenPlayer;
}

void Guard::loadSpriteSheet(const QPixmap &sheet)
{
    m_framesDown.clear();
    m_framesRight.clear();
    m_framesUp.clear();
    m_framesLeft.clear();

    if (sheet.isNull())
        return;

    const int columns = 8;
    const int rows    = 5;  // dato real de tu hoja
    int frameWidth  = sheet.width()  / columns;
    int frameHeight = sheet.height() / rows;

    // columnas de interés (0-based):
    // 0 abajo, 2 derecha, 4 arriba, 6 izquierda
    int colDown  = 0;
    int colRight = 2;
    int colUp    = 4;
    int colLeft  = 6;

    for (int row = 0; row < rows; ++row) {
        QRect rDown (colDown  * frameWidth, row * frameHeight, frameWidth, frameHeight);
        QRect rRight(colRight * frameWidth, row * frameHeight, frameWidth, frameHeight);
        QRect rUp   (colUp    * frameWidth, row * frameHeight, frameWidth, frameHeight);
        QRect rLeft (colLeft  * frameWidth, row * frameHeight, frameWidth, frameHeight);

        QPixmap fDown  = sheet.copy(rDown ).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fRight = sheet.copy(rRight).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fUp    = sheet.copy(rUp   ).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fLeft  = sheet.copy(rLeft ).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_framesDown .append(fDown);
        m_framesRight.append(fRight);
        m_framesUp   .append(fUp);
        m_framesLeft .append(fLeft);
    }

    if (!m_framesDown.isEmpty()) {
        m_dir = DirDown;
        m_animIndex = 0;
        setPixmap(m_framesDown[0]);
    }
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
    // aumenta velocidad un 15 % cada vez
    m_currentSpeed = m_baseSpeed * (1.0 + 0.15 * m_timesSeenPlayer);
}

void Guard::moveTowardsCell(const QPoint &cell, double dt)
{
    if (!m_grid)
        return;

    QPointF targetCenter = m_grid->cellCenter(cell);
    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);

    QPointF dirVec = targetCenter - guardCenter;
    double len = qSqrt(dirVec.x() * dirVec.x() + dirVec.y() * dirVec.y());

    if (len < 1.0) {
        // ya casi en el centro
        m_currentCell = cell;
        m_isMoving = false;
        return;
    }

    // decidir dirección principal para la animación
    if (qFabs(dirVec.x()) > qFabs(dirVec.y())) {
        m_dir = (dirVec.x() > 0) ? DirRight : DirLeft;
    } else {
        m_dir = (dirVec.y() > 0) ? DirDown : DirUp;
    }

    QPointF step = (dirVec / len) * (m_currentSpeed * dt);
    setPos(pos() + step);
    m_currentCell = m_grid->sceneToCell(guardCenter + step);
    m_isMoving = true;
}

void Guard::followRoute(double dt)
{
    if (m_route.isEmpty())
        return;
    if (m_routeIndex >= m_route.size())
        return;

    QPoint targetCell = m_route[m_routeIndex];
    moveTowardsCell(targetCell, dt);

    QPointF center = m_grid->cellCenter(targetCell);
    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);

    double dx = guardCenter.x() - center.x();
    double dy = guardCenter.y() - center.y();
    double dist = qSqrt(dx * dx + dy * dy);

    if (dist < 4.0) {
        ++m_routeIndex;
        if (m_routeIndex >= m_route.size()) {
            if (m_state == Patrol && !m_patrolPath.isEmpty()) {
                m_patrolIndex = (m_patrolIndex + 1) % m_patrolPath.size();
                m_route.clear();
                m_routeIndex = 0;
            }
        }
    }
}

const QVector<QPixmap> &Guard::currentFrames() const
{
    switch (m_dir) {
    case DirDown:  return m_framesDown;
    case DirRight: return m_framesRight;
    case DirUp:    return m_framesUp;
    case DirLeft:  return m_framesLeft;
    }
    return m_framesDown;
}

void Guard::updateAnimation(double dt)
{
    const QVector<QPixmap> &frames = currentFrames();
    if (frames.isEmpty())
        return;

    if (!m_isMoving) {
        // quieto → primer frame
        m_animIndex = 0;
        setPixmap(frames[0]);
        return;
    }

    m_animTimer += dt;
    while (m_animTimer >= m_frameDuration) {
        m_animTimer -= m_frameDuration;
        m_animIndex = (m_animIndex + 1) % frames.size();
        setPixmap(frames[m_animIndex]);
    }
}

void Guard::updateAI(const Player *player, double dt)
{
    if (!m_grid)
        return;

    // ¿ve al jugador?
    if (canSeePlayer(player)) {
        if (m_state != Chase) {
            m_state = Chase;
            learn(); // aumenta la velocidad
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
        // Si estaba persiguiendo y lo pierde de vista, vuelve a patrulla
        if (m_state == Chase) {
            m_state = Patrol;
            m_route.clear();
            m_routeIndex = 0;
        }

        // Si está en patrulla y no tiene ruta, crea una ruta hacia
        // el siguiente punto de patrulla
        if (m_state == Patrol && !m_patrolPath.isEmpty() && m_route.isEmpty()) {
            QPoint guardCell = m_grid->sceneToCell(
                pos() + QPointF(pixmap().width() / 2.0,
                                pixmap().height() / 2.0));
            QPoint targetCell = m_patrolPath[m_patrolIndex];
            m_route = m_grid->bfs(guardCell, targetCell);
            m_routeIndex = 0;
        }
    }

    if (!m_route.isEmpty()) {
        followRoute(dt);
    } else {
        m_isMoving = false;
    }

    updateAnimation(dt);
}
