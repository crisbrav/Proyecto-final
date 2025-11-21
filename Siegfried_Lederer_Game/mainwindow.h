#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

class GameManager;
class Level1;
class Level2;
class Level3;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    GameManager *getGameManager() const;

private slots:
    void onPlayClicked();
    void onExitClicked();

    void onLevelCompleted();
    void onLevelFailed();

private:
    Ui::MainWindow *ui;

    GameManager *m_gameManager;

    Level1 *m_level1;
    Level2 *m_level2;
    Level3 *m_level3;

    void setupConnections();
    void setupLevels();
    void showMainMenu();
    void showLevel(int levelIndex);
};


#endif // MAINWINDOW_H
