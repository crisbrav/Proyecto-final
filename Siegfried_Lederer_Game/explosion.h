#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QPixmap>

class Explosion : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Explosion(const QVector<QPixmap> &frames,
                       QGraphicsItem *parent = 0);

    void setFrameDuration(double seconds); // tiempo de cada frame
    void update(double dt);                // dt en segundos
    bool isFinished() const;

private:
    QVector<QPixmap> m_frames;
    int m_currentFrame;
    double m_frameDuration;
    double m_elapsed;
    bool m_finished;
};

#endif // EXPLOSION_H

