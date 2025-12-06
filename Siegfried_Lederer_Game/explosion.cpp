#include "explosion.h"

Explosion::Explosion(const QVector<QPixmap> &frames, QGraphicsItem *parent)
    : QObject(),
    QGraphicsPixmapItem(parent),
    m_frames(frames),
    m_currentFrame(0),
    m_frameDuration(0.05), // por defecto: rápida
    m_elapsed(0.0),
    m_finished(false)
{
    if (!m_frames.isEmpty()) {
        setPixmap(m_frames[0]);
    }
}

void Explosion::setFrameDuration(double seconds)
{
    m_frameDuration = seconds;
}

void Explosion::update(double dt)
{
    if (m_finished || m_frames.isEmpty())
        return;

    m_elapsed += dt;

    while (m_elapsed >= m_frameDuration && !m_finished) {
        m_elapsed -= m_frameDuration;
        ++m_currentFrame;

        if (m_currentFrame >= m_frames.size()) {
            m_finished = true;
            setVisible(false);
        } else {
            setPixmap(m_frames[m_currentFrame]);
        }
    }
}

bool Explosion::isFinished() const
{
    return m_finished;
}
