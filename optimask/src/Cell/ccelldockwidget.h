#ifndef CCELLDOCKWIDGET_H
#define CCELLDOCKWIDGET_H

#include <QWidget>
#include <QTabWidget>

class CCellDockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CCellDockWidget(QWidget *parent = 0);
    ~CCellDockWidget();

    QTabWidget *  cellTreeTabs;
private:
    void init();

signals:

public slots:
};

#endif // CCELLDOCKWIDGET_H
