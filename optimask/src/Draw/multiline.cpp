#include "multiline.h"
#include "ui_multiline.h"

MultiLine::MultiLine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiLine)
{
    ui->setupUi(this);
}

MultiLine::~MultiLine()
{
    delete ui;
}
