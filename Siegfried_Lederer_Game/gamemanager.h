#include <QObject>

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QObject *parent = nullptr);

    void startLevel(int level);
    int getCurrentLevel() const;

    void setLives(int lives);
    int getLives() const;

signals:
    void livesChanged(int lives);
    void levelChanged(int level);

private:
    int m_currentLevel;
    int m_lives;
};
