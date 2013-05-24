#ifndef CLIENTCONNECTIONWINDOW_H
#define CLIENTCONNECTIONWINDOW_H

#include <QWidget>
#include "reader.h"

namespace Ui {
    class ClientConnectionWindow;
}

class ClientConnectionWindow : public QWidget {
    Q_OBJECT
signals:
    void sig_closeConnection();

public:
    ClientConnectionWindow(QWidget *parent = 0);
    ~ClientConnectionWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ClientConnectionWindow *ui;

private slots:
    void on_closeConnectionPushButton_clicked();
    void slot_isAReader(Reader);
    void slot_isNotAReader(QString);
    void slot_dataRead(QString);
    void slot_disconnected();
    void slot_closed();
    void slot_destroyed();
};

#endif // CLIENTCONNECTIONWINDOW_H
