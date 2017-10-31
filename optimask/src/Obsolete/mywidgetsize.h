#ifndef MYWIDGETSIZE
#define MYWIDGETSIZE
#include <QtWidgets>

class MyWidget : public QWidget
{
public:
    QSize sizeHint() const
    {
        return QSize(100, 900); /* 在这里定义dock的初始大小 */
    }
};

#endif // MYWIDGETSIZE

