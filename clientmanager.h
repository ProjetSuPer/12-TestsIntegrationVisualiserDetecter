#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "tcpsocket.h"

class ClientManager : public QObject
{
Q_OBJECT
public:
    explicit ClientManager(int);
    ~ClientManager();

signals:
    void sig_finished();

public slots:
    void manage();
    void stop();

private:
    bool setSocket();
    int _socketDescriptor;
    TcpSocket _tcpSocket;

};

#endif // CLIENTMANAGER_H
