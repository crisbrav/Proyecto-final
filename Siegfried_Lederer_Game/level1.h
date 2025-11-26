#ifndef LEVEL1_H
#define LEVEL1_H

#include "baselevel.h"
#include <QList>
#include <QGraphicsRectItem>
#include <QSet>

class Player;
class Guard;
class MazeGrid;

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
    QList<Guard*> m_guards;
    MazeGrid *m_grid;

    QGraphicsRectItem *m_exitRect;

    QSet<int> m_keysPressed; // teclas presionadas (WASD)

    void setupScene();
    void setupMazeGraphics();
    void resetLevelState();
    void updatePlayerMovement(double dt);
    void updateGuards(double dt);
    void checkCollisions();
};

#endif // LEVEL1_H
