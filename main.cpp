#include <QApplication>
#include <QLabel>
#include <QTextCodec>
#include <QDebug>
#include "ihm.h"
#include "server.h"

int main(int argc, char *argv[])
{
    qDebug();

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QApplication a(argc, argv);";
    QApplication a(argc, argv);

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Server server(\"192.168.60.100\", \"2222\");";
    Server server("192.168.60.100", "2222");

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Ihm w(&server);";
    Ihm w(&server);

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "w.show();";
    w.show();

    return a.exec();
}

