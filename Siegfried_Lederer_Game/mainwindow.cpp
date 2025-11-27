#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gamemanager.h"
#include "level1.h"
#include "level3.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_gameManager(new GameManager(this)),
    m_level1(0),
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
    m_level3 = new Level3(this);

    ui->stackedWidget->addWidget(m_level1); // índice 1
    ui->stackedWidget->addWidget(m_level3); // índice 2
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
    // Pasar al nivel 3
    QMessageBox::information(this, tr("Nivel 1 completado"),
                             tr("Has escapado del laberinto."));
    m_gameManager->startLevel(3);
    m_level3->startLevel();
    showLevel3();
}

void MainWindow::onLevel1Failed()
{
    QMessageBox::warning(this, tr("Has sido atrapado"),
                         tr("El guardia o la trampa te atraparon."));
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
