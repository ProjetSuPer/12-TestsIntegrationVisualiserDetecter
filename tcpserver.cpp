#include "tcpserver.h"
#include <QThread>
#include <QDebug>

TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << parent;
}

TcpServer::~TcpServer()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "close()";

    close();
}

void TcpServer::incomingConnection(int socketDescriptor)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "-> sig_newConnection" << socketDescriptor;

    emit sig_incomingConnection(socketDescriptor);
}
