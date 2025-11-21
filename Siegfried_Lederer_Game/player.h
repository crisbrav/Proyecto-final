#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"

class Player : public GameObject
{
    Q_OBJECT

public:
    explicit Player(QGraphicsItem *parent = nullptr);

    void setSpeed(double speed);
    double speed() const;

    void setLives(int lives);
    int lives() const;

    // Movimiento horizontal para este nivel
    void moveLeft(double dt);
    void moveRight(double dt);

signals:
    void livesChanged(int lives);

private:
    double m_speed;   // px/s
    int m_lives;
};

#endif // PLAYER_H

