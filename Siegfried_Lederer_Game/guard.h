#ifndef GUARD_H
#define GUARD_H

#include "gameobject.h"
#include <QVector>
#include <QPoint>
#include <QPixmap>

class Player;
class MazeGrid;

class Guard : public GameObject
{
    Q_OBJECT

public:
    void resetAI();   // reinicia estado (para cuando empieza el nivel)

    explicit Guard(MazeGrid *grid, QGraphicsItem *parent = 0);

    void setBaseSpeed(double speed);
    void setVisionRange(double rangePixels);
    void setPatrolPath(const QVector<QPoint> &cells);
    void setCurrentCell(const QPoint &cell);

    int timesSeenPlayer() const;

    // Carga el spritesheet completo (guard_l1.png)
    void loadSpriteSheet(const QPixmap &sheet);

    // IA: se llama desde el nivel en cada frame
    void updateAI(const Player *player, double dt);

private:
    enum State {
        Patrol,
        Chase
    };

    enum Direction {
        DirDown,
        DirRight,
        DirUp,
        DirLeft
    };

    MazeGrid *m_grid;
    State m_state;
    Direction m_dir;

    bool hasLineOfSight(const QPoint &from, const QPoint &to) const;  // LOS con paredes

    double m_baseSpeed;
    double m_currentSpeed;
    double m_visionRange;
    int m_timesSeenPlayer;

    QVector<QPoint> m_patrolPath;
    int m_patrolIndex;

    QVector<QPoint> m_route;
    int m_routeIndex;

    QPoint m_currentCell;

    // animación
    QVector<QPixmap> m_framesDown;
    QVector<QPixmap> m_framesRight;
    QVector<QPixmap> m_framesUp;
    QVector<QPixmap> m_framesLeft;
    double m_animTimer;
    double m_frameDuration;
    int m_animIndex;
    bool m_isMoving;

    bool canSeePlayer(const Player *player) const;
    void followRoute(double dt);
    void moveTowardsCell(const QPoint &cell, double dt);
    void learn();

    void updateAnimation(double dt);
    const QVector<QPixmap> &currentFrames() const;
};

#endif // GUARD_H
