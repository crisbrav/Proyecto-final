#include "level1.h"

#include "player.h"
#include "guard.h"
#include "mazegrid.h"
#include "movingtrap.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QBrush>
#include <QPen>
#include <Qtimer>

Level1::Level1(QWidget *parent)
    : BaseLevel(parent),
    m_player(0),
    m_grid(0),
    m_exitRect(0),
    m_playerAnimTimer(0.0),
    m_playerFrameDuration(0.10),
    m_playerFrameIndex(0),
    m_playerDir(PDown),
    m_playerMoving(false),
    m_blueKeysCollected(0),
    m_whiteKeysCollected(0),
    m_blueKeysNeeded(2),
    m_whiteKeysNeeded(2),
    m_lives(MAX_LIVES)
{
    setupScene();
}


void Level1::loadPlayerSprites()
{
    m_playerDownFrames.clear();
    m_playerUpFrames.clear();
    m_playerRightFrames.clear();
    m_playerLeftFrames.clear();

    const int framesPerStrip = 6;

    // FRONT (abajo)
    QPixmap frontSheet(":/assets/prisoner_l1_front.png");
    if (!frontSheet.isNull()) {
        int fw = frontSheet.width() / framesPerStrip;
        int fh = frontSheet.height();
        for (int i = 0; i < framesPerStrip; ++i) {
            QPixmap f = frontSheet.copy(i * fw, 0, fw, fh)
            .scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_playerDownFrames.append(f);
        }
    }

    // BACK (arriba)
    QPixmap backSheet(":/assets/prisoner_l1_back.png");
    if (!backSheet.isNull()) {
        int fw = backSheet.width() / framesPerStrip;
        int fh = backSheet.height();
        for (int i = 0; i < framesPerStrip; ++i) {
            QPixmap f = backSheet.copy(i * fw, 0, fw, fh)
            .scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_playerUpFrames.append(f);
        }
    }

    // SIDES (derecha) y espejado para izquierda
    QPixmap sideSheet(":/assets/prisoner_l1_sides.png");
    if (!sideSheet.isNull()) {
        int fw = sideSheet.width() / framesPerStrip;
        int fh = sideSheet.height();
        for (int i = 0; i < framesPerStrip; ++i) {
            QPixmap right = sideSheet.copy(i * fw, 0, fw, fh)
            .scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_playerRightFrames.append(right);

            QPixmap left = right.transformed(QTransform().scale(-1.0, 1.0));
            m_playerLeftFrames.append(left);
        }
    }
}

void Level1::setupScene()
{
    int cellSize = 32;
    m_grid = new MazeGrid(cellSize, this);

    int width  = m_grid->cols() * cellSize;
    int height = m_grid->rows() * cellSize;
    m_scene->setSceneRect(0, 0, width, height);

    setupMazeGraphics();
    loadPlayerSprites();

    // Crear jugador
    m_player = new Player();

    // frame inicial (abajo)
    QPixmap pm;
    if (!m_playerDownFrames.isEmpty())
        pm = m_playerDownFrames[0];
    else {
        pm = QPixmap(24, 24);
        pm.fill(Qt::blue);
    }
    m_player->setPixmap(pm);

    QPoint startCell = m_grid->playerStartCell();
    QPointF startCenter = m_grid->cellCenter(startCell);

    m_player->setPos(startCenter.x() - pm.width() / 2.0,
                     startCenter.y() - pm.height() / 2.0);
    m_player->setZValue(10);
    m_scene->addItem(m_player);
    m_player->setSpeed(160.0);
    // Crear guardias
    QVector<QPoint> guardCells = m_grid->guardStartCells();

    // Guardias extra lejos del inicio
    QPoint extra1(5, 13);   // fila 13, columna 5 (pasillo amplio al fondo)
    if (m_grid->isWalkable(extra1))
        guardCells.append(extra1);

    QPoint extra2(15, 14);   // otro pasillo más al centro
    if (m_grid->isWalkable(extra2))
        guardCells.append(extra2);


    QPixmap guardSheet(":/assets/guard_l1.png");

    for (int i = 0; i < guardCells.size(); ++i) {
        Guard *g = new Guard(m_grid);
        g->setBaseSpeed(60.0);
        g->setVisionRange(6 * cellSize);

        if (!guardSheet.isNull())
            g->loadSpriteSheet(guardSheet);
        else {
            QPixmap gpm(48, 48);
            gpm.fill(Qt::red);
            g->setPixmap(gpm);
        }

        g->setCurrentCell(guardCells[i]);

        // ruta de patrulla: celda origen + una vecina caminable
        QVector<QPoint> patrol;
        patrol.append(guardCells[i]);

        QPoint base = guardCells[i];
        QPoint candidates[4] = {
            QPoint(base.x() + 2, base.y()),
            QPoint(base.x() - 2, base.y()),
            QPoint(base.x(),     base.y() + 2),
            QPoint(base.x(),     base.y() - 2)
        };

        for (int k = 0; k < 4; ++k) {
            if (m_grid->isWalkable(candidates[k])) {
                patrol.append(candidates[k]);
                break;
            }
        }

        g->setPatrolPath(patrol);

        g->setZValue(10);
        m_scene->addItem(g);
        m_guards.append(g);
    }

    setupKeys();
    setupHearts();
    setupTraps();

    m_view->setSceneRect(m_scene->sceneRect());
    m_view->setFixedSize(width, height);
}

