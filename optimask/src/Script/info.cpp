#include "info.h"

#include <QVBoxLayout>

Info::Info(QWidget *parent) : QWidget(parent)
{
    infoTEdit = new QTextEdit();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(infoTEdit);
    this->setLayout(mainLayout);
    //this->setMaximumHeight(100);
    infoTEdit->setText("Info Dock");

}

Info::~Info()
{

}


