#include "level2.h"

#include "player.h"
#include "laneobstacle.h"
#include "tankprojectile.h"
#include "explosion.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>
#include <QtMath>
#include <QTransform>

// ------------------- Implementación Level2 ------------------- //

Level2::Level2(QWidget *parent)
    : BaseLevel(parent),
    m_player(0),
    m_currentLaneIndex(1),
    m_fixedPlayerX(0.0),
    m_roadTopY(371.0),
    m_roadBottomY(559.0),
    m_obstacleTimer(new QTimer(this)),
    m_projectileTimer(new QTimer(this)),
    m_tankItem(0),
    m_tankMuzzleX(0.0),
    m_elapsedTime(0.0),
    m_totalTime(40.0),   // tiempo de supervivencia (ajústalo si quieres)
    m_lives(3),
    m_maxLives(10),
    m_timeText(0),
    m_timeBarBg(0),
    m_timeBarFill(0),
    m_livesText(0)
{
    setupScene();
    setupHud();
    loadExplosionFrames();

    connect(m_obstacleTimer, &QTimer::timeout,
            this, &Level2::spawnObstacle);

    connect(m_projectileTimer, &QTimer::timeout,
            this, &Level2::spawnProjectile);
}

void Level2::setupScene()
{
    // --- Fondo ---
    QPixmap bgPixmap(":/assets/Fondo_l2.png");

    int width = 800;
    int height = 600;

    if (!bgPixmap.isNull()) {
        width  = bgPixmap.width();
        height = bgPixmap.height();
        m_scene->setSceneRect(0, 0, width, height);

        QGraphicsPixmapItem *bg = m_scene->addPixmap(bgPixmap);
        bg->setPos(0, 0);
        bg->setZValue(-100);
    } else {
        m_scene->setSceneRect(0, 0, width, height);
        QGraphicsRectItem *rect =
            m_scene->addRect(0, 0, width, height,
                             QPen(Qt::NoPen),
                             QBrush(Qt::darkGreen));
        rect->setZValue(-100);
    }

    // --- Carriles usando tus valores ---
    // Carretera: y = 371 .. 559
    // Carril superior: 371 .. 424  → base ≈ 424
    // Carril medio:   434 .. 486  → base ≈ 486
    // Carril inferior:508 .. 559  → base ≈ 559
    m_laneBaseY.clear();
    m_laneBaseY.append(424.0);
    m_laneBaseY.append(486.0);
    m_laneBaseY.append(559.0);

    // --- Jugador ---
    m_player = new Player();

    QPixmap sheet(":/assets/prisionero_l3.png");
    QVector<QPixmap> frames;
    if (!sheet.isNull()) {
        const int columns = 10;
        const int frameWidth  = sheet.width() / columns;
        const int frameHeight = sheet.height();

        for (int col = 0; col < columns; ++col) {
            QPixmap frame = sheet.copy(col * frameWidth,
                                       0,
                                       frameWidth,
                                       frameHeight);
            QPixmap scaled = frame.scaled(128, 128,
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation);
            frames.append(scaled);
        }
        m_player->setAnimationFrames(frames);
    } else {
        QPixmap pm(40, 40);
        pm.fill(Qt::yellow);
        m_player->setPixmap(pm);
    }

    m_player->setZValue(0);

    // X fija (un poco hacia la izquierda)
    m_fixedPlayerX = width * 0.25;

    QPixmap pm = m_player->pixmap();
    m_currentLaneIndex = 1; // carril medio
    double baseY = m_laneBaseY[m_currentLaneIndex];
    double initialY = baseY - pm.height();
    m_player->setPos(m_fixedPlayerX, initialY);

    m_scene->addItem(m_player);

    // El personaje SIEMPRE corriendo (animación activa todo el tiempo)
    m_player->startRunning(1);

    // --- Tanque entre y = 300 y y = 371 ---
    QPixmap tankSheet(":/assets/tanque_l2.png");
    if (!tankSheet.isNull()) {
        const int columns = 4;
        const int rows    = 3;
        int fw = tankSheet.width()  / columns;
        int fh = tankSheet.height() / rows;

        // primer frame
        QPixmap tankFrame = tankSheet.copy(0, 0, fw, fh)
                                .scaled(190, 110,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);

        m_tankItem = m_scene->addPixmap(tankFrame);
        m_tankItem->setZValue(-10);

        // colocamos el tanque pegado al borde superior de la carretera:
        // bottom = m_roadTopY → y = roadTopY - tankHeight
        double tankY = m_roadTopY - m_tankItem->pixmap().height();
        // por si acaso, limitar a no subir de y=300
        if (tankY < 300.0)
            tankY = 300.0;

        m_tankItem->setPos(10, tankY);

        // boca del cañón en el borde derecho del tanque
        m_tankMuzzleX = m_tankItem->x() + m_tankItem->pixmap().width() - 10;
    }

    m_view->setSceneRect(m_scene->sceneRect());
    m_view->setFixedSize(width, height);
}

