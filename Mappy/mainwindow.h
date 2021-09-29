#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QDockWidget;
class QCloseEvent;

class GameWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
//    void closeEvent(QCloseEvent *);

public slots:
//    void newGame();
//    void loadGame();
//    void loadGameAs(QString file);
//    void saveGame();
//    void saveGameAs(QString file);

private:
    GameWidget *m_playerWidget;

};
#endif // MAINWINDOW_H
