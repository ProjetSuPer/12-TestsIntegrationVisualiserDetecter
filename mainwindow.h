#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "clientconnection.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(Server* server, QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    Server* _server;

private slots:
    void addressLineEdit_textEdited(QString);
    void onPushButton_clicked();
    void on_killAllComPushButton_clicked();

    void server_switchedOn();
    void server_switchedOff();
    void server_switchedOffOnError(QString);
    void server_addressChanged(QString);
    void server_portChanged(quint16);
    void server_newConnection(const ClientConnection&);
};

#endif // MAINWINDOW_H
