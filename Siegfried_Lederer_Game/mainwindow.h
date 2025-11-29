#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameManager;
class Level1;
class Level2;   // ← NUEVO
class Level3;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onPlayClicked();
    void onExitClicked();

    void onLevel1Completed();
    void onLevel1Failed();

    void onLevel2Completed();   // ← NUEVO
    void onLevel2Failed();      // ← NUEVO

    void onLevel3Completed();
    void onLevel3Failed();

private:
    Ui::MainWindow *ui;

    GameManager *m_gameManager;
    Level1 *m_level1;
    Level2 *m_level2;   // ← NUEVO
    Level3 *m_level3;

    void setupLevels();
    void setupConnections();

    void showMainMenu();
    void showLevel1();
    void showLevel2();  // ← NUEVO
    void showLevel3();
};

#endif // MAINWINDOW_H
