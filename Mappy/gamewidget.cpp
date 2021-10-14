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
#include <QTextStream>
#include <QRegularExpression>
#include <QMessageBox>
#include <QKeyEvent>

#include "campaign.h"
#include "playerdialog.h"
#include "fightdialog.h"

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , m_lastId(0)
    , m_game(new Campaign())
    , m_table(new QTableWidget(this))
    , m_startBut(new QPushButton(tr("Start Turn")))
{
    QGridLayout *lay = new QGridLayout();

    QPushButton *but = new QPushButton(tr("Add Player"));
    connect(but, &QPushButton::released,
            this, &GameWidget::addPlayer);
    lay->addWidget(but, 0,0);

    QLabel *lab = new QLabel(tr("Doubleclick or enter to add/edit information."
                                " Save happens on exit."));
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

    headerView->setSectionResizeMode(1, QHeaderView::Fixed);
    headerView->setSectionResizeMode(2, QHeaderView::Stretch);
    headerView->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(4, QHeaderView::Fixed);
    headerView->setSectionResizeMode(5, QHeaderView::Fixed);
    headerView->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    headerView->setSectionsClickable(true);
    headerView->setCascadingSectionResizes(true);

    m_table->sortByColumn(2, Qt::AscendingOrder);

    connect(m_table, &QTableWidget::cellDoubleClicked,
            this, &GameWidget::doubleClick);
    lay->addWidget(m_table, 1,0,1,4);

    but = new QPushButton(tr("Remove player"));
    connect(but, &QPushButton::released,
            this, &GameWidget::removePlayer);
    lay->addWidget(but, 2,0);

    connect(m_startBut, &QPushButton::released,
            this, &GameWidget::startTurn);
    connect(this, &GameWidget::turnGoing,
            m_startBut, &QPushButton::setDisabled);
    lay->addWidget(m_startBut, 2,2);

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

void GameWidget::saveGame(QTextStream &str)
{
    QString text;

    if (m_startBut->isEnabled())
        text = "0";
    else
        text = "1";

    str << "[Game]\n";
    str << "State: " << text << "\n";

    str << "[Players]\n";
    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        str << m_table->item(row, 1)->text() << ";;"; // Initiative
        str << m_table->item(row, 2)->text() << ";;"; // Name
        str << m_table->item(row, 3)->text() << ";;"; // Location
        str << m_table->item(row, 4)->
               data(Qt::UserRole).toString() << ";;"; // Camp
        str << m_table->item(row, 5)->
               data(Qt::UserRole).toString() << ";;"; // Fight
        str << m_table->item(row, 6)->text() << ";;"; // Moves
        str << m_table->item(row, 6)->
               data(Qt::UserRole).toString() << "\n"; // Move block
    }
}

void GameWidget::loadGame(QTextStream &str)
{
    QString text;
    QRegularExpression entry("(.+): (.+)");
    QRegularExpressionMatch caught;
    QStringList playerData;

    while (!str.atEnd())
    {
        if (text == "[Game]")
        {
            text.clear();
            while (!str.atEnd() && !text.startsWith('['))
            {
                text = str.readLine();
                caught = entry.match(text);
                if (caught.captured(1) == "State")
                {
                    if (caught.captured(2) == "1")
                        emit turnGoing(true);
                }
            }
        }
        else if (text == "[Players]")
        {
            text.clear();
            while (!str.atEnd() && !text.startsWith('['))
            {
                text = str.readLine();
                playerData = text.split(";;");
                createPlayer(playerData);
            }
        }
        else text = str.readLine();
    }
}

void GameWidget::keyPressEvent(QKeyEvent *k)
{
    if (k->key() == Qt::Key_Enter
            || k->key() == Qt::Key_Return)
        doubleClick(m_table->currentRow(), m_table->currentColumn());
    else
        QWidget::keyPressEvent(k);
}

