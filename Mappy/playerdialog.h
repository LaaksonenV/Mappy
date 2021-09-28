#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QKeyEvent;
class MoveEdit;

class PlayerDialog : public QDialog
{
    Q_OBJECT
public:
    PlayerDialog(const QString &name, const QString &loc,
                 const QStringList &moves, QWidget *parent = nullptr);

    QStringList getMoves() const;
    bool getCamp() const;

public slots:
    void campable(const QString &s);

private:
    MoveEdit *m_move1;
    MoveEdit *m_move2;
    MoveEdit *m_move3;
    QCheckBox *m_camp;
};

class MoveEdit : public QLineEdit
{
    Q_OBJECT
public:
    MoveEdit(QWidget *parent = nullptr);
    MoveEdit(const QString &text, QWidget *parent = nullptr);

private:
    void init();

//protected:
//    void keyPressEvent(QKeyEvent *event);

public slots:
    void onOverflow(const QString &text);

private slots:
//    void showMenu(QPoint position){}
    void onTextChanged(const QString &text);

signals:
    void overflow(const QString &text);

private:

};

#endif // PLAYERDIALOG_H
