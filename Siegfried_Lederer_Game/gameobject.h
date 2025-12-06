#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointF>

class GameObject : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit GameObject(QGraphicsItem *parent = nullptr);

    void setVelocity(const QPointF &v);
    QPointF velocity() const;

    // dt en segundos
    virtual void update(double dt);

protected:
    QPointF m_velocity;
};

#endif // GAMEOBJECT_H