void Level1::setupMazeGraphics()
{
    int cs = m_grid->cellSize();

    QPixmap wallTex(":/assets/muro_l1.png");
    QPixmap floorTex(":/assets/suelo_l1.png");
    QPixmap exitTex(":/assets/salida_l1.png");

    QPixmap wallTile = wallTex.isNull()
                           ? QPixmap(cs, cs)
                           : wallTex.scaled(cs, cs, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if (wallTex.isNull()) wallTile.fill(Qt::darkGray);

    QPixmap floorTile = floorTex.isNull()
                            ? QPixmap(cs, cs)
                            : floorTex.scaled(cs, cs, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if (floorTex.isNull()) floorTile.fill(QColor(30, 30, 30, 60));

    QPixmap exitTile = exitTex.isNull()
                           ? QPixmap(cs, cs)
                           : exitTex.scaled(cs, cs, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if (exitTex.isNull()) exitTile.fill(Qt::green);

    for (int r = 0; r < m_grid->rows(); ++r) {
        for (int c = 0; c < m_grid->cols(); ++c) {
            int v = m_grid->cellAt(r, c);
            int x = c * cs;
            int y = r * cs;

            if (v == MazeGrid::CellWall) {
                QGraphicsPixmapItem *wall =
                    m_scene->addPixmap(wallTile);
                wall->setPos(x, y);
                wall->setZValue(0);
            } else if (v == MazeGrid::CellExit) {
                QGraphicsPixmapItem *ex =
                    m_scene->addPixmap(exitTile);
                ex->setPos(x, y);
                ex->setZValue(1);
            } else {
                QGraphicsPixmapItem *floor =
                    m_scene->addPixmap(floorTile);
                floor->setPos(x, y);
                floor->setZValue(-1);
            }
        }
    }
}

void Level1::setupKeys()
{
    m_keys.clear();
    m_blueKeysCollected = 0;
    m_whiteKeysCollected = 0;

    int cs = m_grid->cellSize();

    // --- preparar pixmaps ---
    QPixmap blueSheet(":/assets/key-blue.png");
    QPixmap whiteSheet(":/assets/key-white.png");

    const int cols = 12; // la hoja de llaves tiene 12 columnas
    QPixmap blueKey, whiteKey;

    if (!blueSheet.isNull()) {
        int fw = blueSheet.width() / cols;
        int fh = blueSheet.height();
        blueKey = blueSheet.copy(0, 0, fw, fh)
                      .scaled(cs - 8, cs - 8,
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation);
    } else {
        blueKey = QPixmap(cs - 8, cs - 8);
        blueKey.fill(Qt::blue);
    }

    if (!whiteSheet.isNull()) {
        int fw = whiteSheet.width() / cols;
        int fh = whiteSheet.height();
        whiteKey = whiteSheet.copy(0, 0, fw, fh)
                       .scaled(cs - 8, cs - 8,
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
    } else {
        whiteKey = QPixmap(cs - 8, cs - 8);
        whiteKey.fill(Qt::white);
    }

    auto addKeyAt = [this, cs, &blueKey, &whiteKey](const QPoint &cell, bool isBlue)
    {
        if (!m_grid->isWalkable(cell))
            return;

        QPixmap pm = isBlue ? blueKey : whiteKey;

        QGraphicsPixmapItem *item = m_scene->addPixmap(pm);
        QPointF center = m_grid->cellCenter(cell);
        item->setPos(center.x() - pm.width() / 2.0,
                     center.y() - pm.height() / 2.0);
        item->setZValue(15);

        KeyData kd;
        kd.item = item;
        kd.isBlue = isBlue;
        kd.collected = false;
        m_keys.append(kd);
    };

    // --- posiciones elegidas dentro del laberinto (celdas caminables) ---
    // azul: (2,5) y (17,9)
    addKeyAt(QPoint(2, 5), true);
    addKeyAt(QPoint(17, 9), true);

    // blanca: (14,3) y (5,13)
    addKeyAt(QPoint(14, 3), false);
    addKeyAt(QPoint(5, 13), false);
}

void Level1::resetKeys()
{
    m_blueKeysCollected = 0;
    m_whiteKeysCollected = 0;

    for (int i = 0; i < m_keys.size(); ++i) {
        m_keys[i].collected = false;
        if (m_keys[i].item)
            m_keys[i].item->setVisible(true);
    }
}

void Level1::setupHearts()
{
    // borrar corazones anteriores si los hubiera (por si recargas escena)
    for (int i = 0; i < m_heartIcons.size(); ++i) {
        QGraphicsPixmapItem *it = m_heartIcons[i];
        if (it) {
            m_scene->removeItem(it);
            delete it;
        }
    }
    m_heartIcons.clear();

    QPixmap heart(":/assets/corazon.png");
    if (heart.isNull()) {
        heart = QPixmap(24, 24);
        heart.fill(Qt::red);
    }
    m_heartPixmap = heart.scaled(24, 24,
                                 Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation);

    int spacing = 4;
    int totalWidth = MAX_LIVES * m_heartPixmap.width()
                     + (MAX_LIVES - 1) * spacing;

    int xStart = static_cast<int>(m_scene->width()) - totalWidth - 10;
    int y = 5;

    for (int i = 0; i < MAX_LIVES; ++i) {
        QGraphicsPixmapItem *icon = m_scene->addPixmap(m_heartPixmap);
        icon->setZValue(200);
        icon->setPos(xStart + i * (m_heartPixmap.width() + spacing), y);
        m_heartIcons.append(icon);
    }

    m_lives = MAX_LIVES;
    updateHeartsHUD();
}

void Level1::updateHeartsHUD()
{
    for (int i = 0; i < m_heartIcons.size(); ++i) {
        if (m_heartIcons[i])
            m_heartIcons[i]->setVisible(i < m_lives);
    }
}



void Level1::resetLevelState()
{
    m_keysPressed.clear();
    m_playerAnimTimer = 0.0;
    m_playerFrameIndex = 0;
    m_playerDir = PDown;
    m_playerMoving = false;

    if (m_player && m_grid) {
        QPoint startCell = m_grid->playerStartCell();
        QPointF startCenter = m_grid->cellCenter(startCell);
        QPixmap pm = m_playerDownFrames.isEmpty()
                         ? m_player->pixmap()
                         : m_playerDownFrames[0];
        m_player->setPixmap(pm);
        m_player->setPos(startCenter.x() - pm.width() / 2.0,
                         startCenter.y() - pm.height() / 2.0);
    }

    QVector<QPoint> guardCells = m_grid->guardStartCells();
    for (int i = 0; i < m_guards.size() && i < guardCells.size(); ++i) {
        Guard *g = m_guards[i];
        if (!g) continue;
        g->setBaseSpeed(60.0);
        g->resetAI();
        g->setCurrentCell(guardCells[i]);
    }

    resetKeys();
    m_lives = MAX_LIVES;
    updateHeartsHUD();

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

    m_playerMoving = up || down || left || right;

    if (down)  m_playerDir = PDown;
    if (up)    m_playerDir = PUp;
    if (left)  m_playerDir = PLeft;
    if (right) m_playerDir = PRight;

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

    QPointF center = newPos + QPointF(m_player->pixmap().width() / 2.0,
                                      m_player->pixmap().height() / 2.0);
    QPoint cell = m_grid->sceneToCell(center);
    if (m_grid->isWalkable(cell)) {
        m_player->setPos(newPos);
    }
}

void Level1::updatePlayerAnimation(double dt)
{
    QVector<QPixmap> *frames = 0;
    switch (m_playerDir) {
    case PDown:  frames = &m_playerDownFrames;  break;
    case PUp:    frames = &m_playerUpFrames;    break;
    case PLeft:  frames = &m_playerLeftFrames;  break;
    case PRight: frames = &m_playerRightFrames; break;
    }

    if (!frames || frames->isEmpty())
        return;

    if (!m_playerMoving) {
        m_playerFrameIndex = 0;
        m_player->setPixmap(frames->at(0));
        return;
    }

    m_playerAnimTimer += dt;
    while (m_playerAnimTimer >= m_playerFrameDuration) {
        m_playerAnimTimer -= m_playerFrameDuration;
        m_playerFrameIndex = (m_playerFrameIndex + 1) % frames->size();
        m_player->setPixmap(frames->at(m_playerFrameIndex));
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
            // perder una vida
            --m_lives;
            if (m_lives < 0) m_lives = 0;
            updateHeartsHUD();

            if (m_lives <= 0) {
                stopLevel();
                emit levelFailed();
                return;
            } else {
                // reposicionar jugador al inicio
                QPoint startCell = m_grid->playerStartCell();
                QPointF startCenter = m_grid->cellCenter(startCell);
                QPixmap pm = m_player->pixmap();
                m_player->setPos(startCenter.x() - pm.width() / 2.0,
                                 startCenter.y() - pm.height() / 2.0);
                // salimos para no procesar más colisiones este frame
                return;
            }
        }
    }

    // recoger llaves
    for (int i = 0; i < m_keys.size(); ++i) {
        KeyData &k = m_keys[i];
        if (!k.item || k.collected)
            continue;

        if (k.item->collidesWithItem(m_player)) {
            k.collected = true;
            k.item->setVisible(false);

            if (k.isBlue)
                ++m_blueKeysCollected;
            else
                ++m_whiteKeysCollected;
        }
    }


    // colisión con trampas oscilatorias
    for (int i = 0; i < m_traps.size(); ++i) {
        MovingTrap *t = m_traps[i];
        if (!t) continue;

        if (t->collidesWithItem(m_player)) {
            stopLevel();
            emit levelFailed();
            return;
        }
    }

    // salida
    QPointF center = m_player->pos()
                     + QPointF(m_player->pixmap().width() / 2.0,
                               m_player->pixmap().height() / 2.0);
    QPoint cell = m_grid->sceneToCell(center);

    bool hasAllKeys = (m_blueKeysCollected >= m_blueKeysNeeded &&
                       m_whiteKeysCollected >= m_whiteKeysNeeded);

    if (cell == m_grid->exitCell() && hasAllKeys) {
        stopLevel();
        emit levelCompleted();
    }

}

void Level1::setupTraps()
{
    if (!m_grid)
        return;

    int cs = m_grid->cellSize();

    QPixmap trapTex(":/assets/trampa_sinusoidal.png");
    QPixmap trapPixmap;
    if (!trapTex.isNull()) {
        trapPixmap = trapTex.scaled(24, 24,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);
    } else {
        trapPixmap = QPixmap(24, 24);
        trapPixmap.fill(Qt::magenta);
    }

    // Elegimos una celda de pasillo para la trampa
    // Por ejemplo: columna 9, fila 3 (x=9,y=3) en nuestro laberinto
    QPoint trapCell(9, 3);
    if (!m_grid->isWalkable(trapCell))
        return; // por si cambiaste el mapa

    MovingTrap *trap = new MovingTrap();
    trap->setPixmap(trapPixmap);

    QPointF cellCenter = m_grid->cellCenter(trapCell);
    QPointF basePos(cellCenter.x() - trapPixmap.width() / 2.0,
                    cellCenter.y() - trapPixmap.height() / 2.0);

    trap->setCenter(basePos);
    trap->setAmplitude(cs * 1.0); // se mueve ~1.5 celdas
    trap->setOmega(3.0);          // +/− frecuencia

    trap->setZValue(5);
    m_scene->addItem(trap);
    m_traps.append(trap);
}


void Level1::updateGame()
{
    double dt = m_fixedDt;

    updatePlayerMovement(dt);
    updatePlayerAnimation(dt);
    updateGuards(dt);

    // Actualizar movimiento sinusoidal de las trampas
    for (int i = 0; i < m_traps.size(); ++i) {
        MovingTrap *t = m_traps[i];
        if (t) t->update(dt);
    }

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
