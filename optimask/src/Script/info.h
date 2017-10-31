#ifndef INFO_H
#define INFO_H

#include <QWidget>
#include <QTextEdit>

class Info : public QWidget
{
    Q_OBJECT
public:
    Info(QWidget *parent = 0);
    ~Info();
    QTextEdit *infoTEdit;

signals:

public slots:
};

#endif // INFO_H
