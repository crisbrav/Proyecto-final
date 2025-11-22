#ifndef LEVEL1_H
#define LEVEL1_H

#include "baselevel.h"

class Player;
class Guard;
class MazeGrid;

class Level1 : public BaseLevel
{
    Q_OBJECT

public:
    explicit Level1(QWidget *parent = nullptr);

    void startLevel() override;

protected slots:
    void updateGame() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Player *m_player;
    QList<Guard*> m_guards;
    MazeGrid *m_grid;

    void setupScene();
    void setupMaze();
    void checkCollisions();
};



#endif // LEVEL1_H
