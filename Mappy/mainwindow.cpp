#include "mainwindow.h"

#include <QDockWidget>
#include <QLabel>
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_playerWidget(QDockWidget(this))
    , m_setting(QSettings("settings", QSettings::IniFormat))
{
    setCentralWidget(new QLabel("MAP TBA"));

    resize(800,600);

/*    QMenuBar *menubar = menuBar();

    QAction *act = new QAction(tr("New Game"));
    connect(act, &QAction::triggered,
            this, &MainWindow::newGame);
    menubar->addAction(act);

    QMenu *menu = new QMenu(tr("Load Game"));
    ... */

    // FOR NOW
//    loadGameAs("game.txt");

}

MainWindow::~MainWindow()
{
}

/*void MainWindow::closeEvent(QCloseEvent *)
{
    saveGameAs("game.txt");
}

//void MainWindow::newGame()
//void MainWindow::loadGame();
void MainWindow::loadGameAs(QString file)
{

}
//void MainWindow::saveGame();
void MainWindow::saveGameAs(QString file)
{

}*/
