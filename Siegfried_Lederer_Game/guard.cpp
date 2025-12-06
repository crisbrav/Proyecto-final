#include "guard.h"
#include "player.h"
#include "mazegrid.h"

#include <QtMath>

static const double MAX_GUARD_SPEED = 120.0; // px/s máx


Guard::Guard(MazeGrid *grid, QGraphicsItem *parent)
    : GameObject(parent),
    m_grid(grid),
    m_state(Patrol),
    m_dir(DirDown),
    m_baseSpeed(60.0),
    m_currentSpeed(60.0),
    m_visionRange(3 * grid->cellSize()),  // antes 6*cellSize
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

    // 8 columnas x 5 filas
    const int columns = 8;
    const int rows    = 5;
    int frameWidth  = sheet.width()  / columns;
    int frameHeight = sheet.height() / rows;

    // columnas (0-based):
    // 0: abajo
    // 2: izquierda  (col 3 de la hoja)
    // 4: arriba
    // 6: derecha    (col 7 de la hoja)
    int colDown   = 0;
    int colLeft   = 2;
    int colUp     = 4;
    int colRight  = 6;

    for (int row = 0; row < rows; ++row) {
        QRect rDown (colDown  * frameWidth, row * frameHeight, frameWidth, frameHeight);
        QRect rLeft (colLeft  * frameWidth, row * frameHeight, frameWidth, frameHeight);
        QRect rUp   (colUp    * frameWidth, row * frameHeight, frameWidth, frameHeight);
        QRect rRight(colRight * frameWidth, row * frameHeight, frameWidth, frameHeight);

        QPixmap fDown  = sheet.copy(rDown ).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fLeft  = sheet.copy(rLeft ).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fUp    = sheet.copy(rUp   ).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap fRight = sheet.copy(rRight).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_framesDown .append(fDown);
        m_framesLeft .append(fLeft);
        m_framesUp   .append(fUp);
        m_framesRight.append(fRight);
    }

    if (!m_framesDown.isEmpty()) {
        m_dir = DirDown;
        m_animIndex = 0;
        setPixmap(m_framesDown[0]);
    }
}




bool Guard::canSeePlayer(const Player *player) const
{
    if (!player || !m_grid)
        return false;

    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);
    QPointF playerCenter = player->pos() +
                           QPointF(player->pixmap().width() / 2.0,
                                   player->pixmap().height() / 2.0);

    double dx = playerCenter.x() - guardCenter.x();
    double dy = playerCenter.y() - guardCenter.y();
    double dist = qSqrt(dx * dx + dy * dy);

    if (dist > m_visionRange)
        return false;

    // comprobamos línea de visión en la malla de celdas
    QPoint guardCell  = m_grid->sceneToCell(guardCenter);
    QPoint playerCell = m_grid->sceneToCell(playerCenter);

    if (guardCell.x() < 0 || playerCell.x() < 0)
        return false;

    if (!hasLineOfSight(guardCell, playerCell))
        return false;

    return true;
}


bool Guard::hasLineOfSight(const QPoint &from, const QPoint &to) const
{
    if (!m_grid)
        return false;

    int x0 = from.x();
    int y0 = from.y();
    int x1 = to.x();
    int y1 = to.y();

    int dx = qAbs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -qAbs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    int x = x0;
    int y = y0;

    while (true) {
        // ignoramos celda de origen y destino, miramos las intermedias
        if (!(x == x0 && y == y0) && !(x == x1 && y == y1)) {
            QPoint cell(x, y);
            if (!m_grid->isWalkable(cell)) {
                // si no es caminable, lo consideramos muro que bloquea visión
                return false;
            }
        }

        if (x == x1 && y == y1)
            break;

        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }

    return true;
}



void Guard::learn()
{
    ++m_timesSeenPlayer;
    // aumenta velocidad base un 15 % cada vez, pero limitada
    m_currentSpeed = m_baseSpeed * (1.0 + 0.15 * m_timesSeenPlayer);
    if (m_currentSpeed > MAX_GUARD_SPEED)
        m_currentSpeed = MAX_GUARD_SPEED;
}

