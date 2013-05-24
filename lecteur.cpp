#include "lecteur.h"

Lecteur::Lecteur(QObject *parent) :
    QObject(parent)
{
}

int Lecteur::getnum_lecteur(){
    return this->num_lecteur;
}
