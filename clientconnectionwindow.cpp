#include "clientconnectionwindow.h"
#include "ui_clientconnectionwindow.h"
#include <QDebug>
#include <QThread>

ClientConnectionWindow::ClientConnectionWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientConnectionWindow)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    ui->setupUi(this);
}

ClientConnectionWindow::~ClientConnectionWindow()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    delete ui;
}

void ClientConnectionWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ClientConnectionWindow::on_closeConnectionPushButton_clicked()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    emit sig_closeConnection();
}

void ClientConnectionWindow::slot_isAReader(Reader reader)
{
    QString string;

    string = "Reader num(" + QString::number(reader.number()) + "), "
              + "placeId(" + QString::number(reader.placeId()) + "), "
              + "address(" + reader.address() + "), "
              + "isConnected(" + QString::number(reader.isConnected()) + ").";

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO
            << "Reader num(" << reader.number() << "),"
        << "placeId(" << reader.placeId() << "),"
        << "address(" << reader.address() << "),"
        << "isConnected(" << reader.isConnected() << ").";

    ui->textEdit->append("*** Is a reader.\n" + string);
}

void ClientConnectionWindow::slot_isNotAReader(QString ip)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << ip;
    ui->textEdit->append("*** " + ip + " is not a reader");
}

void ClientConnectionWindow::slot_dataRead(QString data)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << data;
    ui->textEdit->append("Data received:" + data);
}

void ClientConnectionWindow::slot_disconnected()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    ui->textEdit->append("*** Disconnected");
}

void ClientConnectionWindow::slot_closed()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    ui->textEdit->append("*** Closed");
}

void ClientConnectionWindow::slot_destroyed()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    ui->textEdit->append("*** Destroyed");
}
