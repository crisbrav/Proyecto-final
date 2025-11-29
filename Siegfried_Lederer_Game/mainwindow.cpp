#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gamemanager.h"
#include "level1.h"
#include "level2.h"   // ← NUEVO
#include "level3.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_gameManager(new GameManager(this)),
    m_level1(0),
    m_level2(0),   // ← NUEVO
    m_level3(0)
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

void MainWindow::setupLevels()
{
    // Página 0 del stackedWidget debe ser el menú
    m_level1 = new Level1(this);
    m_level2 = new Level2(this);   // ← NUEVO
    m_level3 = new Level3(this);

    ui->stackedWidget->addWidget(m_level1); // índice 1
    ui->stackedWidget->addWidget(m_level2); // índice 2 ← NUEVO
    ui->stackedWidget->addWidget(m_level3); // índice 3
}

void MainWindow::setupConnections()
{
    // Botones del menú
    connect(ui->btnPlay, &QPushButton::clicked,
            this, &MainWindow::onPlayClicked);

    connect(ui->btnExit, &QPushButton::clicked,
            this, &MainWindow::onExitClicked);

    // Nivel 1
    connect(m_level1, &Level1::levelCompleted,
            this, &MainWindow::onLevel1Completed);

    connect(m_level1, &Level1::levelFailed,
            this, &MainWindow::onLevel1Failed);

    // Nivel 2  ← NUEVO
    connect(m_level2, &Level2::levelCompleted,
            this, &MainWindow::onLevel2Completed);

    connect(m_level2, &Level2::levelFailed,
            this, &MainWindow::onLevel2Failed);

    // Nivel 3
    connect(m_level3, &Level3::levelCompleted,
            this, &MainWindow::onLevel3Completed);

    connect(m_level3, &Level3::levelFailed,
            this, &MainWindow::onLevel3Failed);
}

void MainWindow::showMainMenu()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::showLevel1()
{
    ui->stackedWidget->setCurrentWidget(m_level1);
}

void MainWindow::showLevel2()
{
    ui->stackedWidget->setCurrentWidget(m_level2);
}

void MainWindow::showLevel3()
{
    ui->stackedWidget->setCurrentWidget(m_level3);
}

void MainWindow::onPlayClicked()
{
    // Iniciamos desde el nivel 1
    m_gameManager->startLevel(1);
    m_level1->startLevel();
    showLevel1();
}

void MainWindow::onExitClicked()
{
    close();
}

void MainWindow::onLevel1Completed()
{
    // Pasar del nivel 1 al nivel 2
    QMessageBox::information(this, tr("Nivel 1 completado"),
                             tr("Has escapado del laberinto."));
    m_gameManager->startLevel(2);  // ← ahora arrancamos el nivel 2 en el GameManager
    m_level2->startLevel();
    showLevel2();
}

void MainWindow::onLevel1Failed()
{
    QMessageBox::warning(this, tr("Has sido atrapado"),
                         tr("El guardia o la trampa te atraparon."));
    showMainMenu();
}

void MainWindow::onLevel2Completed()
{
    // Pasar del nivel 2 al nivel 3
    QMessageBox::information(this, tr("Nivel 2 completado"),
                             tr("Has esquivado el fuego del tanque."));
    m_gameManager->startLevel(3);
    m_level3->startLevel();
    showLevel3();
}

void MainWindow::onLevel2Failed()
{
    QMessageBox::warning(this, tr("Has sido alcanzado"),
                         tr("El tanque o los obstáculos te han detenido."));
    showMainMenu();
}

void MainWindow::onLevel3Completed()
{
    QMessageBox::information(this, tr("Nivel 3 completado"),
                             tr("¡Has sobrevivido al bombardeo!"));
    showMainMenu();
}

void MainWindow::onLevel3Failed()
{
    QMessageBox::warning(this, tr("Game Over"),
                         tr("Has perdido todas las vidas."));
    showMainMenu();
}
