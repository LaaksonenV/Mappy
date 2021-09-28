#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>

class QTableWidget;
class Campaign;

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    virtual ~GameWidget();

private:
    void updateStates();

public slots:
    void addPlayer();

    void startTurn();
    void endTurn();

    void doubleClick(int row, int col);

signals:
    void turnGoing(bool);

private:
    Campaign *m_game;
    QTableWidget *m_table;

};

#endif // GAMEWIDGET_H
