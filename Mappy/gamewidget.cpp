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
#include <QLabel>
#include <QHeaderView>

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

    QLabel *lab = new QLabel(tr("Doubleclick to add/edit information"));
    lay->addWidget(lab, 0,1,1,3);

    m_table->setColumnCount(7);
    m_table->setAlternatingRowColors(true);

    QHeaderView *headerView = new QHeaderView(Qt::Horizontal, m_table);
    m_table->setHorizontalHeader(headerView);

    QStringList heads("ID");
    heads << tr("Init.") << tr("Name") << tr("Location") << tr("Camp")
          << tr("Battle") << tr("Moves");
    m_table->setHorizontalHeaderLabels(heads);
    m_table->setColumnHidden(0, true);
    m_table->setIconSize(QSize(25,25));
    m_table->setColumnWidth(1,40);
    m_table->setColumnWidth(4,40);
    m_table->setColumnWidth(5,40);

    headerView->setSectionResizeMode(2, QHeaderView::Stretch);
    headerView->setSectionsClickable(true);

    m_table->sortByColumn(2, Qt::AscendingOrder);

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
    int id, oldstate;
    int state;
    QString text;

    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        id = m_table->item(row, 0)->text().toInt();

        text = QString::fromStdString(m_game->getPlayerData(id,
                              static_cast<int>(Campaign::e_Location)));
        if (text.isEmpty())
            text = "OffMap";
        m_table->item(row, 3)->setText(text);

        oldstate =  m_table->item(row, 5)->data(Qt::UserRole).toInt();
        state = std::stoi(m_game->getPlayerData(id,
                              static_cast<int>(Campaign::e_Fight)));

        if ((bool)oldstate != (bool)state)
        {
            if (state)
                state = static_cast<int>(FightDialog::eFight);
            m_table->item(row, 5)->setIcon(FightDialog::iconForState(
                            static_cast<FightDialog::FightState>(state)));
            m_table->item(row, 5)->setData(Qt::UserRole, QVariant(state));
        }

    }
}

void GameWidget::addPlayer()
{
    m_table->setSortingEnabled(false);

    int row = m_table->rowCount();
    m_table->setRowCount(row+1);
    int id = m_game->addPlayer();

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(id));
    m_table->setItem(row, 0, item);

    item = new QTableWidgetItem("0");
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    m_table->setItem(row, 1, item);

    item = new QTableWidgetItem(QString::fromStdString(
                m_game->getPlayerData(id, static_cast<int>(Campaign::e_Name))
                                    ));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_table->setItem(row, 2, item);

    item = new QTableWidgetItem("OffMap");
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    m_table->setItem(row, 3, item);

    item = new QTableWidgetItem();
    item->setFlags(Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setData(Qt::UserRole, QVariant(0));

    m_table->setItem(row, 4, item);

    item = new QTableWidgetItem("");
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setData(Qt::UserRole, QVariant(FightDialog::eNull));

    m_table->setItem(row, 5, item);

    item = new QTableWidgetItem(tr(""));
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

    int id;
    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        id = m_table->item(row, 0)->text().toInt();

        m_table->item(row, 4)->setIcon(QIcon());
        m_table->item(row, 6)->setText("");
    }
}

void GameWidget::doubleClick(int row, int col)
{
    int id = m_table->item(row, 0)->text().toInt();
    QString name = m_table->item(row, 2)->text();
    QString text = QString();
    QRegExp re("[A-Za-z]+\\d+");

    Campaign::PlayerData type = static_cast<Campaign::PlayerData>(col);

    if (type == Campaign::e_Initiative)
    {
        text = QString::number(
                    QInputDialog::getInt(
                        this, name, tr("Initiative"),
                        m_table->item(row, col)->text().toInt()));
    }
    else if (type == Campaign::e_Name)
    {
        text = QInputDialog::getText(
                        this, name, tr("New Name"), QLineEdit::Normal,
                        name);
    }
    else if (type == Campaign::e_Location)
    {
        QString oldpos = m_table->item(row, col)->text();
        text = QInputDialog::getText(
                        this, name, tr("Location"), QLineEdit::Normal,
                        oldpos);
        if (!re.exactMatch(text))
            text = tr("OffMap");
        else
            text = text.toUpper();
        if (text != oldpos)
        {
            m_game->setPlayerData(id, (int)Campaign::e_Moves, "");
            m_table->item(row, 5)->setText("");
        }
    }
    else if (type == Campaign::e_Moves)
    {
        text = m_table->item(row, col)->text();
        QStringList moves = text.split("->", Qt::SkipEmptyParts);
        moves.removeFirst();
        bool cmp = m_table->item(row, 4)->data(Qt::UserRole).toBool();
        PlayerDialog dial(name, m_table->item(row, 3)->text(), moves, cmp);
        if (dial.exec() == QDialog::Accepted)
        {
            if (dial.getCamp())
            {
                m_table->item(row, 4)->setIcon(QIcon(":/state/camp"));
                m_table->item(row, 4)->setData(Qt::UserRole, QVariant(true));
            }
            else
            {
                m_table->item(row, 4)->setIcon(QIcon());
                m_table->item(row, 4)->setData(Qt::UserRole, QVariant(false));

            }

            moves = dial.getMoves();
            text = moves.join("->");

            m_game->setPlayerData(id, col, text.toStdString());

            moves.prepend(m_table->item(row, 3)->text());
            text = moves.join("->");

            m_table->item(row, col)->setText(text);

        }
    }
    else if (type == Campaign::e_Fight)
    {
        int state = m_table->item(row, col)->data(Qt::UserRole).toInt();
        FightDialog::FightState fstate =
                static_cast<FightDialog::FightState>(state);
        if (fstate != FightDialog::eNull)
        {
            FightDialog dial(name, fstate);
            if (dial.exec() == QDialog::Accepted)
            {
                FightDialog::FightState newfstate = dial.getState();
                if (fstate != newfstate)
                {
                    m_table->item(row, col)->
                            setIcon(FightDialog::iconForState(newfstate));
                    m_table->item(row, col)->setData(Qt::UserRole,
                                                     QVariant(newfstate));
                    if (fstate == FightDialog::eLoss ||
                            newfstate == FightDialog::eLoss)
                        m_game->setPlayerData(id, col, "-1");
                }
            }
        }
        return;
    }
    else return;

    m_table->item(row, col)->setText(text);
    m_game->setPlayerData(id, col, text.toStdString());
}
