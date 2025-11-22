#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameManager;
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

    void onLevel3Completed();
    void onLevel3Failed();

private:
    Ui::MainWindow *ui;

    GameManager *m_gameManager;
    Level3 *m_level3;

    void setupLevel3();
    void setupConnections();

    void showMainMenu();
    void showLevel3();
};

#endif // MAINWINDOW_H
