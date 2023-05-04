
#include "connectivitytest.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConnectivityTest w;
    w.show();
    return a.exec();
}
