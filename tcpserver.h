#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>

class TcpServer : public QTcpServer
{
Q_OBJECT

signals:
    void sig_incomingConnection(int);

public:
    explicit TcpServer(QObject *parent = 0);
    ~TcpServer();

protected:
     void incomingConnection(int socketDescriptor);
};

#endif // TCPSERVER_H
