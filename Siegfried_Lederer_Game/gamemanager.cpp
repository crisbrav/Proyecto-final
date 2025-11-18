// gamemanager.cpp
#include "gamemanager.h"

GameManager::GameManager(QObject *parent)
    : QObject(parent),
    m_currentLevel(1),
    m_lives(3)
{
}

void GameManager::startLevel(int level)
{
    m_currentLevel = level;
    emit levelChanged(m_currentLevel);
}

int GameManager::getCurrentLevel() const
{
    return m_currentLevel;
}

void GameManager::setLives(int lives)
{
    if (m_lives == lives)
        return;

    m_lives = lives;
    emit livesChanged(m_lives);
}

int GameManager::getLives() const
{
    return m_lives;
}
