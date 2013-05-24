#include "clientmanager.h"
#include <QDebug>
#include <QThread>

ClientManager::ClientManager(int socketDescriptor) :
    QObject(), _socketDescriptor(socketDescriptor)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << socketDescriptor;
    _tcpSocket.setParent(this);
}

ClientManager::~ClientManager()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void ClientManager::manage()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "debut";

    if( ! setSocket())
        return;

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "fin";
}

void ClientManager::stop()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    emit sig_finished();
}

bool ClientManager::setSocket()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    bool ok;

    ok = _tcpSocket.setSocketDescriptor(_socketDescriptor);

    if(! ok)
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "ok:" << ok;
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "ok:" << ok;

    return ok;
}
