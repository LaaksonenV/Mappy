#include "gamewidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QLineEdit>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QInputDialog>

#include "campaign.h"
#include "playerdialog.h"
#include "fightdialog.h"

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , m_game(new Campaign())
    , m_table(new QTableWidget(this))
{
    QGridLayout *lay = new QGridLayout();

    QPushButton *but = new QPushButton(tr("Add Player"));
    connect(but, &QPushButton::released,
            this, &GameWidget::addPlayer);
    lay->addWidget(but, 0,0);


    m_table->setAlternatingRowColors(true);
    QStringList heads("ID");
    heads << tr("Initiative") << tr("Name") << tr("Location") << tr("Camp")
          << tr("Battle") << tr("Moves");
    m_table->setHorizontalHeaderLabels(heads);
    m_table->setColumnHidden(0, true);
    connect(m_table, &QTableWidget::cellDoubleClicked,
            this, &GameWidget::doubleClick);
    lay->addWidget(m_table, 1,0,1,4);

    but = new QPushButton(tr("Start Turn"));
    connect(but, &QPushButton::released,
            this, &GameWidget::startTurn);
    connect(this, &GameWidget::turnGoing,
            but, &QPushButton::setDisabled);
    lay->addWidget(but, 2,2);

    but = new QPushButton(tr("End Turn"));
    connect(but, &QPushButton::released,
            this, &GameWidget::endTurn);
    connect(this, &GameWidget::turnGoing,
            but, &QPushButton::setEnabled);
    but->setDisabled(true);
    lay->addWidget(but, 2,3);

    lay->setColumnStretch(1,1);
    setLayout(lay);
}

GameWidget::~GameWidget()
{
    delete m_game;
}

void GameWidget::updateStates()
{
    int id, state;

    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        id = m_table->itemAt(row, 0)->text().toInt();

        m_table->item(row, 3)->setText(
                    m_game->getPlayerData(id, Campaign::e_Location));

        state = std::stoi(m_game->getPlayerData(id, Campaign::e_Fight));

        m_table->item(row, 5)->setIcon(FightDialog::iconForState(
                        static_cast<FightDialog::FightState>(state)));
        m_table->item(row, 5)->setData(Qt::UserRole, QVariant(state));
    }
}

void GameWidget::addPlayer()
{
    m_table->setSortingEnabled(false);

    int row = m_table->rowCount();
    int id = m_game->addPlayer();

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(id));
    m_table->setItem(row, 0, item);

    item = new QTableWidgetItem("0");
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_table->setItem(row, 1, item);

    item = new QTableWidgetItem(m_game->getPlayerData(id, Campaign::e_Name));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_table->setItem(row, 2, item);

    item = new QTableWidgetItem("OffMap");
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_table->setItem(row, 3, item);

    item = new QTableWidgetItem();
    item->setFlags(Qt::NoItemFlags);
    m_table->setItem(row, 4, item);

    item = new QTableWidgetItem();
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setData(Qt::UserRole, QVariant(FightDialog::eNull));

    m_table->setItem(row, 5, item);

    item = new QTableWidgetItem(tr("Doubleclick to add/edit data"));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_table->setItem(row, 6, item);

    m_table->setSortingEnabled(true);
}

void GameWidget::startTurn()
{
    if (m_game->startTurn())
        emit turnGoing(true);

    updateStates();
}

void GameWidget::endTurn()
{
    if (m_game->endTurn())
        emit turnGoing(false);

    updateStates();
}

void GameWidget::doubleClick(int row, int col)
{
    int id = m_table->itemAt(row, 0)->text().toInt();
    QString name = m_table->itemAt(row, 2)->text();
    QString text = QString();
    QRegExp re("[A-Za-z]+\d+");

    switch (static_cast<Campaign::PlayerData>(col))
    {
    case Campaign::e_Initiative:
        text = QString::number(
                    QInputDialog::getInt(
                        this, name, tr("Initiative"),
                        m_table->itemAt(row, col)->text().toInt()));
        break;
    case Campaign::e_Name:
        text = QInputDialog::getText(
                        this, name, tr("New Name"), QLineEdit::Normal,
                        name);
        break;
    case Campaign::e_Location:
        text = QInputDialog::getText(
                        this, name, tr("Location"), QLineEdit::Normal,
                        m_table->itemAt(row, col)->text());
        if (!re.exactMatch(text))
            text = tr("OffMap");
        break;
    case Campaign::e_Moves:
        text = m_table->itemAt(row, col)->text();
        QStringList moves = text.split("->");
        PlayerDialog dial(name, m_table->itemAt(row, 3)->text(), moves);
        if (dial.exec() == QDialog::Accepted)
        {
            if (dial.getCamp())
                m_table->itemAt(row, 4)->setIcon(QIcon(":/state/camp"));
            else
                m_table->itemAt(row, 4)->setIcon(QIcon());

            moves = dial.getMoves();
            text = moves.join("->");
        }
        break;
    case Campaign::e_Fight:
        int state = m_table->itemAt(row, col)->data(Qt::UserRole).toInt();
        FightDialog::FightState fstate =
                static_cast<FightDialog::FightState>(state);
        if (fstate != FightDialog::eNull)
        {
            FightDialog dial(name, fstate);
            if (dial.exec() == QDialog::Accepted)
            {
                fstate = dial.getState();
                m_table->itemAt(row, col)->
                        setIcon(FightDialog::iconForState(fstate));
                m_game->setPlayerData(id, col,
                                      std::stoi(static_cast<int>(fstate)));
            }
        }


    default:
        return;
    }

    m_table->itemAt(row, col)->setText(text);
    m_game->setPlayerData(id, col, text.toStdString());
}
