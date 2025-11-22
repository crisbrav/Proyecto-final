#ifndef LEVEL2_H
#define LEVEL2_H

#include <QGraphicsItem>
#include "baselevel.h"

class Player;
class Projectile;

class Level2 : public BaseLevel
{
    Q_OBJECT

public:
    explicit Level2(QWidget *parent = nullptr);

    void startLevel() override;

protected slots:
    void updateGame() override;
    void spawnObstacleOrProjectile();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Player *m_player;
    QList<QGraphicsItem*> m_obstacles;
    QList<Projectile*> m_projectiles;

    QTimer *m_spawnTimer;
    int m_lives;

    void setupScene();
    void checkCollisions();
};



#endif // LEVEL2_H
