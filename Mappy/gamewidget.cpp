#include "gamewidget.h"

#include "campaign.h"

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , m_game(new Campaign())
{

}

GameWidget::~GameWidget()
{
    delete m_game;
}
