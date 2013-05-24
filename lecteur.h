#ifndef LECTEUR_H
#define LECTEUR_H

#include <QObject>

class Lecteur : public QObject
{
    Q_OBJECT
public:
    explicit Lecteur(QObject *parent = 0);

    int getnum_lecteur();

private:
    int num_lecteur;
    int num_lieu;
    QString ip;
    bool estConnecte;

signals:
    
public slots:
    
};

#endif // LECTEUR_H
