#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QNetworkProxy>
#include <QAuthenticator>

class TcpSocket : public QTcpSocket
{
Q_OBJECT
public:
    explicit TcpSocket(QObject *parent = 0);
    ~TcpSocket();

private slots:
    void slot_connected(); // When the connection has been established
    void slot_disconnected();
    void slot_error(QAbstractSocket::SocketError);
    void slot_hostFound();
    void slot_proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*);
    void slot_stateChanged(QAbstractSocket::SocketState); // Whenever the state changes
    void slot_aboutToClose();
    void slot_readyRead(); // emitted every time a new chunk of data has arrived
    void slot_readChannelFinished();
    void slot_bytesWritten(qint64); // When the client has read the data
};

#endif // TCPSOCKET_H
