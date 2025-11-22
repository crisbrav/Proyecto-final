// level1.cpp
#include "level1.h"

#include "player.h"
#include "guard.h"
//#include "mazegrid.h"

#include <QKeyEvent>

Level1::Level1(QWidget *parent)
    : BaseLevel(parent),
    m_player(nullptr),
    m_grid(nullptr)
{
    setupScene();
}

void Level1::setupScene()
{
    // TODO: crear MazeGrid, paredes, tiles, jugador y guardias
}

void Level1::setupMaze()
{
    // TODO: cargar matriz del laberinto y dibujarla en la escena
}

void Level1::startLevel()
{
    setupMaze();
    //m_mainTimer->start(16);
}

void Level1::updateGame()
{
    double dt = 0.016; // simplificado

    // TODO: actualizar jugador, guardias, IA

    checkCollisions();
}

void Level1::checkCollisions()
{
    // TODO: revisar si guardias tocan al jugador, etc.
}

void Level1::keyPressEvent(QKeyEvent *event)
{
    double dt = 0.016;
    switch (event->key()) {
    case Qt::Key_W:
        // mover hacia arriba
        // m_player->moveUp(dt);
        break;
    case Qt::Key_S:
        // m_player->moveDown(dt);
        break;
    case Qt::Key_A:
        // m_player->moveLeft(dt);
        break;
    case Qt::Key_D:
        // m_player->moveRight(dt);
        break;
    default:
        BaseLevel::keyPressEvent(event);
        break;
    }
}
