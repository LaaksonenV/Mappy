#include "playerdialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QRegExpValidator>
#include <QStyle>
#include <QKeyEvent>
#include <QLocale>

PlayerDialog::PlayerDialog(const QString &name, const QString &loc,
                           const QStringList &moves)
    : m_move1(new MoveEdit(this))
    , m_move2(new MoveEdit(this))
    , m_move3(new MoveEdit(this))
    , m_camp(new QCheckBox(this))
{
    setWindowTitle(name);

    QGridLayout *lay = new QGridLayout(this);

    QIcon icon(this->style()->standardIcon("SP_ArrowForward"));
    QLabel forw(this);
    forw.setPixmap(icon.pixmap(10));

    lay->addWidget(new QLabel(loc), 0, 0);
    lay->addWidget(new QLabel(forw), 0, 1);
    lay->addWidget(m_move1, 0, 2);
    lay->addWidget(new QLabel(forw), 0, 3);
    lay->addWidget(m_move2, 0, 4);
    lay->addWidget(new QLabel(forw), 0, 5);
    lay->addWidget(m_move3, 0, 6);

    m_camp->setIcon(QIcon(":/state/camp"));
    lay->addWidget(m_camp, 0, 6);

    m_move2->setDisabled(true);
    m_move3->setDisabled(true);

    connect(m_move1, &MoveEdit::overflow,
            m_move2, &MoveEdit::onOverflow);
    connect(m_move2, &MoveEdit::overflow,
            m_move3, &MoveEdit::onOverflow);
    connect(m_move2, &MoveEdit::overflow,
            this, &PlayerDialog::campable);

    if (moves.count() > 0)
        m_move1->setText(moves.at(0));
    if (moves.count() > 1)
        m_move1->setText(moves.at(1));
    if (moves.count() > 2)
        m_move1->setText(moves.at(2));
}

QStringList PlayerDialog::getMoves() const
{
    QStringList ret;
    if (!m_move1->text().isEmpty())
    {
        ret << m_move1->text();
        if (!m_move2->text().isEmpty())
        {
            ret << m_move2->text();
            if (!m_move3->text().isEmpty())
            {
                ret << m_move3->text();
            }
        }
    }
    return ret;
}

bool PlayerDialog::getCamp() const
{
    return m_camp->isChecked();
}

void PlayerDialog::campable(const QString &s)
{
    if (s.isNull())
    {
        m_camp->setChecked(false);
        m_camp->setDisabled(true);
    }
    else
    {
        m_camp->setChecked(true);
        m_camp->setDisabled(false);
    }
}

MoveEdit::MoveEdit(QWidget *parent)
    : QLineEdit(parent)
{
    init();
}

MoveEdit::MoveEdit(const QString &text, QWidget *parent)
    : QLineEdit(text, parent)
{
    init();
}

void MoveEdit::init()
{

//    setAcceptDrops(false);
//    setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
//            SLOT(showMenu(QPoint)));
    connect(this, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
}

/*void MoveEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Copy) ||
            event->matches(QKeySequence::Cut) ||
            event->matches(QKeySequence::Paste))
        event->ignore();

    else return QLineEdit::keyPressEvent(event);
}*/

void MoveEdit::onOverflow(const QString &text)
{
    if (text.isNull())
    {
        setDisabled(true);
        setText("");
        emit overflow(text);
    }
    else
    {
        setEnabled(true);
        setFocus();
        onTextChanged(text);
    }

}

void MoveEdit::onTextChanged(const QString &text)
{
    if (text.isEmpty())
    {
        emit overflow(QString());
        return;
    }
    QRegExp val("([A-Za-z]+\d+)");
    int index = val.indexIn(text);
    QString temp = QString();
    if (index >= 0)
    {
        temp = text;
        setText(QLocale().toUpper(val.cap(1));
        temp.remove(0, index+val.cap(1).count());
    }
    else if (QRegExp("[A-Za-z]+").indexIn(text) < 0)
    {
        setText("");
    }
    emit overflow(temp);
}
