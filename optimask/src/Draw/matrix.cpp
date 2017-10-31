#include "matrix.h"
#include "ui_matrix.h"
#include <QFileDialog>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QMap>
#include <QMapIterator>
#include <QList>

Matrix::Matrix(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::Matrix)
{
    model = new QStandardItemModel(this);
    initialize();
    ui->setupUi(this);
    ui->tableView->setModel(model);
}

Matrix::~Matrix()
{
    delete ui;
}

void Matrix::initialize()
{
    model->setHorizontalHeaderLabels(QStringList() << tr("X")<< tr("Y") );
}

void Matrix::on_openButton_clicked()
{
    QString dir(".");
    QString filename = QFileDialog::getOpenFileName(this,
            tr("%1 - Open").arg(QApplication::applicationName()),
            dir,
            tr("%1 (*.txt)").arg(QApplication::applicationName()));
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Error"), tr("can't open the file"));
    }

   // QDataStream in(&file);
    QTextStream in(&file);
    uint number = 0;
    QString xValue;
    QString yValue;
    QMap<quint16, QList<QStandardItem*> > itemsForXY;

    while (!in.atEnd()) {
        in >> xValue >> yValue;
        QList<QStandardItem*> items;
        foreach (const QString &text, QStringList() << xValue<< yValue )
            items << new QStandardItem(text);
        itemsForXY[ ++number] = items;
    }
    file.close();

     QMapIterator<quint16, QList<QStandardItem*> > i(itemsForXY);
     while (i.hasNext())
         model->appendRow(i.next().value());

}

void Matrix::on_pushButton_clicked()
{
    QList<QStandardItem*> items;
//    QStandardItem *vertexItem = new QStandardItem;
//    vertexItem->setData(MinZipcode, Qt::EditRole);
//    items << zipItem;
    for(int i = 0; i<model->columnCount(); ++i){
        items << new QStandardItem(tr("0"));
    }
    model->appendRow(items);
    ui->tableView->scrollToBottom();
    ui->tableView->setFocus();
}
