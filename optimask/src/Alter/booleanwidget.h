#ifndef BOOLEANWIDGET_H
#define BOOLEANWIDGET_H

#include <QWidget>
#include<QComboBox>

class BooleanWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BooleanWidget(QWidget *parent = 0);
    ~BooleanWidget();
private:
    void createWidgets();
    void createLayout();

    QComboBox *AndComboBox;
    QComboBox *OrComboBox;
    QComboBox *XorComboBox;
    QComboBox *A_BComboBox;
    QComboBox *B_AComboBox;

signals:

public slots:
};

#endif // BOOLEANWIDGET_H
