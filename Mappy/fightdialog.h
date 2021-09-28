#ifndef FIGHTDIALOG_H
#define FIGHTDIALOG_H

#include <QDialog>

#include <QButtonGroup>
#include <QIcon>

class FightDialog : public QDialog
{
    Q_OBJECT
public:

    enum FightState
    {
        eNull,
        eLoss,
        eFight,
        eWin
    };

    static QIcon iconForState(FightState state) const;

    FightDialog(const QString &name, FightState state);

    const FightState getState() const;

private:
    QButtonGroup m_buttons;
};

#endif // FIGHTDIALOG_H