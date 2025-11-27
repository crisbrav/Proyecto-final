#ifndef LEVEL1_H
#define LEVEL1_H

#include "baselevel.h"
#include <QList>
#include <QSet>
#include <QVector>
#include <QPixmap>
#include <QGraphicsRectItem>

class Player;
class Guard;
class MazeGrid;
class MovingTrap;
class Level1 : public BaseLevel
{
    Q_OBJECT

public:
    explicit Level1(QWidget *parent = 0);

    void startLevel() override;

protected slots:
    void updateGame() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Player *m_player;
    QList<MovingTrap*> m_traps;
    QList<Guard*> m_guards;
    MazeGrid *m_grid;

    QGraphicsRectItem *m_exitRect;

    QSet<int> m_keysPressed; // teclas presionadas (WASD)

    // sprites jugador top-down
    QVector<QPixmap> m_playerDownFrames;
    QVector<QPixmap> m_playerUpFrames;
    QVector<QPixmap> m_playerRightFrames;
    QVector<QPixmap> m_playerLeftFrames;

    double m_playerAnimTimer;
    double m_playerFrameDuration;
    int m_playerFrameIndex;
    enum PlayerDir { PDown, PUp, PLeft, PRight };
    PlayerDir m_playerDir;
    bool m_playerMoving;

    void setupScene();
    void setupMazeGraphics();
    void resetLevelState();

    void loadPlayerSprites();
    void setupTraps();
    void updatePlayerMovement(double dt);
    void updatePlayerAnimation(double dt);
    void updateGuards(double dt);
    void checkCollisions();
};

#endif // LEVEL1_H