void Guard::resetAI()
{
    m_state = Patrol;
    m_currentSpeed = m_baseSpeed;
    m_timesSeenPlayer = 0;
    m_patrolIndex = 0;
    m_route.clear();
    m_routeIndex = 0;
    m_isMoving = false;
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
        // ya estamos prácticamente en el centro de esa celda
        m_currentCell = cell;
        m_isMoving = false;
        return;
    }

    // decidir dirección para animación
    if (qFabs(dirVec.x()) > qFabs(dirVec.y())) {
        m_dir = (dirVec.x() > 0) ? DirRight : DirLeft;
    } else {
        m_dir = (dirVec.y() > 0) ? DirDown : DirUp;
    }

    QPointF step = (dirVec / len) * (m_currentSpeed * dt);
    QPointF newCenter = guardCenter + step;

    // actualizamos posición a partir del nuevo centro
    setPos(newCenter.x() - pixmap().width()  / 2.0,
           newCenter.y() - pixmap().height() / 2.0);

    m_currentCell = m_grid->sceneToCell(newCenter);
    m_isMoving = true;
}

void Guard::followRoute(double dt)
{
    if (m_route.isEmpty() || m_routeIndex >= m_route.size())
        return;

    QPoint targetCell = m_route[m_routeIndex];
    moveTowardsCell(targetCell, dt);

    // cuando estamos lo bastante cerca del centro de la celda objetivo,
    // pasamos a la siguiente en la ruta
    QPointF targetCenter = m_grid->cellCenter(targetCell);
    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);

    double dx = guardCenter.x() - targetCenter.x();
    double dy = guardCenter.y() - targetCenter.y();
    double dist = qSqrt(dx * dx + dy * dy);

    if (dist < 4.0) {
        m_currentCell = targetCell;
        ++m_routeIndex;
        if (m_routeIndex >= m_route.size()) {
            // hemos llegado al final de la ruta
            m_route.clear();
            m_routeIndex = 0;
            m_isMoving = false;
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

    // celda actual
    QPointF guardCenter = pos() + QPointF(pixmap().width() / 2.0,
                                          pixmap().height() / 2.0);
    QPoint guardCell = m_grid->sceneToCell(guardCenter);
    m_currentCell = guardCell;

    bool seesPlayer = canSeePlayer(player);

    if (seesPlayer && player) {
        // ====== PERSECUCIÓN: BFS hasta la celda del jugador ======
        QPointF playerCenter = player->pos() +
                               QPointF(player->pixmap().width() / 2.0,
                                       player->pixmap().height() / 2.0);
        QPoint playerCell = m_grid->sceneToCell(playerCenter);

        QVector<QPoint> newRoute = m_grid->bfs(guardCell, playerCell);

        if (newRoute.size() >= 2) {
            if (m_state != Chase) {
                m_state = Chase;
                learn();  // aumenta velocidad, pero limitada
            }
            m_route = newRoute;
            m_routeIndex = 1; // índice 0 es la celda actual
        } else {
            // ve al jugador pero no hay ruta: vuelve a patrulla
            if (m_state == Chase) {
                m_state = Patrol;
                m_currentSpeed = m_baseSpeed;
                m_route.clear();
                m_routeIndex = 0;
            }
        }
    } else {
        // ====== PATRULLA ======
        if (m_state == Chase) {
            m_state = Patrol;
            m_currentSpeed = m_baseSpeed;
            m_route.clear();
            m_routeIndex = 0;
        }

        if (m_state == Patrol && !m_patrolPath.isEmpty()) {
            if (m_route.isEmpty()) {
                QPoint targetCell = m_patrolPath[m_patrolIndex];
                QVector<QPoint> newRoute = m_grid->bfs(guardCell, targetCell);

                if (newRoute.size() >= 2) {
                    m_route = newRoute;
                    m_routeIndex = 1;
                } else {
                    // si la ruta es vacía o de longitud 1 (ya estoy allí),
                    // pasamos al siguiente punto de patrulla
                    m_patrolIndex = (m_patrolIndex + 1) % m_patrolPath.size();
                }
            }
        }
    }

    // ====== MOVER A LO LARGO DE LA RUTA (Patrol o Chase) ======
    if (!m_route.isEmpty() && m_routeIndex < m_route.size()) {
        followRoute(dt);
    } else {
        m_isMoving = false;
    }

    updateAnimation(dt);
}
