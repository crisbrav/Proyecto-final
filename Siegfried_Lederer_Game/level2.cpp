// level2.cpp
#include "level2.h"

#include "player.h"
#include "projectile.h"

#include <QTimer>
#include <QKeyEvent>

Level2::Level2(QWidget *parent)
    : BaseLevel(parent),
    m_player(nullptr),
    m_spawnTimer(new QTimer(this)),
    m_lives(3)
{
    setupScene();

    connect(m_spawnTimer, &QTimer::timeout,
            this, &Level2::spawnObstacleOrProjectile);
}

void Level2::setupScene()
{
    // TODO: fondo de carretera, jugador en posición inicial, etc.
}

void Level2::startLevel()
{
    m_lives = 3;
    m_mainTimer->start(16);
    m_spawnTimer->start(1000); // cada segundo (ajustar)
}

void Level2::updateGame()
{
    double dt = 0.016;
    Q_UNUSED(dt);

    // TODO: mover obstáculos/proyectiles hacia el jugador

    checkCollisions();
}

void Level2::spawnObstacleOrProjectile()
{
    // TODO: crear obstáculos y/o proyectiles
}

void Level2::checkCollisions()
{
    // TODO: detectar colisiones y restar vidas
}

void Level2::keyPressEvent(QKeyEvent *event)
{
    double dt = 0.016;
    switch (event->key()) {
    case Qt::Key_W:
        // cambiar carril arriba
        break;
    case Qt::Key_S:
        // cambiar carril abajo
        break;
    default:
        BaseLevel::keyPressEvent(event);
        break;
    }
}
