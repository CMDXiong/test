#include "booleanwidget.h"
#include <QFormLayout>

BooleanWidget::BooleanWidget(QWidget *parent) : QWidget(parent)
{
    createWidgets();
    createLayout();
}

BooleanWidget::~BooleanWidget()
{

}

void BooleanWidget::createWidgets()
{
    AndComboBox = new QComboBox();
    OrComboBox  = new QComboBox();
    XorComboBox = new QComboBox();
    A_BComboBox = new QComboBox();
    B_AComboBox = new QComboBox();
}

void BooleanWidget::createLayout()
{
    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("And"), AndComboBox);
    layout->addRow(tr("Or"), OrComboBox);
    layout->addRow(tr("Xor"), XorComboBox);
    layout->addRow(tr("A-B"), A_BComboBox);
    layout->addRow(tr("B-A"), B_AComboBox);
    setLayout(layout);
}

