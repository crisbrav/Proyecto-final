#ifndef BASELEVEL_H
#define BASELEVEL_H

#include <QWidget>

class QGraphicsView;
class QGraphicsScene;
class QTimer;

class BaseLevel : public QWidget
{
    Q_OBJECT

public:
    explicit BaseLevel(QWidget *parent = nullptr);
    virtual ~BaseLevel();

    virtual void startLevel() = 0;
    virtual void stopLevel();

signals:
    void levelCompleted();
    void levelFailed();

protected:
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QTimer *m_mainTimer;

    // dt fijo aproximado (16 ms)
    const double m_fixedDt = 0.016; // ~60 FPS

    void keyPressEvent(QKeyEvent *event) override;

protected slots:
    virtual void updateGame() = 0;
};

#endif // BASELEVEL_H
