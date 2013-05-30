#ifndef AFFICHEALARME_H
#define AFFICHEALARME_H

#include <QObject>
#include "ihm.h"

class AfficheAlarme : public QObject
{
    Q_OBJECT
public:
    explicit AfficheAlarme(QObject *parent = 0, int num_Lecteur=-1);
    
signals:
    void signalLecteurInactif(int);
    
public slots:
    void lecteurInactif();

private:
    int numLecteur;
};

#endif // AFFICHEALARME_H
