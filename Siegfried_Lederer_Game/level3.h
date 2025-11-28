// level3.h
#ifndef LEVEL3_H
#define LEVEL3_H

#include "baselevel.h"
#include <QList>
#include <QVector>
#include <QPixmap>

class Player;
class Bomb;
class Explosion;
class QMediaPlayer;
class QAudioOutput;
class QGraphicsTextItem;
class QGraphicsRectItem;
class QGraphicsPixmapItem;
class QTimer;

class Level3 : public BaseLevel
{
    Q_OBJECT

public:
    explicit Level3(QWidget *parent = 0);

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
    QList<Bomb*> m_bombsToRemove;
    QList<Explosion*> m_explosions;

    QTimer *m_bombSpawnTimer;
    QTimer *m_difficultyTimer;

    double m_elapsedTime;
    double m_totalTime;

    int m_lives;
    int m_maxLives;   // NUEVO: vidas máximas

    QMediaPlayer *m_bgm;
    QAudioOutput *m_audioOutput;


    double m_minX;
    double m_maxX;
    double m_groundY;

    // HUD
    QGraphicsTextItem  *m_timeText;
    QGraphicsTextItem  *m_livesText;   // si quieres seguir mostrando texto
    QGraphicsRectItem  *m_timeBarBg;   // barra gris
    QGraphicsRectItem  *m_timeBarFill; // barra naranja

    // Vidas (corazones)
    QVector<QGraphicsPixmapItem*> m_heartIcons;
    QPixmap m_heartPixmap;

    // Idle del jugador
    QPixmap m_idleRightFrame;
    QPixmap m_idleLeftFrame;
    bool    m_facingRight;

    // frames precargados para explosión
    QVector<QPixmap> m_explosionFrames;

    void setupScene();
    void resetLevelState();
    void checkCollisions();
    void removeBomb(Bomb *bomb);
    void createExplosionAt(double x, double y);

    // NUEVOS helpers
    void loadIdleSprites();
    void setupHud();
    void setupHearts();
    void updateHeartsHUD();
    void updateTimeHUD();

};

#endif // LEVEL3_H
