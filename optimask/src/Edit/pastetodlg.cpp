#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>


#include "./Edit/pastetodlg.h"

PasteToDlg::PasteToDlg(QDialog *parent):QDialog(parent)
{
    setWindowTitle(tr("Paste To"));
    createWidgets();
    createLayout();
    setFixedSize(minimumSizeHint());
}

PasteToDlg::~PasteToDlg()
{

}

void PasteToDlg::createWidgets()
{
    cellLayerSpinBox = new QDoubleSpinBox;
    cellLayerSpinBox->setRange(0.0,100.0);
    cellLayerSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    cellLayerSpinBox->setDecimals(1);

    goalLayerSpinBox = new QDoubleSpinBox;
    goalLayerSpinBox->setRange(0.0,100.0);
    goalLayerSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    goalLayerSpinBox->setDecimals(1);

    xSpinBox = new QDoubleSpinBox;
    xSpinBox->setRange(-1000.0,1000.0);
    xSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    xSpinBox->setDecimals(1);

    ySpinBox = new QDoubleSpinBox;
    ySpinBox->setRange(-1000.0,1000.0);
    ySpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    ySpinBox->setDecimals(1);
}

void PasteToDlg::createLayout()
{
    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("InputLayer"), cellLayerSpinBox);
    layout->addRow(tr("GoalLayer"), goalLayerSpinBox);
    QHBoxLayout *shearLayout = new QHBoxLayout;
    shearLayout->addWidget(xSpinBox);
    shearLayout->addWidget(ySpinBox);
    layout->addRow(tr("XY"), shearLayout);
    setLayout(layout);
}

