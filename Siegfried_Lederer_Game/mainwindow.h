#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GameManager;
class Level1;
class Level2;
class Level3;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Botones del menú
    void onPlayClicked();    // Nivel 1
    void onLevel2Clicked();  // Nivel 2
    void onLevel3Clicked();  // Nivel 3
    void onExitClicked();

    // Señales de los niveles
    void onLevel1Completed();
    void onLevel1Failed();

    void onLevel2Completed();
    void onLevel2Failed();

    void onLevel3Completed();
    void onLevel3Failed();

private:
    Ui::MainWindow *ui;

    GameManager *m_gameManager;
    Level1 *m_level1;
    Level2 *m_level2;
    Level3 *m_level3;

    void setupLevels();
    void setupConnections();

    void showMainMenu();
    void showLevel1();
    void showLevel2();
    void showLevel3();

    // Muestra un diálogo modal con una imagen y un botón "Volver al menú"
    void showImageDialog(const QString &resourcePath, const QString &title);
};

#endif // MAINWINDOW_H
