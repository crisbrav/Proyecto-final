// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gamemanager.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"

#include <QStackedWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_gameManager(new GameManager(this)),
    m_level1(nullptr),
    m_level2(nullptr),
    m_level3(nullptr)
{
    ui->setupUi(this);

    setupLevels();
    setupConnections();
    showMainMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

GameManager *MainWindow::getGameManager() const
{
    return m_gameManager;
}

void MainWindow::setupLevels()
{
    // Aquí asumes que en el .ui tienes un QStackedWidget llamado stackedWidget
    // y una página para el menú principal (index 0).

    m_level1 = new Level1(this);
    m_level2 = new Level2(this);
    m_level3 = new Level3(this);

    ui->stackedWidget->addWidget(m_level1); // index 1
    ui->stackedWidget->addWidget(m_level2); // index 2
    ui->stackedWidget->addWidget(m_level3); // index 3
}

void MainWindow::setupConnections()
{
    // Botones del menú (ajusta los nombres a lo que pongas en Qt Designer)
    connect(ui->btnPlay, &QPushButton::clicked, this, &MainWindow::onPlayClicked);
    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);

    // Señales de niveles
    connect(m_level1, &Level1::levelCompleted, this, &MainWindow::onLevelCompleted);
    connect(m_level1, &Level1::levelFailed, this, &MainWindow::onLevelFailed);

    connect(m_level2, &Level2::levelCompleted, this, &MainWindow::onLevelCompleted);
    connect(m_level2, &Level2::levelFailed, this, &MainWindow::onLevelFailed);

    connect(m_level3, &Level3::levelCompleted, this, &MainWindow::onLevelCompleted);
    connect(m_level3, &Level3::levelFailed, this, &MainWindow::onLevelFailed);
}

void MainWindow::showMainMenu()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::showLevel(int levelIndex)
{
    ui->stackedWidget->setCurrentIndex(levelIndex);
}

void MainWindow::onPlayClicked()
{
    m_gameManager->startLevel(1);
    m_level1->startLevel();
    showLevel(1);
}

void MainWindow::onExitClicked()
{
    close();
}

void MainWindow::onLevelCompleted()
{
    int current = m_gameManager->getCurrentLevel();
    if (current == 1) {
        m_gameManager->startLevel(2);
        m_level2->startLevel();
        showLevel(2);
    } else if (current == 2) {
        m_gameManager->startLevel(3);
        m_level3->startLevel();
        showLevel(3);
    } else if (current == 3) {
        // TODO: mostrar pantalla de victoria final y volver al menú
        qDebug() << "Juego completado!";
        showMainMenu();
    }
}

void MainWindow::onLevelFailed()
{
    // TODO: mostrar mensaje de Game Over y volver al menú o reintentar
    qDebug() << "Nivel fallado";
    showMainMenu();
}
