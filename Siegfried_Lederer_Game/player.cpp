#include "player.h"
#include <QTransform>

Player::Player(QGraphicsItem *parent)
    : GameObject(parent),
    m_speed(300),
    m_lives(3),
    m_currentFrame(0),
    m_animTimer(0.0),
    m_frameDuration(0.12),   // ~16 fps
    m_isRunning(false),
    m_direction(1),
    m_moveDir(0)

{
}

void Player::setSpeed(double speed)
{
    m_speed = speed;
}

double Player::speed() const
{
    return m_speed;
}

void Player::setMoveDirection(int dir)
{
    if (dir < 0) m_moveDir = -1;
    else if (dir > 0) m_moveDir = 1;
    else m_moveDir = 0;
}

int Player::moveDirection() const
{
    return m_moveDir;
}


void Player::setLives(int lives)
{
    if (m_lives == lives)
        return;

    m_lives = lives;
    emit livesChanged(m_lives);
}

int Player::lives() const
{
    return m_lives;
}

void Player::setAnimationFrames(const QVector<QPixmap> &framesRight)
{
    m_framesRight = framesRight;
    m_framesLeft.clear();

    // pre-computamos los frames espejados para la izquierda
    QTransform flip;
    flip.scale(-1.0, 1.0);

    for (int i = 0; i < m_framesRight.size(); ++i) {
        m_framesLeft.append(m_framesRight[i].transformed(flip));
    }

    if (!m_framesRight.isEmpty()) {
        m_currentFrame = 0;
        setPixmap(m_framesRight[0]);
    }
}

void Player::update(double dt)
{
    // aplicar movimiento continuo según la dirección
    if (m_moveDir != 0) {
        setPos(x() + m_moveDir * m_speed * dt, y());
    }

    updateAnimation(dt);
}


void Player::moveLeft(double dt)
{
    setPos(x() - m_speed * dt, y());
}

void Player::moveRight(double dt)
{
    setPos(x() + m_speed * dt, y());
}

void Player::startRunning(int direction)
{
    if (direction < 0)
        m_direction = -1;
    else
        m_direction = 1;

    m_isRunning = true;
    // opcional: resetear el timer
    // m_animTimer = 0.0;
}

void Player::stopRunning()
{
    m_isRunning = false;

    // Volver al primer frame
    if (!m_framesRight.isEmpty()) {
        m_currentFrame = 0;
        if (m_direction >= 0)
            setPixmap(m_framesRight[0]);
        else
            setPixmap(m_framesLeft[0]);
    }
}

void Player::updateAnimation(double dt)
{
    if (!m_isRunning || m_framesRight.isEmpty())
        return;

    m_animTimer += dt;
    while (m_animTimer >= m_frameDuration) {
        m_animTimer -= m_frameDuration;
        m_currentFrame = (m_currentFrame + 1) % m_framesRight.size();

        if (m_direction >= 0)
            setPixmap(m_framesRight[m_currentFrame]);
        else
            setPixmap(m_framesLeft[m_currentFrame]);
    }
}
