#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gamemanager.h"
#include "level3.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_gameManager(new GameManager(this)),
    m_level3(0)
{
    ui->setupUi(this);

    setupLevel3();
    setupConnections();
    showMainMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupLevel3()
{
    // Crea el widget del nivel 3 y lo agrega al stackedWidget
    m_level3 = new Level3(this);

    // Se asume que la página 0 del stackedWidget es el menú principal
    ui->stackedWidget->addWidget(m_level3);   // será, por ejemplo, el índice 1
}

void MainWindow::setupConnections()
{
    // Botones del menú (ajusta los nombres si son distintos en tu .ui)
    connect(ui->btnPlay, &QPushButton::clicked,
            this, &MainWindow::onPlayClicked);

    connect(ui->btnExit, &QPushButton::clicked,
            this, &MainWindow::onExitClicked);

    // Señales del nivel 3
    if (m_level3) {
        connect(m_level3, &Level3::levelCompleted,
                this, &MainWindow::onLevel3Completed);

        connect(m_level3, &Level3::levelFailed,
                this, &MainWindow::onLevel3Failed);
    }
}

void MainWindow::showMainMenu()
{
    // Se asume que la página 0 del stackedWidget es el menú
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::showLevel3()
{
    if (!m_level3)
        return;

    ui->stackedWidget->setCurrentWidget(m_level3);
}

void MainWindow::onPlayClicked()
{
    // Indica al GameManager que estás en el nivel 3 (si lo quieres usar)
    m_gameManager->startLevel(3);

    // Inicia la lógica del nivel 3
    if (m_level3) {
        m_level3->startLevel();
    }

    showLevel3();
}

void MainWindow::onExitClicked()
{
    close();
}

void MainWindow::onLevel3Completed()
{
    QMessageBox::information(this, tr("Nivel completado"),
                             tr("¡Has sobrevivido al bombardeo!"));
    showMainMenu();
}

void MainWindow::onLevel3Failed()
{
    QMessageBox::warning(this, tr("Game Over"),
                         tr("Has perdido todas las vidas."));
    showMainMenu();
}
