#include "level3.h"

#include "player.h"
#include "bomb.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

Level3::Level3(QWidget *parent)
    : BaseLevel(parent),
    m_player(nullptr),
    m_bombSpawnTimer(new QTimer(this)),
    m_difficultyTimer(new QTimer(this)),
    m_elapsedTime(0.0),
    m_totalTime(60.0),
    m_lives(3),
    m_bgm(new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_minX(0.0),
    m_maxX(0.0),
    m_groundY(0.0)
{
    m_bgm->setAudioOutput(m_audioOutput);
    setupScene();

    // Spawner de bombas
    connect(m_bombSpawnTimer, &QTimer::timeout,
            this, &Level3::spawnBomb);

    // Aumento de dificultad cada 15 s
    connect(m_difficultyTimer, &QTimer::timeout,
            this, &Level3::increaseDifficulty);
}

void Level3::setupScene()
{
    const int width = 800;
    const int height = 600;
    m_scene->setSceneRect(0, 0, width, height);

    // Fondo simple (luego lo puedes cambiar por sprite)
    QGraphicsRectItem *background =
        m_scene->addRect(0, 0, width, height, QPen(Qt::NoPen), QBrush(Qt::darkBlue));
    background->setZValue(-100);

    // Suelo
    const int groundHeight = 40;
    m_groundY = height - groundHeight;

    QGraphicsRectItem *ground =
        m_scene->addRect(0, m_groundY, width, groundHeight,
                         QPen(Qt::NoPen), QBrush(Qt::darkGreen));
    ground->setZValue(-50);

    // Límites horizontales del jugador
    m_minX = 20.0;
    m_maxX = width - 20.0;

    // ---------- SPRITE SHEET DEL JUGADOR ----------
    QPixmap sheet(":/assets/prisoner_level3.png");
    if (sheet.isNull()) {
        // fallback: sprite amarillo si algo falla
        QPixmap pm(40, 40);
        pm.fill(Qt::yellow);
        m_player = new Player();
        m_player->setPixmap(pm);
    } else {
        // sprite sheet: 2 filas x 8 columnas (1280 x 256)
        const int columns = 8;
        const int rows = 2;
        const int frameWidth  = sheet.width() / columns;  // 160
        const int frameHeight = sheet.height() / rows;    // 128

        // cogemos por ahora el primer frame (fila 0, columna 0)
        QPixmap frame = sheet.copy(0, 0, frameWidth, frameHeight);

        // Escalamos para que encaje en la escena (opcional)
        QPixmap scaled = frame.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_player = new Player();
        m_player->setPixmap(scaled);
    }

    m_player->setZValue(0);
    // posición inicial sobre el suelo
    m_player->setPos(width / 2.0 - m_player->pixmap().width() / 2.0,
                     m_groundY - m_player->pixmap().height());
    m_scene->addItem(m_player);

    // Ajustar vista
    m_view->setSceneRect(m_scene->sceneRect());
    m_view->setFixedSize(width, height);
}


void Level3::resetLevelState()
{
    // eliminar bombas existentes
    for (Bomb *bomb : m_bombs) {
        if (bomb) {
            m_scene->removeItem(bomb);
            delete bomb;
        }
    }
    m_bombs.clear();

    m_elapsedTime = 0.0;
    m_lives = 3;

    // Reiniciar posición del jugador
    if (m_player) {
        QPixmap pm = m_player->pixmap();
        m_player->setPos(m_scene->width() / 2.0 - pm.width() / 2.0,
                         m_groundY - pm.height());
    }

    // Intervalo inicial de spawn (ms)
    m_bombSpawnTimer->setInterval(800);
}

void Level3::startLevel()
{
    resetLevelState();

    // ---------- MÚSICA DE FONDO ----------
    // Si está en resources.qrc con prefix "/assets":
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

    // Actualizar bombas (MRUA)
    for (Bomb *bomb : m_bombs) {
        if (bomb) {
            bomb->update(dt);
        }
    }

    checkCollisions();

    // Eliminar bombas que se salen de la pantalla (debajo del suelo)
    QList<Bomb*> bombsToRemove;
    for (Bomb *bomb : m_bombs) {
        if (!bomb) continue;
        if (bomb->y() > m_scene->height() + 100) {
            bombsToRemove.append(bomb);
        }
    }
    for (Bomb *bomb : bombsToRemove) {
        removeBomb(bomb);
    }

    // ¿Se acabó el tiempo?
    if (m_elapsedTime >= m_totalTime) {
        stopLevel();
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

    QList<Bomb*> collidedBombs;

    for (Bomb *bomb : m_bombs) {
        if (!bomb) continue;
        if (bomb->collidesWithItem(m_player)) {
            collidedBombs.append(bomb);
        }
    }

    for (Bomb *bomb : collidedBombs) {
        removeBomb(bomb);
        m_lives--;



        if (m_lives <= 0) {
            stopLevel();
            m_bombSpawnTimer->stop();
            m_difficultyTimer->stop();
            m_bgm->stop();

            emit levelFailed();
            break;
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
    double dt = m_fixedDt;

    if (!m_player) {
        BaseLevel::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        m_player->moveLeft(dt);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        m_player->moveRight(dt);
        break;
    default:
        BaseLevel::keyPressEvent(event);
        break;
    }

    // Limitar movimiento del jugador dentro de [m_minX, m_maxX]
    if (m_player->x() < m_minX) {
        m_player->setX(m_minX);
    }
    if (m_player->x() + m_player->pixmap().width() > m_maxX) {
        m_player->setX(m_maxX - m_player->pixmap().width());
    }
}
