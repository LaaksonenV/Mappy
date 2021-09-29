#ifndef FIGHTDIALOG_H
#define FIGHTDIALOG_H

#include <QDialog>

#include <QIcon>

class QButtonGroup;

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

    static QIcon iconForState(FightState state);

    FightDialog(const QString &name, FightState state,
                QWidget *parent = nullptr);

    const FightState getState() const;

private:
    QButtonGroup *m_buttons;
};

#endif // FIGHTDIALOG_H
