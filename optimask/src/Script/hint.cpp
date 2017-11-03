#include "hint.h"

#include <QVBoxLayout>

Hint::Hint(QWidget *parent) : QWidget(parent)
{
    hintTEdit = new QTextEdit();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(hintTEdit);
    this->setLayout(mainLayout);
    hintTEdit->setText("Hint Dock");

}

Hint::~Hint()
{

}