void Level2::setupHud()
{
    const int margin    = 10;
    const int barWidth  = 220;
    const int barHeight = 14;

    // barra de tiempo
    m_timeBarBg = m_scene->addRect(margin, margin,
                                   barWidth, barHeight,
                                   QPen(Qt::black),
                                   QBrush(Qt::darkGray));
    m_timeBarBg->setZValue(190);

    m_timeBarFill = m_scene->addRect(margin, margin,
                                     barWidth, barHeight,
                                     QPen(Qt::NoPen),
                                     QBrush(QColor(255, 140, 0)));
    m_timeBarFill->setZValue(191);

    m_timeText = m_scene->addText("");
    m_timeText->setDefaultTextColor(Qt::white);
    m_timeText->setZValue(192);
    m_timeText->setPos(margin, margin + barHeight + 4);

    // texto vidas
    m_livesText = m_scene->addText("");
    m_livesText->setDefaultTextColor(Qt::white);
    m_livesText->setZValue(192);

    setupHearts();

    updateTimeHUD();
    updateHeartsHUD();
}

void Level2::setupHearts()
{
    for (QGraphicsPixmapItem *icon : m_heartIcons) {
        if (icon) {
            m_scene->removeItem(icon);
            delete icon;
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
    int totalWidth = m_maxLives * m_heartPixmap.width()
                     + (m_maxLives - 1) * spacing;

    QRectF rect = m_scene->sceneRect();
    // mismo truco que en nivel 3 (corrimiento horizontal)
    int xStart = static_cast<int>(rect.right()) - totalWidth - 150;
    int y = 5;

    for (int i = 0; i < m_maxLives; ++i) {
        QGraphicsPixmapItem *icon = m_scene->addPixmap(m_heartPixmap);
        icon->setZValue(200);
        icon->setPos(xStart + i * (m_heartPixmap.width() + spacing), y);
        m_heartIcons.append(icon);
    }

    if (m_livesText) {
        m_livesText->setPos(xStart, y + m_heartPixmap.height() + 2);
    }
}

void Level2::loadExplosionFrames()
{
    QPixmap sheet(":/assets/explosion_l3.png");
    if (sheet.isNull())
        return;

    m_explosionFrames.clear();

    int frameHeight = sheet.height();
    int columns     = sheet.width() / frameHeight;
    int frameWidth  = frameHeight;

    for (int col = 0; col < columns; ++col) {
        QPixmap frame = sheet.copy(col * frameWidth,
                                   0,
                                   frameWidth,
                                   frameHeight);
        QPixmap scaled = frame.scaled(128, 128,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation);
        m_explosionFrames.append(scaled);
    }
}

void Level2::updateTimeHUD()
{
    if (!m_timeBarBg || !m_timeBarFill || !m_timeText)
        return;

    double remaining = m_totalTime - m_elapsedTime;
    if (remaining < 0.0) remaining = 0.0;

    double ratio = (m_totalTime > 0.0) ? (remaining / m_totalTime) : 0.0;
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;

    QRectF bgRect = m_timeBarBg->rect();
    double newWidth = bgRect.width() * ratio;

    m_timeBarFill->setRect(bgRect.x(), bgRect.y(),
                           newWidth, bgRect.height());

    m_timeText->setPlainText(
        QString("Tiempo: %1 s").arg(remaining, 0, 'f', 1));
}

void Level2::updateHeartsHUD()
{
    for (int i = 0; i < m_heartIcons.size(); ++i) {
        if (m_heartIcons[i])
            m_heartIcons[i]->setVisible(i < m_lives);
    }

    if (m_livesText)
        m_livesText->setPlainText(QString("Vidas: %1").arg(m_lives));
}

void Level2::resetLevelState()
{
    clearObstacles();
    clearProjectiles();
    clearExplosions();

    m_elapsedTime = 0.0;
    m_lives = m_maxLives;

    if (m_player && !m_laneBaseY.isEmpty()) {
        QPixmap pm = m_player->pixmap();
        m_currentLaneIndex = 1; // carril medio
        double baseY = m_laneBaseY[m_currentLaneIndex];
        double y = baseY - pm.height();
        m_player->setPos(m_fixedPlayerX, y);
    }

    // aseguramos que la animación de correr está activa
    if (m_player)
        m_player->startRunning(1);

    updateTimeHUD();
    updateHeartsHUD();
}

void Level2::startLevel()
{
    resetLevelState();

    m_mainTimer->start(16);          // ~60 FPS
    m_obstacleTimer->start(1200);    // obstáculo cada 1.2 s
    m_projectileTimer->start(1800);  // proyectil cada 1.8 s
}

void Level2::stopLevel()
{
    m_mainTimer->stop();
    m_obstacleTimer->stop();
    m_projectileTimer->stop();
}

void Level2::updateGame()
{
    double dt = m_fixedDt;
    m_elapsedTime += dt;

    // jugador: solo se anima, X fija
    if (m_player) {
        m_player->update(dt);
        m_player->setX(m_fixedPlayerX);
    }

    updateObstacles(dt);
    updateProjectiles(dt);
    updateExplosions(dt);
    checkCollisions();

    updateTimeHUD();
    updateHeartsHUD();

    // victoria: sobrevivir hasta que se acabe el tiempo
    if (m_elapsedTime >= m_totalTime) {
        stopLevel();
        emit levelCompleted();
    }
}

void Level2::updateObstacles(double dt)
{
    QList<LaneObstacle*> toRemove;

    for (LaneObstacle *obs : m_obstacles) {
        if (!obs) continue;
        obs->update(dt);
        if (obs->x() + obs->pixmap().width() < 0.0) {
            toRemove.append(obs);
        }
    }

    for (LaneObstacle *obs : toRemove) {
        m_obstacles.removeOne(obs);
        m_scene->removeItem(obs);
        delete obs;
    }
}

void Level2::updateProjectiles(double dt)
{
    QList<TankProjectile*> toRemove;

    for (TankProjectile *p : m_projectiles) {
        if (!p) continue;

        p->update(dt);

        double bottomY = p->y() + p->pixmap().height();
        double centerX = p->x() + p->pixmap().width() / 2.0;

        // Si el proyectil llega a la carretera (suelo inferior), explota
        if (bottomY >= m_roadBottomY) {
            createExplosionAt(centerX, m_roadBottomY);
            toRemove.append(p);
        } else if (p->x() > m_scene->sceneRect().right() + 50.0) {
            // fuera de pantalla
            toRemove.append(p);
        }
    }

    for (TankProjectile *p : toRemove) {
        m_projectiles.removeOne(p);
        m_scene->removeItem(p);
        delete p;
    }
}

void Level2::updateExplosions(double dt)
{
    QList<Explosion*> finished;

    for (Explosion *exp : m_explosions) {
        if (!exp) continue;
        exp->update(dt);
        if (exp->isFinished())
            finished.append(exp);
    }

    for (Explosion *exp : finished) {
        m_explosions.removeOne(exp);
        m_scene->removeItem(exp);
        delete exp;
    }
}

void Level2::checkCollisions()
{
    if (!m_player)
        return;

    // Obstáculos
    QList<LaneObstacle*> obsToRemove;
    for (LaneObstacle *obs : m_obstacles) {
        if (!obs) continue;
        if (obs->collidesWithItem(m_player)) {
            obsToRemove.append(obs);
            --m_lives;
        }
    }
    for (LaneObstacle *obs : obsToRemove) {
        m_obstacles.removeOne(obs);
        m_scene->removeItem(obs);
        delete obs;
    }

    // Proyectiles
    QList<TankProjectile*> projToRemove;
    for (TankProjectile *p : m_projectiles) {
        if (!p) continue;
        if (p->collidesWithItem(m_player)) {
            double bottomY = m_laneBaseY[m_currentLaneIndex];
            double centerX = p->x() + p->pixmap().width() / 2.0;
            createExplosionAt(centerX, bottomY);
            projToRemove.append(p);
            --m_lives;
        }
    }
    for (TankProjectile *p : projToRemove) {
        m_projectiles.removeOne(p);
        m_scene->removeItem(p);
        delete p;
    }

    if (m_lives < 0) m_lives = 0;

    if (m_lives <= 0) {
        stopLevel();
        emit levelFailed();
    }
}

void Level2::spawnObstacle()
{
    if (!m_player || m_laneBaseY.isEmpty())
        return;

    // Obstáculo de alambre de púas
    QPixmap trapPixmap(":/assets/obstaculo_l2.png");
    if (trapPixmap.isNull()) {
        trapPixmap = QPixmap(50, 50);
        trapPixmap.fill(Qt::red);
    }

    QPixmap scaled = trapPixmap.scaled(80, 80,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    LaneObstacle *obs = new LaneObstacle(220.0); // velocidad hacia la izquierda
    obs->setPixmap(scaled);

    int lane = QRandomGenerator::global()->bounded(m_laneBaseY.size());
    double baseY = m_laneBaseY[lane];
    double y = baseY - scaled.height();

    double startX = m_scene->sceneRect().right() + 20.0;
    obs->setPos(startX, y);
    obs->setZValue(-1);

    m_scene->addItem(obs);
    m_obstacles.append(obs);
}

void Level2::spawnProjectile()
{
    if (!m_player || m_laneBaseY.isEmpty())
        return;

    QPixmap projPixmap(":/assets/proyectil_l2.png");
    if (projPixmap.isNull()) {
        projPixmap = QPixmap(20, 10);
        projPixmap.fill(Qt::yellow);
    }

    QPixmap scaled = projPixmap.scaled(64, 64,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    // Proyectil con trayectoria parabólica
    // vx > 0 hacia la derecha, vy < 0 hacia arriba
    double vx = 280.0;
    double vy = -220.0;
    double g  = 420.0;

    TankProjectile *proj = new TankProjectile(vx, vy, g);
    proj->setPixmap(scaled);

    int lane = QRandomGenerator::global()->bounded(m_laneBaseY.size());
    double baseY = m_laneBaseY[lane];
    double y = baseY - scaled.height();

    double startX = (m_tankItem
                         ? m_tankItem->x() + m_tankItem->pixmap().width()
                         : m_scene->sceneRect().left() + 40.0);
    proj->setPos(startX, y - 40.0);  // un poco por encima del carril
    proj->setZValue(-1);

    m_scene->addItem(proj);
    m_projectiles.append(proj);
}

void Level2::clearObstacles()
{
    for (LaneObstacle *obs : m_obstacles) {
        if (obs) {
            m_scene->removeItem(obs);
            delete obs;
        }
    }
    m_obstacles.clear();
}

void Level2::clearProjectiles()
{
    for (TankProjectile *p : m_projectiles) {
        if (p) {
            m_scene->removeItem(p);
            delete p;
        }
    }
    m_projectiles.clear();
}

void Level2::clearExplosions()
{
    for (Explosion *exp : m_explosions) {
        if (exp) {
            m_scene->removeItem(exp);
            delete exp;
        }
    }
    m_explosions.clear();
}

void Level2::createExplosionAt(double x, double y)
{
    if (m_explosionFrames.isEmpty())
        return;

    Explosion *exp = new Explosion(m_explosionFrames);
    exp->setFrameDuration(0.04);

    int w = exp->pixmap().width();
    int h = exp->pixmap().height();
    exp->setPos(x - w / 2.0, y - h);

    exp->setZValue(50);
    m_scene->addItem(exp);
    m_explosions.append(exp);
}

void Level2::changeLane(int delta)
{
    if (!m_player || m_laneBaseY.isEmpty())
        return;

    int newIndex = m_currentLaneIndex + delta;
    if (newIndex < 0 || newIndex >= m_laneBaseY.size())
        return;

    m_currentLaneIndex = newIndex;

    QPixmap pm = m_player->pixmap();
    double baseY = m_laneBaseY[m_currentLaneIndex];
    double newY = baseY - pm.height();
    m_player->setPos(m_fixedPlayerX, newY);
}

void Level2::keyPressEvent(QKeyEvent *event)
{
    if (!m_player) {
        BaseLevel::keyPressEvent(event);
        return;
    }

    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    switch (event->key()) {
    // cambiar de carril hacia arriba
    case Qt::Key_W:
    case Qt::Key_Up:
        changeLane(-1);
        break;

    // cambiar de carril hacia abajo
    case Qt::Key_S:
    case Qt::Key_Down:
        changeLane(1);
        break;

    default:
        BaseLevel::keyPressEvent(event);
        break;
    }
}

void Level2::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_player) {
        BaseLevel::keyReleaseEvent(event);
        return;
    }

    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    // No paramos la animación de correr nunca, así que
    // solo dejamos que BaseLevel maneje otras teclas si las hubiera
    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
    case Qt::Key_S:
    case Qt::Key_Down:
        // nada
        break;

    default:
        BaseLevel::keyReleaseEvent(event);
        break;
    }
}
