#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QMainWindow>

namespace Ui {
class LibraryWidget;
}

class LibraryWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit LibraryWidget(QWidget *parent = 0);
    ~LibraryWidget();

private:
    Ui::LibraryWidget *ui;
};

#endif // LIBRARYWIDGET_H
