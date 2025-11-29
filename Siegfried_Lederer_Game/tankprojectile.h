#ifndef TANKPROJECTILE_H
#define TANKPROJECTILE_H

#include <QGraphicsPixmapItem>

class TankProjectile : public QGraphicsPixmapItem
{
public:
    // vx: velocidad horizontal (px/s, positiva hacia la derecha)
    // vy: velocidad vertical inicial (px/s, NEGATIVA hacia arriba)
    // gravity: aceleración vertical (px/s^2, positiva hacia abajo)
    explicit TankProjectile(double vx = 0.0,
                            double vy = 0.0,
                            double gravity = 400.0,
                            QGraphicsItem *parent = 0);

    void setVelocity(double vx, double vy);
    void setGravity(double g);

    double vx() const { return m_vx; }
    double vy() const { return m_vy; }
    double gravity() const { return m_gravity; }

    // actualiza la posición (trayectoria parabólica)
    void update(double dt);

private:
    double m_vx;       // velocidad horizontal
    double m_vy;       // velocidad vertical
    double m_gravity;  // aceleración vertical (gravedad)
};

#endif // TANKPROJECTILE_H
