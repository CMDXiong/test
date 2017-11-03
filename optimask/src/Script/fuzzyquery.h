#ifndef FUZZYQUERY_H
#define FUZZYQUERY_H

#include <QTextEdit>
#include <QStringList>

class QListView;
class QStringListModel;
class QModelIndex;

class FuzzyQuery : public QTextEdit
{
    Q_OBJECT
public:
    FuzzyQuery(QStringList words, QWidget *parent = 0);
//    void  keyPressEvent(QKeyEvent  *event);

public slots:
    void setCompleter(const QString &text); // Dynamic display completion list
    void completeText(const QModelIndex &index); // Click the item in the finish list to use this item to automatically complete the entered word
    void slotTextChanged();

private:
    QStringList words; // The entire list of finished words
    QListView *listView; //The completed the list
    QStringListModel *model; // The model of this completed the list
    QString str,str1,str2;
    int i;

protected:
    virtual void keyPressEvent(QKeyEvent *e);

signals:
    void  textChangedInf(const QString& string);
};

#endif // FUZZYQUERY_H
