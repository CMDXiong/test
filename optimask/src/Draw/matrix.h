#ifndef MATRIX_H
#define MATRIX_H

#include <QTabWidget>

class QStandardItemModel;

namespace Ui {
class Matrix;
}

class Matrix : public QTabWidget
{
    Q_OBJECT

public:
    explicit Matrix(QWidget *parent = 0);
    ~Matrix();

private slots:
    void on_openButton_clicked();
    void on_pushButton_clicked();

private:
    void initialize();
private:
    Ui::Matrix *ui;
    QStandardItemModel *model;
};

#endif // MATRIX_H