void GameWidget::updateStates(bool reset)
{
    int id, oldstate;
    int state;
    QString text;
    QStringList moves;

    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        id = m_table->item(row, 0)->text().toInt();

        text = QString::fromStdString(m_game->getPlayerData(id,
                              static_cast<int>(Campaign::e_Location)));
        if (text.isEmpty())
            text = "OffMap";
        m_table->item(row, 3)->setText(text);

        if (reset)
        {
            setIconData(row, 4, FightDialog::eCamp);
            m_table->item(row, 6)->setText("");
        }
        else
        {
            moves = QString::fromStdString(m_game->getPlayerData(id,
                                           static_cast<int>(Campaign::e_Moves)))
                    .split("->", Qt::SkipEmptyParts);
            for (int i = 0; i < moves.count(); ++i)
            {
                if (moves.at(i) == text)
                {
                    moves[i].append('*');
                    break;
                }
            }
            m_table->item(row, 6)->setText(moves.join("->"));
        }

        oldstate =  m_table->item(row, 5)->data(Qt::UserRole).toInt();
        state = std::stoi(m_game->getPlayerData(id,
                              static_cast<int>(Campaign::e_Fight)));

        if ((oldstate == FightDialog::eWin || oldstate == FightDialog::eLoss)
                && state)

        {
            setIconData(row, 6, FightDialog::eBlock);
        }
        else
        {
            setIconData(row, 6, FightDialog::eNull);
        }

        if ((bool)oldstate != (bool)state)
        {
            if (state)
                state = FightDialog::eFight;
            setIconData(row, 5, state);
        }

    }
}

void GameWidget::createPlayer(const QStringList &list)
{
    if (list.count() < 6)
//ERROR msg!
        return;

    m_table->setSortingEnabled(false);

    int row = m_table->rowCount();
    m_table->setRowCount(row+1);
    int id = m_game->addPlayer();

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(id));
    m_table->setItem(row, 0, item);

    item = new QTableWidgetItem(list.at(0));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    m_game->setPlayerData(id, Campaign::e_Initiative, list.at(0).toStdString());
    m_table->setItem(row, 1, item);

    QString text = list.at(1);
    if (text.isEmpty())
    {
        text = "Player ";
        text += QString::number(id);
    }
    item = new QTableWidgetItem(text);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_game->setPlayerData(id, Campaign::e_Name, list.at(1).toStdString());
    m_table->setItem(row, 2, item);

    item = new QTableWidgetItem(list.at(2));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    m_game->setPlayerData(id, Campaign::e_Location, list.at(2).toStdString());
    m_table->setItem(row, 3, item);

    item = new QTableWidgetItem("");
    item->setFlags(Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    setItemIconData(item, list.at(3).toInt());
    m_table->setItem(row, 4, item);

    item = new QTableWidgetItem("");
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    setItemIconData(item, list.at(4).toInt());
    m_table->setItem(row, 5, item);

    item = new QTableWidgetItem(list.at(5));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    m_table->setItem(row, 6, item);
    if (!list.at(5).isEmpty())
        m_game->setPlayerData(id, Campaign::e_Moves, list.at(5).toStdString());
    if (list.count() > 6)
    {
        setItemIconData(item, list.at(6).toInt());
    }

    if (list.at(4).toInt() == FightDialog::eLoss)
        m_game->setPlayerData(id, Campaign::e_Fight, "-1");

    m_table->setSortingEnabled(true);
}

void GameWidget::setIconData(int row, int col, int state)
{
     QTableWidgetItem *item = m_table->item(row, col);
     if (item)
         setItemIconData(item, state);
}

void GameWidget::setItemIconData(QTableWidgetItem *item,
                                 int state)
{
    item->setIcon(FightDialog::iconForState(
                      static_cast<FightDialog::FightState>(state)));
    item->setData(Qt::UserRole, state);
}

void GameWidget::addPlayer()
{
    QStringList list;
    list << "0" << "" << "OffMap" << QString::number(FightDialog::eNull)
         << QString::number(FightDialog::eNull) << ""
         << QString::number(FightDialog::eNull);
    createPlayer(list);
}

