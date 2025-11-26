#include "level1.h"

#include "player.h"
#include "guard.h"
#include "mazegrid.h"

#include <Qtimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QBrush>
#include <QPen>

Level1::Level1(QWidget *parent)
    : BaseLevel(parent),
    m_player(0),
    m_grid(0),
    m_exitRect(0)
{
    setupScene();
}

void Level1::setupScene()
{
    int cellSize = 32;
    m_grid = new MazeGrid(cellSize, this);

    // tamaño de la escena según el laberinto
    int width  = m_grid->cols() * cellSize;
    int height = m_grid->rows() * cellSize;
    m_scene->setSceneRect(0, 0, width, height);

    setupMazeGraphics();

    // Crear jugador
    m_player = new Player();
    // TODO: reemplazar por sprite top-down
    QPixmap pm(24, 24);
    pm.fill(Qt::blue);
    m_player->setPixmap(pm);

    QPoint startCell = m_grid->playerStartCell();
    QPointF startCenter = m_grid->cellCenter(startCell);

    m_player->setPos(startCenter.x() - pm.width() / 2.0,
                     startCenter.y() - pm.height() / 2.0);
    m_player->setZValue(10);
    m_scene->addItem(m_player);

    // Crear guardias
    QVector<QPoint> guardCells = m_grid->guardStartCells();
    for (int i = 0; i < guardCells.size(); ++i) {
        Guard *g = new Guard(m_grid);
        // TODO: sprite propio para guardia
        QPixmap gpm(24, 24);
        gpm.fill(Qt::red);
        g->setPixmap(gpm);
        g->setBaseSpeed(60.0);
        g->setVisionRange(6 * cellSize);

        g->setCurrentCell(guardCells[i]);

        // ruta de patrulla simple: celda actual + una vecina si es caminable
        QVector<QPoint> patrol;
        patrol.append(guardCells[i]);
        QPoint right(guardCells[i].x() + 2, guardCells[i].y());
        if (m_grid->isWalkable(right))
            patrol.append(right);
        g->setPatrolPath(patrol);

        g->setZValue(10);
        m_scene->addItem(g);
        m_guards.append(g);
    }

    // Ajustar vista
    m_view->setSceneRect(m_scene->sceneRect());
    m_view->setFixedSize(width, height);
}

void Level1::setupMazeGraphics()
{
    int cs = m_grid->cellSize();

    for (int r = 0; r < m_grid->rows(); ++r) {
        for (int c = 0; c < m_grid->cols(); ++c) {
            int v = m_grid->cellAt(r, c);
            int x = c * cs;
            int y = r * cs;

            if (v == MazeGrid::CellWall) {
                QGraphicsRectItem *wall =
                    m_scene->addRect(x, y, cs, cs,
                                     QPen(Qt::NoPen),
                                     QBrush(Qt::darkGray));
                wall->setZValue(0);
            } else if (v == MazeGrid::CellExit) {
                m_exitRect = m_scene->addRect(x, y, cs, cs,
                                              QPen(Qt::yellow),
                                              QBrush(Qt::green));
                m_exitRect->setZValue(1);
            } else {
                // piso opcional
                QGraphicsRectItem *floor =
                    m_scene->addRect(x, y, cs, cs,
                                     QPen(Qt::NoPen),
                                     QBrush(QColor(30, 30, 30, 60)));
                floor->setZValue(-1);
            }
        }
    }
}

void Level1::resetLevelState()
{
    m_keysPressed.clear();

    // reposicionar jugador
    if (m_player && m_grid) {
        QPoint startCell = m_grid->playerStartCell();
        QPointF startCenter = m_grid->cellCenter(startCell);
        QPixmap pm = m_player->pixmap();
        m_player->setPos(startCenter.x() - pm.width() / 2.0,
                         startCenter.y() - pm.height() / 2.0);
    }

    // reiniciar guardias
    QVector<QPoint> guardCells = m_grid->guardStartCells();
    for (int i = 0; i < m_guards.size() && i < guardCells.size(); ++i) {
        Guard *g = m_guards[i];
        if (!g) continue;
        g->setBaseSpeed(60.0);
        g->setCurrentCell(guardCells[i]);
    }
}

void Level1::startLevel()
{
    resetLevelState();
    m_mainTimer->start(16);
}

void Level1::updatePlayerMovement(double dt)
{
    if (!m_player)
        return;

    bool up    = m_keysPressed.contains(Qt::Key_W) || m_keysPressed.contains(Qt::Key_Up);
    bool down  = m_keysPressed.contains(Qt::Key_S) || m_keysPressed.contains(Qt::Key_Down);
    bool left  = m_keysPressed.contains(Qt::Key_A) || m_keysPressed.contains(Qt::Key_Left);
    bool right = m_keysPressed.contains(Qt::Key_D) || m_keysPressed.contains(Qt::Key_Right);

    QPointF oldPos = m_player->pos();
    QPointF newPos = oldPos;

    double speed = m_player->speed(); // px/s

    if (up)
        newPos.setY(newPos.y() - speed * dt);
    if (down)
        newPos.setY(newPos.y() + speed * dt);
    if (left)
        newPos.setX(newPos.x() - speed * dt);
    if (right)
        newPos.setX(newPos.x() + speed * dt);

    // comprobamos colisión contra paredes de forma simple:
    // tomamos el centro del jugador y vemos si la celda es caminable
    QPointF center = newPos + QPointF(m_player->pixmap().width() / 2.0,
                                      m_player->pixmap().height() / 2.0);
    QPoint cell = m_grid->sceneToCell(center);
    if (m_grid->isWalkable(cell)) {
        m_player->setPos(newPos);
    }
}

void Level1::updateGuards(double dt)
{
    for (int i = 0; i < m_guards.size(); ++i) {
        Guard *g = m_guards[i];
        if (!g) continue;
        g->updateAI(m_player, dt);
    }
}

void Level1::checkCollisions()
{
    if (!m_player || !m_grid)
        return;

    // colisión con guardias
    for (int i = 0; i < m_guards.size(); ++i) {
        Guard *g = m_guards[i];
        if (!g) continue;

        if (g->collidesWithItem(m_player)) {
            // te atraparon
            stopLevel();
            emit levelFailed();
            return;
        }
    }

    // ¿llegó a la salida? (comprobamos celda)
    QPointF center = m_player->pos()
                     + QPointF(m_player->pixmap().width() / 2.0,
                               m_player->pixmap().height() / 2.0);
    QPoint cell = m_grid->sceneToCell(center);

    if (cell == m_grid->exitCell()) {
        stopLevel();
        emit levelCompleted();
    }
}

void Level1::updateGame()
{
    double dt = m_fixedDt;

    updatePlayerMovement(dt);
    updateGuards(dt);
    checkCollisions();
}

void Level1::keyPressEvent(QKeyEvent *event)
{
    m_keysPressed.insert(event->key());
    BaseLevel::keyPressEvent(event);
}

void Level1::keyReleaseEvent(QKeyEvent *event)
{
    m_keysPressed.remove(event->key());
    BaseLevel::keyReleaseEvent(event);
}
