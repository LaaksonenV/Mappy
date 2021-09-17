#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>

class Campaign;

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    virtual ~GameWidget();


public slots:
    void addPlayer();

    void startTurn();
    void endTurn();

signals:

private:
    Campaign *m_game;
};

#endif // GAMEWIDGET_H