void GameWidget::removePlayer()
{
    int row = m_table->currentRow();
    if (row < 0)
        return;

    QString name = m_table->item(row, 2)->text();

    if (QMessageBox::question(this, tr("Removing player"),
                              tr("Are you sure you want to remove player \n")
                              + name,
                              QMessageBox::StandardButtons(
                                  QMessageBox::Yes|QMessageBox::No),
                              QMessageBox::No) == QMessageBox::Yes)
    {
        m_game->removePlayer(m_table->item(row, 0)->text().toInt());
        m_table->removeRow(row);
    }
}

void GameWidget::startTurn()
{
    bool ok = true;
    bool denied = false;
    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        if (m_table->item(row, 6)->text().isEmpty())
        {
            ok = false;
        }
        if (m_table->item(row, 6)->data((Qt::UserRole)).toInt()
                                        == FightDialog::eDenied)
        {
            denied = true;
        }
    }

    if (denied)
    {
        if (QMessageBox::question(this, tr("Illegal moves"),
                              tr("Some of the players' moves are not "
                                 "continuous. Proceed anyway?"),
                                  QMessageBox::StandardButtons(
                                      QMessageBox::Yes|QMessageBox::No),
                                  QMessageBox::No)
                != QMessageBox::Yes)
        return;
    }


    if (!ok)
    {
        if (QMessageBox::question(this, tr("Moves missing"),
                              tr("Some of the players haven't given moves yet. "
                                 "Proceed anyway?")) == QMessageBox::Yes)
        {
            if (m_game->startTurn())
                emit turnGoing(true);

            updateStates();
        }
    }
    else
    {
        if (m_game->startTurn())
            emit turnGoing(true);

        updateStates();
    }
}

void GameWidget::endTurn()
{
    bool ok = true;
    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        if (m_table->item(row, 5)->data(Qt::UserRole).toInt()
                == FightDialog::eFight)
        {
            ok = false;
            break;
        }
    }

    if (!ok)
    {
        QMessageBox::critical(this, tr("Unable to end turn"),
                              tr("Some of the players still have unresolved"
                                 " fights."));
        return;
    }

    if (m_game->endTurn())
    {
        emit turnGoing(false);
        updateStates(true);
    }
    else
        updateStates(false);
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
            m_game->setPlayerData(id, Campaign::e_Moves, "");
            m_table->item(row, 6)->setText("");
            setIconData(row, 6, FightDialog::eNull);
        }
    }
    else if (type == Campaign::e_Moves)
    {
        text = m_table->item(row, col)->text();
        QStringList moves = text.split("->", Qt::SkipEmptyParts);
        if (!moves.isEmpty())
            moves.removeFirst();

        for (int i = 0; i < moves.count(); ++i)
        {
            if (moves.at(i).endsWith('*'))
            {
                moves[i].chop(1);
                break;
            }
        }
        bool cmp = (bool)m_table->item(row, 4)->data(Qt::UserRole).toInt();
        PlayerDialog dial(name, m_table->item(row, 3)->text(), moves, cmp);
        if (dial.exec() == QDialog::Accepted)
        {
            moves = dial.getMoves();
            moves.prepend(m_table->item(row, 3)->text());
            text = moves.join("->");

 // Need a better representation for which move offends
            if (m_game->checkMoves(text.toStdString()) < 0)
            {
                setIconData(row, col, FightDialog::eNull);
            }
            else
            {
                setIconData(row, col, FightDialog::eDenied);
            }

            if (dial.getCamp())
                setIconData(row, 4, FightDialog::eCamp);
            else
                setIconData(row, 4, FightDialog::eNull);

            m_game->setPlayerData(id, col, text.toStdString());

            m_table->item(row, col)->setText(text);

        }
        return;
    }
    else if (type == Campaign::e_Fight)
    {
        int state = m_table->item(row, col)->data(Qt::UserRole).toInt();
        FightDialog::FightState fstate =
                static_cast<FightDialog::FightState>(state);
        if (fstate != FightDialog::eNull && fstate != FightDialog::eBlock)
        {
            FightDialog dial(name, fstate);
            if (dial.exec() == QDialog::Accepted)
            {
                FightDialog::FightState newfstate = dial.getState();
                if (fstate != newfstate)
                {
                    setIconData(row, col, newfstate);

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
