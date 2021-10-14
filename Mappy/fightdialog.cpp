#include "fightdialog.h"

#include <QRadioButton>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QStyle>

QIcon FightDialog::iconForState(FightState state)
{
    QString iconName;
    if (state == eLoss)
        iconName = ":/battle/lose";
    else if (state == eFight)
        iconName = ":/battle/fight";
    else if (state == eWin)
        iconName = ":/battle/win";
    else if (state == eBlock)
        iconName = ":/state/block";
    else if (state == eCamp)
        iconName = ":/state/camp";
    else if (state == eDenied)
        return QWidget().style()->standardIcon(QStyle::SP_BrowserStop);

    else
        return QIcon();

    return QIcon(iconName);
}

FightDialog::FightDialog(const QString &name, FightState state,
                         QWidget *parent)
    : QDialog(parent)
    , m_buttons(new QButtonGroup(this))
{
    setWindowTitle(name);

    QBoxLayout *mainlay = new QBoxLayout(QBoxLayout::TopToBottom);

    QBoxLayout *lay = new QBoxLayout(QBoxLayout::LeftToRight);

    QRadioButton *but = new QRadioButton();
    but->setIcon(iconForState(eLoss));
    but->setIconSize(QSize(50,50));
    m_buttons->addButton(but, 0);
    if (state == eLoss)
        but->setChecked(true);
    lay->addWidget(but);

    but = new QRadioButton();
    but->setIcon(iconForState(eFight));
    but->setIconSize(QSize(50,50));
    m_buttons->addButton(but, 1);
    if (state == eFight)
        but->setChecked(true);
    lay->addWidget(but);


    but = new QRadioButton();
    but->setIcon(iconForState(eWin));
    but->setIconSize(QSize(50,50));
    m_buttons->addButton(but, 2);
    if (state == eWin)
        but->setChecked(true);
    lay->addWidget(but);

    m_buttons->setExclusive(true);

    mainlay->addLayout(lay);

    lay = new QBoxLayout(QBoxLayout::RightToLeft);

    QPushButton *pbut = new QPushButton(tr("Ok"));
    connect(pbut, &QPushButton::released,
            this, &QDialog::accept);
    lay->addWidget(pbut);

    pbut = new QPushButton(tr("Cancel"));
        connect(pbut, &QPushButton::released,
                this, &QDialog::reject);
    lay->addWidget(pbut);

    mainlay->addLayout(lay);
    setLayout(mainlay);
}

const FightDialog::FightState FightDialog::getState() const
{
    return static_cast<FightState>(m_buttons->checkedId()+1);
}
