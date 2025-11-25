#include "level3.h"

#include "player.h"
#include "bomb.h"
#include "explosion.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QtMath>


Level3::Level3(QWidget *parent)
    : BaseLevel(parent),
    m_player(0),
    m_bombSpawnTimer(new QTimer(this)),
    m_difficultyTimer(new QTimer(this)),
    m_elapsedTime(0.0),
    m_totalTime(60.0),
    m_lives(3),
    m_bgm(new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_minX(0.0),
    m_maxX(0.0),
    m_groundY(0.0),
    m_timeText(0),
    m_livesText(0)
{
    m_bgm->setAudioOutput(m_audioOutput);

    QPixmap sheet(":/assets/explosion_l3.png");
    if (!sheet.isNull()) {
        int frameHeight = sheet.height();            // 128
        int columns     = sheet.width() / frameHeight; // 1536/128 = 12
        int frameWidth  = frameHeight;               // es cuadrado

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

    setupScene();

    connect(m_bombSpawnTimer, &QTimer::timeout,
            this, &Level3::spawnBomb);

    connect(m_difficultyTimer, &QTimer::timeout,
            this, &Level3::increaseDifficulty);
}


void Level3::setupScene()
{
    // Intentar cargar el fondo desde recursos
    QPixmap bgPixmap(":/assets/fondo_level3_800.png");

    int width = 800;
    int height = 600;

    if (!bgPixmap.isNull()) {
        width = bgPixmap.width();
        height = bgPixmap.height();
        m_scene->setSceneRect(0, 0, width, height);

        QGraphicsPixmapItem *bgItem = m_scene->addPixmap(bgPixmap);
        bgItem->setPos(0, 0);
        bgItem->setZValue(-100);
    } else {
        // Fondo por defecto si falla
        m_scene->setSceneRect(0, 0, width, height);
        QGraphicsRectItem *background =
            m_scene->addRect(0, 0, width, height,
                             QPen(Qt::NoPen), QBrush(Qt::darkBlue));
        background->setZValue(-100);
    }

    // Definimos una altura aproximada de suelo (un poco por encima del borde inferior)
    m_groundY = height - 90.0;

    // Límites horizontales para el jugador
    m_minX = 20.0;
    m_maxX = width - 20.0;


    m_player = new Player();

    QPixmap sheet(":/assets/prisionero_l3.png");
    QVector<QPixmap> frames;

    if (!sheet.isNull()) {
        // En la imagen que enviaste hay 9 frames horizontales
        const int columns = 10;
        const int frameWidth  = sheet.width() / columns;
        const int frameHeight = sheet.height();  // una sola fila

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
        // Si falla el sprite sheet, usamos un cuadrado amarillo
        QPixmap pm(40, 40);
        pm.fill(Qt::yellow);
        m_player->setPixmap(pm);
    }


    m_player->setZValue(0);

    // Posición inicial: centrado sobre el "suelo"
    QPixmap pm = m_player->pixmap();
    m_player->setPos(width / 2.0 - pm.width() / 2.0,
                     m_groundY - pm.height());

    m_scene->addItem(m_player);

    m_timeText = m_scene->addText("Tiempo: 60.0");
    m_timeText->setDefaultTextColor(Qt::white);
    m_timeText->setZValue(200);
    m_timeText->setPos(10, 10);

    m_livesText = m_scene->addText("Vidas: 3");
    m_livesText->setDefaultTextColor(Qt::white);
    m_livesText->setZValue(200);
    m_livesText->setPos(10, 40);

    m_view->setSceneRect(m_scene->sceneRect());
    m_view->setFixedSize(width, height);
}



void Level3::resetLevelState()
{
    // borrar bombas
    for (Bomb *bomb : m_bombs) {
        if (bomb) {
            m_scene->removeItem(bomb);
            delete bomb;
        }
    }
    m_bombs.clear();
    m_bombsToRemove.clear();

    // borrar explosiones
    for (Explosion *exp : m_explosions) {
        if (exp) {
            m_scene->removeItem(exp);
            delete exp;
        }
    }
    m_explosions.clear();

    m_elapsedTime = 0.0;
    m_lives = 3;

    if (m_player) {
        QPixmap pm = m_player->pixmap();
        m_player->setPos(m_scene->width() / 2.0 - pm.width() / 2.0,
                         m_groundY - pm.height());
    }

    m_bombSpawnTimer->setInterval(800);

    if (m_timeText)
        m_timeText->setPlainText("Tiempo: 60.0");
    if (m_livesText)
        m_livesText->setPlainText("Vidas: 3");
}


void Level3::startLevel()
{
    resetLevelState();


    m_bgm->setSource(QUrl("qrc:/assets/363_full_game-of-rings_0155_preview.mp3"));
    m_audioOutput->setVolume(0.4); // volumen entre 0.0 y 1.0
    m_bgm->setLoops(QMediaPlayer::Infinite);
    m_bgm->play();

    m_mainTimer->start(16);           // ~60 FPS
    m_bombSpawnTimer->start();        // usa el intervalo ya asignado
    m_difficultyTimer->start(15000);  // cada 15 s
}


void Level3::updateGame()
{
    double dt = m_fixedDt;
    m_elapsedTime += dt;

    // jugador
    if (m_player) {
        m_player->update(dt);

        if (m_player->x() < m_minX)
            m_player->setX(m_minX);
        if (m_player->x() + m_player->pixmap().width() > m_maxX)
            m_player->setX(m_maxX - m_player->pixmap().width());
    }

    // bombas (caída)
    for (Bomb *bomb : m_bombs) {
        if (bomb)
            bomb->update(dt);
    }

    // colisiones bomba-jugador (solo marca para borrar)
    checkCollisions();

    // bombas que tocan el suelo
    QList<Bomb*> groundHits;
    for (Bomb *bomb : m_bombs) {
        if (!bomb) continue;

        double bottomY = bomb->y() + bomb->pixmap().height();
        if (bottomY >= m_groundY) {
            double centerX = bomb->x() + bomb->pixmap().width() / 2.0;
            createExplosionAt(centerX, m_groundY);
            groundHits.append(bomb);
        }
    }

    // juntar todas las bombas a borrar (colisiones + suelo)
    for (Bomb *b : groundHits) {
        if (!m_bombsToRemove.contains(b))
            m_bombsToRemove.append(b);
    }

    // borrar bombas marcadas
    for (Bomb *bomb : m_bombsToRemove) {
        removeBomb(bomb);
    }
    m_bombsToRemove.clear();

    // actualizar explosiones y limpiar las que terminan
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

    // actualizar HUD
    double remaining = m_totalTime - m_elapsedTime;
    if (remaining < 0.0) remaining = 0.0;

    if (m_timeText)
        m_timeText->setPlainText(
            QString("Tiempo: %1").arg(remaining, 0, 'f', 1));

    if (m_livesText)
        m_livesText->setPlainText(
            QString("Vidas: %1").arg(m_lives));

    // comprobar fin de nivel
    if (m_elapsedTime >= m_totalTime) {
        m_mainTimer->stop();
        m_bombSpawnTimer->stop();
        m_difficultyTimer->stop();
        m_bgm->stop();
        emit levelCompleted();
    }
}



void Level3::spawnBomb()
{
    if (!m_player)
        return;

    Bomb *bomb = new Bomb();

    QPixmap bombPixmap(":/assets/drop_bomb.png");
    if (bombPixmap.isNull()) {
        // fallback: cuadrado rojo
        QPixmap pm(20, 20);
        pm.fill(Qt::red);
        bomb->setPixmap(pm);
    } else {
        // Escalar para que no sea gigante
        QPixmap scaled = bombPixmap.scaled(48, 48,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
        bomb->setPixmap(scaled);
    }

    const int pixWidth = bomb->pixmap().width();
    const int pixHeight = bomb->pixmap().height();

    double x = QRandomGenerator::global()
                   ->bounded(static_cast<int>(m_scene->width() - pixWidth));
    double y = -pixHeight; // empieza fuera de pantalla arriba

    bomb->setPos(x, y);
    bomb->setZValue(-1);

    m_scene->addItem(bomb);
    m_bombs.append(bomb);
}


void Level3::increaseDifficulty()
{
    int interval = m_bombSpawnTimer->interval();
    // No dejar que el intervalo baje demasiado
    if (interval > 200) {
        m_bombSpawnTimer->setInterval(interval - 100);
    }
}

void Level3::checkCollisions()
{
    if (!m_player)
        return;

    for (Bomb *bomb : m_bombs) {
        if (!bomb) continue;

        if (bomb->collidesWithItem(m_player)) {
            // explosión sobre el jugador
            double centerX = m_player->x() + m_player->pixmap().width() / 2.0;
            double centerY = m_player->y();
            createExplosionAt(centerX, centerY);

            if (!m_bombsToRemove.contains(bomb))
                m_bombsToRemove.append(bomb);

            // restar vida
            --m_lives;
            // TODO: sonido de perder vida

            if (m_lives <= 0) {
                m_mainTimer->stop();
                m_bombSpawnTimer->stop();
                m_difficultyTimer->stop();
                m_bgm->stop();
                emit levelFailed();
                return;
            }
        }
    }
}


void Level3::removeBomb(Bomb *bomb)
{
    if (!bomb)
        return;

    m_bombs.removeOne(bomb);
    m_scene->removeItem(bomb);
    delete bomb;
}


void Level3::keyPressEvent(QKeyEvent *event)
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
    case Qt::Key_A:
    case Qt::Key_Left:
        m_player->setMoveDirection(-1);
        m_player->startRunning(-1);
        break;

    case Qt::Key_D:
    case Qt::Key_Right:
        m_player->setMoveDirection(1);
        m_player->startRunning(1);
        break;

    default:
        BaseLevel::keyPressEvent(event);
        break;
    }
}


void Level3::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_player) {
        BaseLevel::keyReleaseEvent(event);
        return;
    }

    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        if (m_player->moveDirection() < 0) {
            m_player->setMoveDirection(0);
            m_player->stopRunning();
        }
        break;

    case Qt::Key_D:
    case Qt::Key_Right:
        if (m_player->moveDirection() > 0) {
            m_player->setMoveDirection(0);
            m_player->stopRunning();
        }
        break;

    default:
        BaseLevel::keyReleaseEvent(event);
        break;
    }
}

void Level3::createExplosionAt(double x, double y)
{
    if (m_explosionFrames.isEmpty())
        return;

    Explosion *exp = new Explosion(m_explosionFrames);
    exp->setFrameDuration(0.04); // ajusta si quieres más lenta/rápida

    // centrarla en (x, y) (y ≈ suelo)
    int w = exp->pixmap().width();
    int h = exp->pixmap().height();
    exp->setPos(x - w / 2.0, y - h);

    exp->setZValue(50);
    m_scene->addItem(exp);
    m_explosions.append(exp);
}



