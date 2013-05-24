#include "server.h"
#include "clientmanager.h"
#include <QThread>
#include "thread.h"

Server::SwitchOnState Server::switchOn()
{
    SwitchOnState state = Success;

    if(! _server.isListening())
    {
        bool ok = _server.listen(_address, _port);

        if(ok)
        {
            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "-> sig_switchedOn";

            _setAddress(_server.serverAddress().toString());
            _setPort(QString::number(_server.serverPort()));
            _clientConnectionErrorReceived = false;

            emit sig_switchedOn();
        }
        else
        {
            QAbstractSocket::SocketError socketError = _server.serverError();

            switch(socketError)
            {
            case QAbstractSocket::SocketAddressNotAvailableError:
                state = this->AddressNotAvailableError;
                break;

            case QAbstractSocket::SocketAccessError:
                state = this->PortProtectedError;
                break;

            case QAbstractSocket::AddressInUseError:
                state = this->PortAlreadyInUseError;
                break;

            default:
                state = this->UnknownError;
            }

            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "erreur listen, return " << state;
        }
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "ignore (deja en ecoute), return" << state;

    return state;
}

void Server::switchOff()
{
    if(_server.isListening())
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "close -> sig_switchedOff";

        _server.close();
        emit sig_switchedOff();
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

bool Server::setAddress(QString address)
{
    bool ok = false;

    if(! _server.isListening())
        ok = _setAddress(address);
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << address << "ignore (deja en ecoute), return" << ok;

    return ok;
}

bool Server::setPort(QString port)
{
    bool ok = false;

    if(! _server.isListening())
    {
        ok = _setPort(port);
    }
    else
         qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << port << "ignore (deja en ecoute), return" << ok;

    return ok;
}

void Server::closeAllClientConnection()
{
    emit sig_closeAllClientConnection();
}

Server::Server(QString address, QString port, QObject *parent) :
    QObject(parent)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << address << port << parent;

    _setAddress(address);
    _setPort(port);
    _clientConnectionErrorReceived = false;

    this->connect(&_server, SIGNAL(sig_incomingConnection(int)), SLOT(incomingConnection(int)));
}

Server::~Server()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;

    int i = 0;
    int count = _clientThreadMap.count();
    QMapIterator<ClientConnection*, Thread*> mapIterator(_clientThreadMap);

    while (mapIterator.hasNext())
    {
        i++;
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << i << "/"  << count;
        mapIterator.next();

        ClientConnection* clientConnection = mapIterator.key();
        Thread* thread = mapIterator.value();

        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete" << thread;
        delete thread;

        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete" << clientConnection;
        delete clientConnection;
    }

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "map.clear";
    _clientThreadMap.clear();
}

QString Server::address()
{
    return _address.toString();
}

quint16 Server::port()
{
    return _port;
}

void Server::incomingConnection(int socketDescriptor)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << socketDescriptor;

    ClientConnection* clientConnection;
    clientConnection = new ClientConnection(socketDescriptor);

    Thread* thread;
    thread = new Thread;

    _clientThreadMap.insert(clientConnection, thread);

    // MoveToThread
    clientConnection->moveToThread(thread);

    // Connexions

    // Thread::started() déclenche clientConnection::open()
    clientConnection->connect(thread, SIGNAL(started()), SLOT(open()));

    // Une erreur chez un client entrainera l'arrêt de l'écoute du serveur
    this->connect(clientConnection, SIGNAL(sig_error(QString)), SLOT(clientConnection_error(QString)));

    // Le serveur surveille les déconnexions de ses clients pour libérer les ressources
    this->connect(clientConnection, SIGNAL(sig_disconnected()), SLOT(clientConnection_disconnected()));

    // Le signal closeAllClientConnection déclenchera le close() de TOUS les clientconnection
    clientConnection->connect(this, SIGNAL(sig_closeAllClientConnection()), SLOT(close()));

    // clientConnection::sig_disconnected() déclenchera sa propre destruction
    //clientConnection->connect(clientConnection, SIGNAL(sig_disconnected()), SLOT(deleteLater()));

    // L'arrêt du thread entrainera la destruction de clientConnection
    //clientConnection->connect(thread, SIGNAL(finished()), SLOT(deleteLater()));

    // La destruction de clientConnection déclenchera l'arrêt du thread
    //thread->connect(clientConnection, SIGNAL(destroyed()), SLOT(quit()));

    // L'arrêt du thread déclenche la destruction de clientconnection
    //clientConnection->connect(thread, )

    // L'arrêt du thread déclenchera sa propre destruction
    //thread->connect(thread, SIGNAL(finished()), SLOT(deleteLater()));
    // TODO : Traiter de la même façon le signal terminated ?

    // La destruction de clientConnection déclenchera la destruction du thread
    //thread->connect(clientConnection, SIGNAL(destroyed()), SLOT(deleteLater()));

    emit sig_newConnection(*clientConnection);

    // Lancement du thread
    thread->start();
}

bool Server::_setAddress(QString address)
{
    bool ok;
    QHostAddress addressQHost;

    ok = addressQHost.setAddress(address);

    if(ok && addressQHost != _address)
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << address << "-> sig_addressChanged, return" << ok;

        _address.setAddress(address);
        emit sig_addressChanged(address);
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << address << "ignore (mauvais format ou meme valeur), return" << ok;

    return ok;
}

bool Server::_setPort(QString port)
{
    bool ok;
    quint16 portQuint16 = port.toULong(&ok); // quint16 <=> ulong

    if(ok && portQuint16 != _port)
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << port << "-> sig_portChanged, return" << ok;

        _port = portQuint16;
        emit sig_portChanged(portQuint16);
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << port << "ignore (mauvais format ou meme valeur), return" << ok;

    return ok;
}

void Server::clientConnection_error(QString error)
{
    // Seulement si aucune erreur n'a été rencontrée depuis le dernier SwitchOn
    if(!_clientConnectionErrorReceived)
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;

        // Mémoriser le passage ici pour ne pas avoir à le faire plusieurs fois
        _clientConnectionErrorReceived = true;

        // Stopper l'écoute pour eviter un flood de demande de connexion en cas d'erreur répétitive
        switchOff();

        // Et le signaler
        emit sig_switchedOffOnError(error);
    }
}

void Server::clientConnection_disconnected()
{
    ClientConnection* sender = (ClientConnection*) QObject::sender();
    Thread* thread = _clientThreadMap.take(sender);

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete" << thread;
    delete thread;

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete" << sender;
    delete sender;
}

QDebug operator<<(QDebug debug, const Server::SwitchOnState& state)
{
    QString stateString;

    switch(state)
    {
    case Server::Success:
        stateString = "Success";
        break;
    case Server::AddressNotAvailableError:
        stateString = "AddressNotAvailableError";
        break;
    case Server::PortProtectedError:
        stateString = "PortProtectedError";
        break;
    case Server::PortAlreadyInUseError:
        stateString = "PortAlreadyInUseError";
        break;
    case Server::UnknownError:
        stateString = "UnknownError";
        break;
    }

    debug << stateString;

    return debug.nospace();
}
