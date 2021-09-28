#include "fightdialog.h"

#include <QRadioButton>
#include <QBoxLayout>

QIcon FightDialog::iconForState(FightState state) const
{
    QString iconName;
    if (state == eLoss)
        iconName = ":/battle/lose";
    else if (state == eFight)
        iconName = ":/battle/fight";
    else if (state == eWin)
        iconName = ":/battle/win";
    else
        return QIcon();

    return QIcon(iconName);
}

FightDialog::FightDialog(const QString &name, FightState state)
    : m_buttons(QButtonGroup())
{
    setWindowTitle(name);

    QBoxLayout *mainlay = new QBoxLayout(QBoxLayout::TopToBottom);

    QBoxLayout *lay = new QBoxLayout(QBoxLayout::LeftToRight);

    QRadioButton *but = new QRadioButton();
    but->setIcon(iconForState(eLoss));
    m_buttons.addButton(but, 0);
    if (state == eLoss)
        but->setChecked(true);
    lay->addWidget(but);

    but = new QRadioButton();
    but->setIcon(iconForState(eFight));
    m_buttons.addButton(but, 1);
    if (state == eFight)
        but->setChecked(true);
    lay->addWidget(but);

    but = new QRadioButton();
    but->setIcon(iconForState(eWin));
    m_buttons.addButton(but, 2);
    if (state == eWin)
        but->setChecked(true);
    lay->addWidget(but);

    m_buttons.setExclusive(true);

    mainlay->addLayout(lay);

    lay = new QBoxLayout(QBoxLayout::RightToLeft);

    but = new QPushButton(tr("Ok"));
    connect(but, &QPushButton::released,
            this, &QDialog::accept);
    lay->addWidget(but);

    but = new QPushButton(tr("Cancel"));
        connect(but, &QPushButton::released,
                this, &QDialog::reject);
    lay->addWidget(but);

    mainlay->addLayout(lay);
    setLayout(mainlay);
}

const FightDialog::FightState FightDialog::getState() const
{
    return static_cast<FightState>(m_buttons.checkedId());
}
