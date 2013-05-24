#include <QApplication>
#include <QLabel>
#include <QTextCodec>
#include "ihm.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);
    qDebug();

    //Server server("192.168.60.100", "2222");

    Ihm w;
    w.show();

    return a.exec();
}

