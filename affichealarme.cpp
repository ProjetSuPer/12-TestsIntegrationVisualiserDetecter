#include "affichealarme.h"

AfficheAlarme::AfficheAlarme(QObject *parent, int num_Lecteur) :
    QObject(parent)
{
    this->numLecteur = num_Lecteur;
}
void AfficheAlarme::lecteurInactif(){

    emit signalLecteurInactif(numLecteur);
}
