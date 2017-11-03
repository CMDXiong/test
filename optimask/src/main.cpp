/* Optimask主程序 */

#include "optimask.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Optimask w;
    w.show();

    return a.exec();
}
