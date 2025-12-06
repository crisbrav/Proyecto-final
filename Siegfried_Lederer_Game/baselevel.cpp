#include "baselevel.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QVBoxLayout>
#include <QKeyEvent>

BaseLevel::BaseLevel(QWidget *parent)
    : QWidget(parent),
    m_view(new QGraphicsView(this)),
    m_scene(new QGraphicsScene(this)),
    m_mainTimer(new QTimer(this))
{
    m_view->setScene(m_scene);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);
    setLayout(layout);

    setFocusPolicy(Qt::StrongFocus);
    m_view->setFocusPolicy(Qt::NoFocus);

    connect(m_mainTimer, &QTimer::timeout, this, [this]() {
        updateGame();
    });
}

BaseLevel::~BaseLevel()
{
}

void BaseLevel::stopLevel()
{
    m_mainTimer->stop();
}

void BaseLevel::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
}
