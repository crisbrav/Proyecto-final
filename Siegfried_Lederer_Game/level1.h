#ifndef LEVEL1_H
#define LEVEL1_H

#include "baselevel.h"
#include <QList>
#include <QSet>
#include <QVector>
#include <QPixmap>
#include <QGraphicsRectItem>
#include <QMediaPlayer>
#include <QAudioOutput>


class Player;
class Guard;
class MazeGrid;
class MovingTrap;
class QMediaPlayer;
class QAudioOutput;
class Level1 : public BaseLevel
{
    Q_OBJECT

public:
    explicit Level1(QWidget *parent = 0);

    void stopLevel() override;
    void startLevel() override;

protected slots:
    void updateGame() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:

    struct KeyData {
        QGraphicsPixmapItem *item;
        bool isBlue;
        bool collected;
    };


    // --- llaves ---
    QVector<KeyData> m_keys;
    int m_blueKeysCollected;
    int m_whiteKeysCollected;
    int m_blueKeysNeeded;
    int m_whiteKeysNeeded;

    // --- vidas / corazones ---
    static const int MAX_LIVES = 5;
    int m_lives;
    QVector<QGraphicsPixmapItem*> m_heartIcons;
    QPixmap m_heartPixmap;

    // ---- Audio ----
    QMediaPlayer  *m_bgm;              // música del nivel
    QAudioOutput  *m_bgmOutput;

    QMediaPlayer  *m_guardHitSfx;      // sonido cuando te atrapa un guardia
    QAudioOutput  *m_guardHitOutput;


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

    void setupKeys();
    void resetKeys();
    void setupHearts();
    void updateHeartsHUD();


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
