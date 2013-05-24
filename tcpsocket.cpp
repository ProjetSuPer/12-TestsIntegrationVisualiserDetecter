#include "tcpsocket.h"
#include <QThread>

TcpSocket::TcpSocket(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_error(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(hostFound()), this, SLOT(slot_hostFound()));
    connect(this, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(slot_proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slot_stateChanged(QAbstractSocket::SocketState)));
    connect(this, SIGNAL(aboutToClose()), this, SLOT(slot_aboutToClose()));
    connect(this, SIGNAL(readyRead()), this, SLOT(slot_readyRead()));
    connect(this, SIGNAL(readChannelFinished()), this, SLOT(slot_readChannelFinished()));
    connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(slot_bytesWritten(qint64)));
}

TcpSocket::~TcpSocket()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_connected()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_disconnected()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_error(QAbstractSocket::SocketError error)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << error;
}

void TcpSocket::slot_hostFound()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_stateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << state;
}

void TcpSocket::slot_aboutToClose()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_readyRead()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_readChannelFinished()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void TcpSocket::slot_bytesWritten(qint64 number)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << number;
}
