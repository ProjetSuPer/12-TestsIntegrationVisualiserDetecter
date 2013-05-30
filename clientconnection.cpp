#include "clientconnection.h"
#include "reader.h"
#include "bddConfig.h"
#include <QThread>
#include <QMetaType>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegExp>

QMutex ClientConnection::_mutex;

ClientConnection::ClientConnection(int socketDescriptor) :
    QObject(), _socketDescriptor(socketDescriptor), _opened(false), _isAReader(false), _clientAddress(""), _bufferDataRead("")
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << socketDescriptor;
    _tcpSocket.setParent(this);

    this->connect(&_tcpSocket, SIGNAL(disconnected()), SIGNAL(sig_disconnected()));
    // TODO this->connect(&_tcpSocket, SIGNAL(disconnected()), SLOT(deleteLater())); // C'est Server qui crée des ClientConnection, c'est donc Server qui décide quand les détruire
    this->connect(&_tcpSocket, SIGNAL(disconnected()), SLOT(on_disconnected()));
}

ClientConnection::~ClientConnection()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    _tcpSocket.close();
    on_disconnected();
}

void ClientConnection::open()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;

    if(! _opened)
    {
        _tcpSocket.setSocketDescriptor(_socketDescriptor);

        if(_tcpSocket.isValid())
        {
            _opened = true;

            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "_tcpSocket.isValid()," << _opened;

             // Activation de l'option KeepAlive
            _tcpSocket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);

            // Récupère l'adresse du client.
            _clientAddress = _tcpSocket.peerAddress().toString();
            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "clientAddress:" << _clientAddress;

            filter();
        }
        else
        {
            emit sig_error("socket invalid"); // TODO : Utiliser un enum ?
            // deleteLater(); C'est Server qui crée des ClientConnection, c'est donc Server qui décide quand les détruire
        }
    }
}

void ClientConnection::close()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    _tcpSocket.close();
    emit sig_closed(); // TODO : Supprimer ?
}

void ClientConnection::filter()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;

    // Demande à la BDD si c'est un lecteur.
    QString nameDatabaseConnexion = QString::number(QThread::currentThreadId());
    Reader reader;

    {
        _mutex.lock();
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", nameDatabaseConnexion);
        _mutex.unlock();

        db.setConnectOptions("MYSQL_OPT_RECONNECT=5");
        db.setHostName(BDD_HOST_NAME);
        // TODO : Envoyer un nom invalide à setDatabaseName pour tester le comportement du code
        db.setDatabaseName(BDD_DATABASE_NAME);
        db.setUserName(BDD_USER_NAME);
        db.setPassword(BDD_PASSWORD);

        if (!db.open())
        {
            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Error : QSqlDatabase::open() fail :" << db.lastError();
            emit sig_error("open database error");
            // TODO : Stopper proprement
            _tcpSocket.close();
        }
        else
        {
            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlDatabase::open() : Success.";

            QSqlQuery query(db);
            // TODO : Glisser une erreur de requete pour tester le comportement du code
            query.exec("SELECT  num_lecteur, num_lieu, ip, estConnecte FROM lecteur WHERE ip like \"" + _clientAddress + "\"");
            if(!query.isActive())
            {
                qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Error : QSqlQuery::exec() [reader ip" << _clientAddress << "exists ?] fail.";
                emit sig_error("sql error : [Select reader infos]");
                // TODO : Stopper proprement
                _tcpSocket.close();
            }
            else
            {
                qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [lecteur d'ip X existe ?] ok";

                if(query.size() == 0)
                {
                    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "query.size() == 0 -> isNotAReader";
                    // Signale la détection d'un intrus
                    emit sig_isNotAReader(_clientAddress);

                    // Le laisser connecté (évite le flood de connexion déconnexion)
                    // Mais lui vider son buffer dès qu'il se remplit
                    this->connect(&_tcpSocket, SIGNAL(readyRead()), SLOT(bleedBuffer()));
                }
                else
                {
                    query.next();
                    reader.number(query.value(0).toInt());
                    reader.placeId(query.value(1).toInt());
                    reader.address(query.value(2).toString());
                    reader.isConnected(true);
                    query.finish();

                    // Update BDD (lecteur connecté)
                    // TODO : Glisser une erreur de requete pour tester le comportement du code
                    query.exec("UPDATE lecteur SET estConnecte=" + QString::number(reader.isConnected()) + " WHERE ip=\"" + reader.address() + "\";");
                    if(!query.isActive())
                    {
                        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [Update lecteur.estConnecte] ERROR";
                        emit sig_error("sql error : [Update reader.isConnected]");
                        // TODO : Stopper proprement
                    }
                    else
                    {
                        _isAReader = true;
                        query.finish();

                        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [Update lecteur.estConnecte] ok -> sig_isAReader(reader)";
                        qRegisterMetaType<Reader>("Reader");

                        // Signale la détection d'un lecteur
                        emit sig_isAReader(reader);

                        // Traiter les trames reçues
                        this->connect(&_tcpSocket, SIGNAL(readyRead()), SLOT(readyRead()));
                    }
                }
            }
            db.close();
        }
    }

    QSqlDatabase::removeDatabase(nameDatabaseConnexion); // TODO : Semble inutile, db est détruit au bloc précédent. L'enlever et réindenter ?
}

