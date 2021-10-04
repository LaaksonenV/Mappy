#include "mainwindow.h"

#include <QDockWidget>
#include <QLabel>
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "gamewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_playerWidget(new GameWidget(this))
{
    setCentralWidget(m_playerWidget);

    setWindowTitle("Mappy");
    resize(800,600);

/*    QMenuBar *menubar = menuBar();

    QAction *act = new QAction(tr("New Game"));
    connect(act, &QAction::triggered,
            this, &MainWindow::newGame);
    menubar->addAction(act);

    QMenu *menu = new QMenu(tr("Load Game"));
    ... */

    // FOR NOW
    loadGameAs("game.txt");

}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (QMessageBox::question(this, tr("Exiting"), tr("Save changes?"))
            == QMessageBox::Yes)
        saveGameAs("game.txt");
}

//void MainWindow::newGame()
//void MainWindow::loadGame();
void MainWindow::loadGameAs(QString file)
{
    QFile fl(file);
    if (fl.open(QFile::ReadOnly))
    {
        QTextStream str(&fl);
        m_playerWidget->loadGame(str);
        fl.close();
    }
}
//void MainWindow::saveGame();
void MainWindow::saveGameAs(QString file)
{
    QFile fl(file);
    fl.copy(file + ".bcup");
    if (fl.open(QFile::WriteOnly))
    {
        QTextStream str(&fl);
        m_playerWidget->saveGame(str);
        fl.close();
    }
}
