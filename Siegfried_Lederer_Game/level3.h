#ifndef LEVEL3_H
#define LEVEL3_H

#include "baselevel.h"
#include <QList>

class Player;
class Bomb;
class QTimer;
class QMediaPlayer;
class QAudioOutput;

class Level3 : public BaseLevel
{
    Q_OBJECT

public:
    explicit Level3(QWidget *parent = nullptr);

    void startLevel() override;

protected slots:
    void updateGame() override;
    void spawnBomb();
    void increaseDifficulty();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Player *m_player;
    QList<Bomb*> m_bombs;

    QTimer *m_bombSpawnTimer;
    QTimer *m_difficultyTimer;

    double m_elapsedTime;   // tiempo acumulado
    double m_totalTime;     // tiempo objetivo (60 s)

    int m_lives;

    // para la música de fondo
    QMediaPlayer *m_bgm;
    QAudioOutput *m_audioOutput;

    // límites horizontales para el jugador
    double m_minX;
    double m_maxX;
    double m_groundY;

    void setupScene();
    void resetLevelState();
    void checkCollisions();
    void removeBomb(Bomb *bomb);
};

#endif // LEVEL3_H

