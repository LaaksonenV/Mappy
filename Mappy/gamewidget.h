#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>

class QTableWidget;
class QTableWidgetItem;
class Campaign;
class QPushButton;
class QKeyEvent;

#include <QTextStream>

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    virtual ~GameWidget();

    void saveGame(QTextStream &str);
    void loadGame(QTextStream &str);

protected:
    virtual void keyPressEvent(QKeyEvent *k);

private:
    void updateStates(bool reset = false);
    void createPlayer(const QStringList &list);
    void setIconData(int row, int col, int state);
    void setItemIconData(QTableWidgetItem *item, int state);

public slots:
    void addPlayer();
    void removePlayer();

    void startTurn();
    void endTurn();

    void doubleClick(int row, int col);

signals:
    void turnGoing(bool);

private:
    int m_lastId;
    Campaign *m_game;
    QTableWidget *m_table;
    QPushButton *m_startBut;
};

#endif // GAMEWIDGET_H
