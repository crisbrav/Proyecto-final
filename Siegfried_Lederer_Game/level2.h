#ifndef LEVEL2_H
#define LEVEL2_H

#include "baselevel.h"

#include <QList>
#include <QVector>
#include <QPixmap>

class Player;
class QTimer;
class QGraphicsTextItem;
class QGraphicsRectItem;
class QGraphicsPixmapItem;

class LaneObstacle;
class TankProjectile;
class Explosion;

class Level2 : public BaseLevel
{
    Q_OBJECT

public:
    explicit Level2(QWidget *parent = 0);

    void startLevel() override;
    void stopLevel() override;

signals:
    void levelCompleted();
    void levelFailed();

protected slots:
    void updateGame() override;

private slots:
    void spawnObstacle();
    void spawnProjectile();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // --- Jugador ---
    Player *m_player;
    int     m_currentLaneIndex;   // 0 = carril alto, 1 = medio, 2 = bajo
    double  m_fixedPlayerX;       // X fija del prisionero (solo se mueve en Y)

    // --- Carriles / carretera ---
    QVector<double> m_laneBaseY;  // posición Y del "suelo" de cada carril
    double          m_roadTopY;   // y = 371
    double          m_roadBottomY;// y = 559

    // --- Obstáculos y proyectiles ---
    QList<LaneObstacle*>    m_obstacles;
    QList<TankProjectile*>  m_projectiles;
    QTimer                 *m_obstacleTimer;
    QTimer                 *m_projectileTimer;
    struct ProjectileInfo {
        QGraphicsPixmapItem *indicator; // mira
        double impactY;                 // altura donde debe explotar
    };

    QHash<TankProjectile*, ProjectileInfo> m_projectileInfos;

    // Fondo desplazable
    QGraphicsPixmapItem *m_bg1;
    QGraphicsPixmapItem *m_bg2;
    double m_bgScrollSpeed;   // píxeles/segundo

    void updateBackground(double dt);


    // --- Tanque ---
    QGraphicsPixmapItem *m_tankItem;
    double               m_tankMuzzleX;

    // --- Tiempo del nivel ---
    double m_elapsedTime;
    double m_totalTime;

    // --- Vidas ---
    int m_lives;
    int m_maxLives;

    // --- HUD: tiempo ---
    QGraphicsTextItem  *m_timeText;
    QGraphicsRectItem  *m_timeBarBg;
    QGraphicsRectItem  *m_timeBarFill;

    // --- HUD: vidas (corazones) ---
    QVector<QGraphicsPixmapItem*> m_heartIcons;
    QGraphicsTextItem  *m_livesText;
    QPixmap             m_heartPixmap;

    // --- Explosiones ---
    QVector<QPixmap> m_explosionFrames;
    QList<Explosion*> m_explosions;

    // --- Métodos auxiliares ---
    void setupScene();
    void setupHud();
    void setupHearts();

    void resetLevelState();
    void updateTimeHUD();
    void updateHeartsHUD();

    void changeLane(int delta);  // -1: subir carril, +1: bajar carril

    void updateObstacles(double dt);
    void updateProjectiles(double dt);
    void updateExplosions(double dt);
    void checkCollisions();

    void clearObstacles();
    void clearProjectiles();
    void clearExplosions();

    void createExplosionAt(double x, double y);
    void loadExplosionFrames();
};

#endif // LEVEL2_H
