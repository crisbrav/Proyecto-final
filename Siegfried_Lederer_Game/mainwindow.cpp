#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gamemanager.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
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

void MainWindow::setupLevels()
{
    // Página 0 del stackedWidget es el menú (definido en el .ui)
    m_level1 = new Level1(this);
    m_level2 = new Level2(this);
    m_level3 = new Level3(this);

    ui->stackedWidget->addWidget(m_level1); // índice 1
    ui->stackedWidget->addWidget(m_level2); // índice 2
    ui->stackedWidget->addWidget(m_level3); // índice 3
}

void MainWindow::setupConnections()
{
    // Botones del menú principal (asegúrate de que existan en el .ui)
    connect(ui->btnPlay,   &QPushButton::clicked,
            this,          &MainWindow::onPlayClicked);    // Nivel 1

    connect(ui->btnLevel2, &QPushButton::clicked,
            this,          &MainWindow::onLevel2Clicked);  // Nivel 2

    connect(ui->btnLevel3, &QPushButton::clicked,
            this,          &MainWindow::onLevel3Clicked);  // Nivel 3

    connect(ui->btnExit,   &QPushButton::clicked,
            this,          &MainWindow::onExitClicked);

    // Señales de los niveles
    // Nivel 1
    connect(m_level1, &Level1::levelCompleted,
            this,     &MainWindow::onLevel1Completed);
    connect(m_level1, &Level1::levelFailed,
            this,     &MainWindow::onLevel1Failed);

    // Nivel 2
    connect(m_level2, &Level2::levelCompleted,
            this,     &MainWindow::onLevel2Completed);
    connect(m_level2, &Level2::levelFailed,
            this,     &MainWindow::onLevel2Failed);

    // Nivel 3
    connect(m_level3, &Level3::levelCompleted,
            this,     &MainWindow::onLevel3Completed);
    connect(m_level3, &Level3::levelFailed,
            this,     &MainWindow::onLevel3Failed);
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

// ---------- Botones del menú ----------

void MainWindow::onPlayClicked()
{
    // Botón de Nivel 1
    m_gameManager->startLevel(1);
    m_level1->startLevel();
    showLevel1();
}

void MainWindow::onLevel2Clicked()
{
    m_gameManager->startLevel(2);
    m_level2->startLevel();
    showLevel2();
}

void MainWindow::onLevel3Clicked()
{
    m_gameManager->startLevel(3);
    m_level3->startLevel();
    showLevel3();
}

void MainWindow::onExitClicked()
{
    close();
}

// ---------- Diálogo de imagen ----------

void MainWindow::showImageDialog(const QString &resourcePath, const QString &title)
{
    QDialog dlg(this);
    dlg.setModal(true);
    dlg.setWindowTitle(title);
    dlg.resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QLabel *label = new QLabel(&dlg);
    label->setAlignment(Qt::AlignCenter);

    QPixmap pm(resourcePath);
    if (!pm.isNull()) {
        // Escalamos para llenar la ventana sin deformar demasiado
        label->setPixmap(pm.scaled(dlg.size(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation));
    } else {
        label->setText(tr("No se pudo cargar la imagen:\n%1").arg(resourcePath));
    }

    layout->addWidget(label, 1);

    QPushButton *btnMenu = new QPushButton(tr("Volver al menú"), &dlg);
    btnMenu->setFixedHeight(40);
    layout->addWidget(btnMenu, 0, Qt::AlignHCenter | Qt::AlignBottom);

    connect(btnMenu, &QPushButton::clicked,
            &dlg,    &QDialog::accept);

    dlg.exec();
}

// ---------- Respuesta a señales de los niveles ----------

void MainWindow::onLevel1Completed()
{
    // Imagen de nivel 1 completado
    showImageDialog(":/assets/img_complet_l1.png",
                    tr("Nivel 1 completado"));
    showMainMenu();
}

void MainWindow::onLevel1Failed()
{
    // Imagen de Game Over
    showImageDialog(":/assets/img_gameover.jpg",
                    tr("Game Over"));
    showMainMenu();
}

void MainWindow::onLevel2Completed()
{
    showImageDialog(":/assets/img_complet_l2.png",
                    tr("Nivel 2 completado"));
    showMainMenu();
}

void MainWindow::onLevel2Failed()
{
    showImageDialog(":/assets/img_gameover.jpg",
                    tr("Game Over"));
    showMainMenu();
}

void MainWindow::onLevel3Completed()
{
    showImageDialog(":/assets/img_complet_l3.jpg",
                    tr("Nivel 3 completado"));
    showMainMenu();
}

void MainWindow::onLevel3Failed()
{
    showImageDialog(":/assets/img_gameover.jpg",
                    tr("Game Over"));
    showMainMenu();
}
