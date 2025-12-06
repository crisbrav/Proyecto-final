#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"
#include <QVector>
#include <QPixmap>

class Player : public GameObject
{
    Q_OBJECT

public:
    explicit Player(QGraphicsItem *parent = 0);

    void setSpeed(double speed);
    double speed() const;

    void setLives(int lives);
    int lives() const;

    // Recibe todos los frames de la animación hacia la derecha
    void setAnimationFrames(const QVector<QPixmap> &framesRight);

    // dt viene del bucle del juego
    void update(double dt) override;

    // Para nivel 3: movimiento horizontal
    void moveLeft(double dt);
    void moveRight(double dt);

    // Control de animación según la tecla
    // direction: -1 izquierda, 1 derecha
    void startRunning(int direction);
    void stopRunning();

    void setMoveDirection(int dir);
    int moveDirection() const;

signals:
    void livesChanged(int lives);

private:
    double m_speed;
    int m_lives;

    QVector<QPixmap> m_framesRight;
    QVector<QPixmap> m_framesLeft;
    int m_currentFrame;
    double m_animTimer;
    double m_frameDuration;
    bool m_isRunning;
    int m_direction;      // -1 izquierda, 1 derecha

    int m_moveDir;

    void updateAnimation(double dt);
};

#endif // PLAYER_H
