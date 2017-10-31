#ifndef REGENERATEWIDGET_H
#define REGENERATEWIDGET_H

#include <QWidget>
#include<QComboBox>

class RegenerateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegenerateWidget(QWidget *parent = 0);
    ~RegenerateWidget();

private:
    void createWidgets();
    void createLayout();

    QComboBox *groupComboBox;
    QComboBox *ungroupComboBox;
    QComboBox *duplicateComboBox;
    QComboBox *splitComboBox;
    QComboBox *cropComboBox;
    QComboBox *mergeComboBox;
    QComboBox *fractComboBox;
signals:   

public slots:
};

#endif // REGENERATEWIDGET_H
