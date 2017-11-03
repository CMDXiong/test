#ifndef MULTILINE_H
#define MULTILINE_H

#include <QWidget>

namespace Ui {
class MultiLine;
}

class MultiLine : public QWidget
{
    Q_OBJECT

public:
    explicit MultiLine(QWidget *parent = 0);
    ~MultiLine();

private:
    Ui::MultiLine *ui;
};

#endif // MULTILINE_H
