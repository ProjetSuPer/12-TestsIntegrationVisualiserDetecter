#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QMutex>
#include "tcpsocket.h"
#include "reader.h"

class ClientConnection : public QObject
{
Q_OBJECT
signals:
    void sig_isAReader(Reader);
    void sig_isNotAReader(QString);
    void sig_dataRead(QString);
    void sig_frameReceived(QString);
    void sig_disconnected();
    void sig_closed(); // TODO : Inutile car en cas de rupture de connexion disconnected est aussi émis. A supprimer ?
    void sig_error(QString);

public:
    explicit ClientConnection(int);
    ~ClientConnection(); // TODO : Personne ne devrait l'employer, le rendre privé ?

public slots:
    void open(); // TODO : Seul server (ou le thread ?) l'emploient, la rendre privée et ajouter server en amis ?
    void close();

private:
    TcpSocket _tcpSocket;
    int _socketDescriptor;
    bool _opened;
    static QMutex _mutex;
    bool _isAReader;
    QString _clientAddress;
    QString _bufferDataRead;

private slots:
    void filter(); // N'est connecté nulle part. N'a pas à être un slot ?
    void readyRead();
    void bleedBuffer();
    void on_disconnected();
};

#endif // CLIENTCONNECTION_H
