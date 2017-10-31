#ifndef HINT_H
#define HINT_H

#include <QWidget>
#include <QTextEdit>

class Hint : public QWidget
{
    Q_OBJECT
public:
    Hint(QWidget *parent = 0);
    ~Hint();

    QTextEdit *hintTEdit;


signals:

public slots:
};

#endif // HINT_H
