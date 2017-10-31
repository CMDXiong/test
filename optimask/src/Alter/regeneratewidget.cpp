#include "regeneratewidget.h"
#include <QFormLayout>
RegenerateWidget::RegenerateWidget(QWidget *parent) : QWidget(parent)
{
    createWidgets();
    createLayout();
}

RegenerateWidget::~RegenerateWidget()
{

}

void RegenerateWidget::createWidgets()
{
    groupComboBox     = new QComboBox();
    ungroupComboBox   = new QComboBox();
    duplicateComboBox = new QComboBox();
    splitComboBox     = new QComboBox();
    cropComboBox      = new QComboBox();
    mergeComboBox     = new QComboBox();
    fractComboBox     = new QComboBox();
}

void RegenerateWidget::createLayout()
{
    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("Group"), groupComboBox);
    layout->addRow(tr("Ungroup"), ungroupComboBox);
    layout->addRow(tr("Duplicate"), duplicateComboBox);
    layout->addRow(tr("Split"), splitComboBox);
    layout->addRow(tr("Crop"), cropComboBox);
    layout->addRow(tr("Merge"), mergeComboBox);
    layout->addRow(tr("Fract"), fractComboBox);
    setLayout(layout);
}

