#include <QApplication>
#include <QLabel>
#include "ihm.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ihm w;
    w.show();

    
    return a.exec();
}

