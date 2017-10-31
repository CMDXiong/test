#include "fuzzyquery.h"

#include <QKeyEvent>
#include <QListView>
#include <QStringListModel>
FuzzyQuery::FuzzyQuery(QStringList words, QWidget *parent)
    : QTextEdit(parent), words(words)
{
    listView     = new QListView(this);
    model       = new QStringListModel(this);
    listView->setWindowFlags(Qt::ToolTip);
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
    connect(this, SIGNAL(textChangedInf(const QString &)), this, SLOT(setCompleter(const QString &)));
    connect(listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(completeText(const QModelIndex &)));
}

void FuzzyQuery::keyPressEvent(QKeyEvent *e)
{
    if (!listView->isHidden())
    {
        int key    = e->key();
        int count  = listView->model()->rowCount();
        QModelIndex currentIndex = listView->currentIndex();

        // When you press the down key, move the cursor to select the item in the next completion list
        if (Qt::Key_Down == key)
        {
            int row = currentIndex.row() + 1;
            if (row >= count)
            {
                row = 0;
            }
            QModelIndex index = listView->model()->index(row, 0);
            listView->setCurrentIndex(index);
        }

        // When you press the up key, move the cursor to select the item in the previous completion list
        else if (Qt::Key_Up == key)
        {
            int row = currentIndex.row() - 1;
            if (row < 0)
            {
                row  = count - 1;
            }

            QModelIndex index = listView->model()->index(row, 0);
            listView->setCurrentIndex(index);
        }

        // When you press the Esc key, hide the completion list
        else if (Qt::Key_Escape == key)
        {
            listView->hide();
        }

        // When you press the Enter key, use the selected item in the completion list and hide the completion list
        else if (Qt::Key_Enter == key || Qt::Key_Return == key)
        {
            if (currentIndex.isValid())
            {
                QString text = listView->currentIndex().data().toString();
                setText(text);
            }
            this->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
            listView->hide();
        }

        // In other cases, hide the completion list and use the QTextEdit keyboard to press the event
        else
        {
            listView->hide();
            QTextEdit::keyPressEvent(e);
        }
    }
    else
    {
        QTextEdit::keyPressEvent(e);
    }
}

void FuzzyQuery::slotTextChanged()
{
        str      = this->toPlainText();
        str2     = str;

        for( i = 0;!str2.isEmpty();i++)
        {
            str1 = str.section("\n",i,i);
            str2 = str.section("\n",i+1,i+1);
        }
        emit textChangedInf(str1);
}

void FuzzyQuery::setCompleter(const QString &text)
{
    if (text.isEmpty())
    {
        listView->hide();
        return;
    }

    // If a word in the complete completion list contains the entered text, it is added to the completion list string to be displayed
    QStringList sl;
    foreach(QString word, words)
    {
        if (word.startsWith(text,Qt::CaseInsensitive))
        {
            sl << word;
        }
        else
        {
            listView->hide();
        }
    }
    model->setStringList(sl);
    listView->setModel(model);
    if (model->rowCount() == 0)
    {
        return;
    }
    // Position the text edit
    listView->setMinimumWidth(150);
    listView->setMaximumWidth(150);
    QPoint p(0, height());
    int x = mapToGlobal(p).x();
    int y = mapToGlobal(p).y() + 1;
    listView->move(x,y);
    listView->show();
}

void FuzzyQuery::completeText(const QModelIndex &index)
{
    QString text = index.data().toString();
    QString textStr1;
    if(i == 1)
    {
     setText(text);
    }
    else
    {
        textStr1  = str.section("\n",0,i-2) + "\n" + text;
        setText(textStr1);
    }
    this->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
    listView->hide();
}
