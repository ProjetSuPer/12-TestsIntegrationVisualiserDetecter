#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class Thread : public QThread
{
Q_OBJECT
public:
    explicit Thread(QObject *parent = 0);
    ~Thread();
    void run();

private slots:
    void slot_started();
    void slot_finished();
    void slot_destroyed();
    void slot_terminated();
};

#endif // THREAD_H