void ClientConnection::readyRead()
{
    qint64 nbBytesAvailable = _tcpSocket.bytesAvailable();
    QString dataRead = _tcpSocket.readAll();
    _bufferDataRead += dataRead;

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "bytesAvailable:" << nbBytesAvailable;
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "dataRead :" << dataRead;
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "_bufferDataRead :" << _bufferDataRead;

    emit sig_dataRead(dataRead);

    if(_bufferDataRead.count() >= 12)
    {
        QString regexFrameString("\\[[0-9A-F]{10}\\]");
        QRegExp lastValidFrameRegex("(" + regexFrameString + ")(?!" + regexFrameString + ")");

        if(_bufferDataRead.contains(lastValidFrameRegex))
        {
            QString lastValidFrameString = lastValidFrameRegex.cap(1);
            _bufferDataRead.clear();

            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "lastValidFrameString :" << lastValidFrameString;

            emit sig_frameReceived(lastValidFrameString);
        }
        else
            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "! data.contains(lastFrameRegex)";
    }
}

void ClientConnection::bleedBuffer()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    _tcpSocket.readAll();
}

void ClientConnection::on_disconnected()
{
    if (!_isAReader)
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "isNotAReader";
    else
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "isAReader";
        QString nameDatabaseConnexion = QString::number(QThread::currentThreadId());
        {
            _mutex.lock();
            QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", nameDatabaseConnexion);
            _mutex.unlock();

            db.setHostName(BDD_HOST_NAME);
            db.setDatabaseName(BDD_DATABASE_NAME);
            db.setUserName(BDD_USER_NAME);
            db.setPassword(BDD_PASSWORD);

            if (!db.open())
            {
                qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Error : QSqlDatabase::open() fail :" << db.lastError();
                emit sig_error("open database error");
            }
            else
            {
                QSqlQuery query(db);
                query.exec("UPDATE lecteur SET estConnecte=" + QString::number(false) + " WHERE ip=\"" + _clientAddress + "\";");
                if(!query.isActive())
                {
                    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [Update reader.isDisconnected] ERROR";
                    emit sig_error("sql error : [Update reader.isDisconnected]");
                }
                else
                {
                    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [Update lecteur.isDisconnected] ok";
                    query.finish();
                }
                db.close();
            }
        }

        QSqlDatabase::removeDatabase(nameDatabaseConnexion); // TODO : Semble inutile, db est détruit au bloc précédent. L'enlever et réindenter ?
    }
}
