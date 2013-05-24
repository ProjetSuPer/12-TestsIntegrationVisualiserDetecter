#include "ihm.h"
#include "ui_ihm.h"
#include "contenuonglet.h"
#include <QtSql>
#include <QString>
#include <QtDebug>
#include <QMessageBox>
#include <QLabel>
#include <QToolTip>


//////////////////////
/*** CONSTRUCTEUR ***/
Ihm::Ihm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Ihm)
{
    ui->setupUi(this);
    pLecteur = new Lecteur; //à enlever à l'intégration
    pDynamique = new Dynamique;
    pBdd = new Bdd;

/* //signaux
     Bouton btNewLecteur
    connect(ui->btNewLecteur, SIGNAL(clicked()), this, SLOT(lecteurActif(pLecteur)));

    //connect
    connect(this, SIGNAL(signalNewLecteur(const Lecteur &)), this, SLOT(lecteurActif(const Lecteur &)));
    connect(this, SIGNAL(signalDelLecteur(pLecteur)), this, SLOT(lecteurInactif(pLecteur)));
    //émission signal
   emit signalNewLecteur(pLecteur);
*/

    //réception signal homme en danger
    connect(this, SIGNAL(signalHommeEnDanger(QString &)), this, SLOT(hommeEnDanger(QString &)));
    //réception signal perte réceptin
    connect(this, SIGNAL(signalPerteReception(int, int, T_ListeLabel *)), this, SLOT(perteReception(int, int, T_ListeLabel *)));

    //obtention du nombre de vue max
    int vueMax = pBdd->getVueMax();

    //déclaration QList
    QList<T_TupleOnglet> listeTupleO;

    //récupération des infos sur les onglets
    pBdd->getVue(&listeTupleO);

    if(!listeTupleO.empty()){
        for(int i = 0; i < listeTupleO.count() && i < vueMax; i++) {
            int num_vue = listeTupleO.at(i).num_vue;
            QString legende = listeTupleO.at(i).legende;
            QString image = listeTupleO.at(i).image;
            //ajout de l'onglet
            this->ajoutOnglet(num_vue, legende, image);
        }
    }

    //supprimer les deux onglets de base
    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(vueMax);
    //mettre l'onglet de base dans la vue
    ui->tabWidget->setCurrentIndex(0);


    //régler les temps des timer en fonction de la base de données
    int tempoMouv; // ms tempo pour le timer mouvement
    int tempoRec; // ms tempo pour le timer de réception
    pBdd->getTempo(&tempoMouv, &tempoRec);
    this->setTempo(tempoMouv, tempoRec);


    lecteurActif(pLecteur);     // à enlever à l'intégration
   // lecteurInactif(pLecteur);   // à enlever à l'intégration
   // lecteurInconnu();           // à enlever à l'intégration

    traitementTrame("F60016A703");  //à enlever à l'intégration
   // traitementTrame("050026B102");  //à enlever à l'intégration
    //trame type : AD D01 6A7 01
    //AD niveau de reception
    //DO1 n° de badge
    //6A7 mouvement
    //01 n° lecteur

}
////////////
//
///////////
void Ihm::setTempo(int tempoMouv, int tempoRec){

    this->tempoM = tempoMouv;
    this->tempoR = tempoRec;

}

///////
//SLOT homme en danger
///////
void Ihm::hommeEnDanger(QString & nom){
    //affichage texte alarme
    ui->txtAlarme->textCursor().insertText("<ALARME> "+ nom + " est en danger ! Aucun mouvement.\n");
}
//////
//SLOT perte réception
//////
void Ihm::perteReception(int numBadge, int numLecteur, T_ListeLabel *tll){


    //obtenir vue(s) en fonction du lecteur
    //déclaration QList
    QList<T_TupleLecteurS> listeTupleL1;

    pBdd->getVueFctLect(numLecteur, &listeTupleL1);

    if (!listeTupleL1.empty()){
        for (int i = 0; i < listeTupleL1.count(); i++){

            int num_vue = listeTupleL1.at(i).num_vue;

            //affichage
            tll->labelB[num_vue][numBadge]->setEnabled(true);

            //en fonction de l'état
            //dans tout les cas images sans sens de passage
            if (num_vue == 1){
                //petite image
                tll->labelB[num_vue][numBadge]->setPixmap(QPixmap("../ressources/pers_orange.jpg"));
            }else{    //image normale
                tll->labelB[num_vue][numBadge]->setPixmap(QPixmap("../ressources/pers_orange_2.jpg"));
            }
        } //fin for
    } //fin if
}

/*-------------------------------*
 * Slot traitement de la trame   *
 *-------------------------------*/
bool Ihm::traitementTrame(QString trame){

    //témoin timer affichage
    if (ui->lbActivite->isEnabled())
        ui->lbActivite->setEnabled(false);
    else
        ui->lbActivite->setEnabled(true);

    //décodage trame
    QString num_badge, sens, mouvement, num_lecteur;
    T_ListeLabel *tll;  //pointeur sur structure

    //séparation des parties de la trame
    num_badge = trame.mid(2,3); //numéro de badge

    //suppression mauvais badge
    if(num_badge == "000") {
        qDebug("Mauvais badge.");
        ui->txtAlarme->textCursor().insertText("<Erreur> Mauvais badge num=000\n");
        return false;
    }

    sens = trame.mid(0,2); //niveau de réception du tag
    mouvement = trame.mid(5,3); //niveau de mouvement mesuré
    num_lecteur = trame.mid(8,2);   //numéro du lecteur

    //conversion des valeurs en int à partir de ASCII hexa et mise à l'échelle
    //c'est-à-dire conversion de l'hexadécimal en décimal
    int num_badge_i = num_badge.toInt(0,16);
    int sens_i = sens.toInt(0,16);
    int num_lecteur_i = num_lecteur.toInt(0,16);
    int mouvement_i = mouvement.toInt(0,16);

    //si le badge n'existe pas dans la BDD
    if(!pBdd->badgeExiste(num_badge)){
        ui->txtAlarme->textCursor().insertText("<Erreur><Badge "+num_badge+QString::fromUtf8("> Badge inconnu  dans la Base de données\n"));
        return false;
    }

    //badge n'existe pas sur l'IHM
    if(!pDynamique->BadgeActif[num_badge_i]){

        //Historique des événements (log) : nouveau badge
        pBdd->setLog(1, num_badge_i);    //1=nouveau badge

        tll = new T_ListeLabel();

        for(int i=0 ; i<MAXLECTEURS ; i++){   // init à 100
            for(int j=0 ; j<MAXVAL ; j++){
                tll->moySens[i][j]=100;
            }
        }
        for(int i=0 ; i<MAXLECTEURS ; i++){
            tll->sdp[i]=0;      //sens de passage
            tll->sdpMem[i]=0;
        }
        memset(tll->indMoy, 0, sizeof(tll->indMoy));    //init à 0

        //obtenir vue(s) en fonction du lecteur
        //déclaration QList
        QList<T_TupleLecteurS> listeTupleL;

        pBdd->getVueFctLect(num_lecteur_i, &listeTupleL);

        //récupération des infos dans la liste
        if (!listeTupleL.empty()){
            for (int i = 0; i < listeTupleL.count(); i++) {

                int num_vue = listeTupleL.at(i).num_vue;

                //se placer sur l'onglet
                QWidget *onglet;
                onglet = pDynamique->onglet[num_vue];

                //nouveau label dynamique pour un badge
                tll->labelB[num_vue][num_badge_i] = new QLabel(onglet);

                //réglage par défaut du nouveau badge (vert + haut)
                tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/haut_vert.jpg"));
                tll->labelB[num_vue][num_badge_i]->setGeometry(590, 620, 15, 42); // largeur hauteur à définir
            }
        }

        tll->numBadge = num_badge_i;        //numéro de badge
        tll->numLecteur = num_lecteur_i;    //numéro de lecteur
        tll->etat = 0;                      //aller

        // réglage du timer associé au mouvement
        tll->tpsMouv = new QTimer(this);                                    //nouveau Timer
        connect(tll->tpsMouv, SIGNAL(timeout()), this, SLOT(timerMouv()));  //connect timeout
        tll->tpsMouv->setSingleShot(true);                                  //un seul temps
        tll->tpsMouv->start(this->tempoM);                                  //débute le timer

        // réglage du timer associé à la réception
        tll->tpsSens[num_lecteur_i] = new QTimer(this);                                     //nouveau Timer
        connect(tll->tpsSens[num_lecteur_i], SIGNAL(timeout()), this, SLOT(timerRec()));   //connect timeout
        tll->tpsSens[num_lecteur_i]->setSingleShot(true);                                   //un seul temps
        tll->tpsSens[num_lecteur_i]->start(this->tempoR);                                   //débute le timer

        // ajout à la liste mémoire
        listeLabel.append(tll);

        //maintenant le badge existe sur l'IHM donc le sauvegarder
        pDynamique->BadgeActif[num_badge_i] = true;
    }

    tll->numLecteur = num_lecteur_i;    //sauvegarde numéro lecteur

    tll->etat |= MOUV0;   // mouv=0
    //relance du timer si mouvement
    if (mouvement_i > 0 ) {  // si mouvement
        tll->etat &= ~MOUV;    // alarme mouvement
        tll->etat &= ~MOUV0;   // un seul mouvement
        tll->tpsMouv->setSingleShot(true);
        tll->tpsMouv->start(this->tempoM);  //ms
    }

    // réarmer le timer REC, le créer si nouveau lecteur
    tll->etat &= ~REC;
    //création timer réception, si nouveau lecteur
    if (!tll->tpsSens[num_lecteur_i]) {
        tll->tpsSens[num_lecteur_i] = new QTimer(this);
        connect(tll->tpsSens[num_lecteur_i], SIGNAL(timeout()), this, SLOT(TimerRec()));
    }
    //réarmer le timer de réception
    tll->tpsSens[num_lecteur_i]->setSingleShot(true);
    tll->tpsSens[num_lecteur_i]->start(this->tempoR);


    //déclaration QList
    QList<T_Personne > listePersonne;

    //Recherche identité de la personne
    int num_pers = pBdd->badgeIdentite(num_badge_i, &listePersonne);
    if (num_pers == -1){
        //le badge n'est pas lié avec une personne
        ui->txtAlarme->textCursor().insertText("<Erreur><Badge "+num_badge+QString::fromUtf8("> Badge non lié à une personne\n"));
    } else {
        tll->nom[num_pers] = listePersonne.at(0).nom;
        tll->prenom[num_pers] = listePersonne.at(0).prenom;
        tll->societe[num_pers] = listePersonne.at(0).societe;
        tll->photo[num_pers] = listePersonne.at(0).photo;
    }


    // calcul de la moyenne de la sensibilité
    tll->moySens[num_lecteur_i][tll->indMoy[num_lecteur_i]++] = sens_i ;

    if (tll->indMoy[num_lecteur_i] == MAXVAL){
        tll->indMoy[num_lecteur_i] = 0;     //indice du tableau de moyenne
    }
    int moy = 0;
    moy = calculerMoyenne(tll);     //sur MAXVAL valeur
    tll->sdp[num_lecteur_i] = moy;  //mémo pour calcul sens de passage
    moy -= 100;

    if (!sensDePassage(tll)){ //maj de zone et du sens de passage de ce badge
        //pas de sens de passage
        qDebug("pas de sens de passage dans BDD");
        ui->txtAlarme->textCursor().insertText("<Erreur><Lecteur "+num_lecteur+ QString::fromUtf8("> Pas de sens de passage précisé dans BDD\n"));
        return false;
    }

    // recherche si lecteur n'est pas connecté
    if (!pBdd->getEtatLect(num_lecteur_i)){
        qDebug("le lecteur n'est pas connecte ?!");
        ui->txtAlarme->textCursor().insertText("<Erreur><Lecteur "+num_lecteur+QString::fromUtf8("> Lecteur non connecté\n"));
        return false;
    }

    //Obtenir les points de la zone en fonction des vues

    //obtenir vue(s) en fonction du lecteur
    //déclaration QList
    QList<T_TupleLecteurS> listeTupleL;

    pBdd->getVueFctLect(num_lecteur_i, &listeTupleL);

    if (!listeTupleL.empty()){
        for (int i = 0; i < listeTupleL.count(); i++) {

            int num_vue = listeTupleL.at(i).num_vue;

            pBdd->getPointsZone(num_vue, tll->zone, &tll->ptA, &tll->ptB);

            this->calculerDroite(moy, tll->ptA, tll->ptB, &tll->ptBadge[num_vue]);

            //affichage
            tll->labelB[num_vue][num_badge_i]->setEnabled(true);

            //en fonction de l'état
            switch(tll->etat) {
            case 0:  // ALLER
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_vert.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/haut_vert.jpg"));
                }
                break;
            case 1:
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_rouge.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/haut_rouge.jpg"));
                }
                //homme en danger
                emit signalHommeEnDanger(tll->nom[num_pers]);
                //Historique des événements (log) : alarme mouvement
                pBdd->setLog(3, num_badge_i);    //3=alarme mouvement
                break;
            case 2:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            case 3:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            case 4:
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_vert.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/bas_vert.jpg"));
                }
                break;
            case 5:
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_rouge.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/bas_rouge.jpg"));
                }
                //homme en danger
                emit signalHommeEnDanger(tll->nom[num_pers]);
                //Historique des événements (log) : alarme mouvement
                pBdd->setLog(3, num_badge_i);    //3=alarme mouvement
                break;
            case 6:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            case 7:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            case 8:
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_orange.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/haut_orange.jpg"));
                }
                break;
            case 9:
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_rouge.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/haut_rouge.jpg"));
                }
                //homme en danger
                emit signalHommeEnDanger(tll->nom[num_pers]);
                //Historique des événements (log) : alarme mouvement
                pBdd->setLog(3, num_badge_i);    //3=alarme mouvement
                break;
            case 10:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            case 11:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            case 12:
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_orange.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/bas_orange.jpg"));
                }
                break;
            case 13:
                if (num_vue == 1 || tll->zone == -1){
                    //pas de sens de passage
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/pers_rouge.jpg"));
                } else {
                    tll->labelB[num_vue][num_badge_i]->setPixmap(QPixmap("../ressources/bas_rouge.jpg"));
                }
                //homme en danger
                emit signalHommeEnDanger(tll->nom[num_pers]);
                //Historique des événements (log) : alarme mouvement
                pBdd->setLog(3, num_badge_i);    //3=alarme mouvement
                break;
            case 14:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            case 15:
                tll->labelB[num_vue][num_badge_i]->setEnabled(false);
                break;
            } //fin switch

            //affichage position exacte badge
            if (num_vue==1 && num_pers==1)  //taille petite, pas de décalement
                tll->labelB[num_vue][num_badge_i]->setGeometry(tll->ptBadge[num_vue].x, tll->ptBadge[num_vue].y,15,20);
            else if (num_vue==1 && num_pers!=1) //taile petite, décalement
                tll->labelB[num_vue][num_badge_i]->setGeometry(tll->ptBadge[num_vue].x + (15*num_pers), tll->ptBadge[num_vue].y,15,20);

            else if (num_vue!=1 && num_pers==1) //taille grande, pas de décalement
                tll->labelB[num_vue][num_badge_i]->setGeometry(tll->ptBadge[num_vue].x, tll->ptBadge[num_vue].y,30,20);
            else    //taille grande, décalement
                tll->labelB[num_vue][num_badge_i]->setGeometry(tll->ptBadge[num_vue].x + (30*num_pers), tll->ptBadge[num_vue].y,30,20);

            //affichage identité personne
            if (num_pers != -1) {
               tll->labelB[num_vue][num_badge_i]->setToolTip("<img src=':" + tll->photo[num_pers] + "'/>"
                                                             +" Badge "+ QString::number(num_badge_i) +" de : "
                                                             + tll->nom[num_pers] +" "  + tll->prenom[num_pers]
                                                             +QString::fromUtf8(" Société : ")+ tll->societe[num_pers]);

            } else { //badge pas affecté
                tll->labelB[num_vue][num_badge_i]->setToolTip(QString::fromUtf8("Badge non affecté à une personne"));
            }

        } //fin for
    } //fin if

    pBdd->setBadgeActif(num_badge_i);      //le badge n'est pas perdu
    return true;
}


///////////////////////////////////////////////////////////////
void Ihm::calculerDroite(int sens, T_Point pointA, T_Point pointB, T_Point *pointF)
{
    //pas de calcul, les points correspondent à la droite (uniquement vue 1)
    if ((pointB.x == 0) && (pointB.y == 0)){
        pointF->x = pointA.x;
        pointF->y = pointA.y;
    }
    //sinon calculer position
    else{
        float dx, dy, a, x, y;

        dx = pointB.x - pointA.x;
        dy = pointB.y - pointA.y;

        x = sens*dx/100;  // mise à l'échelle
        a = dy/dx;     // coeff directeur, pas d'ordonnée à l'origine car changement de repère
        y = a*x;   // équation de la droite
        pointF->x = pointA.x + x;
        pointF->y = pointA.y + y;
    }

}

///////////////////////////////////////////////////////////////
bool Ihm::sensDePassage(T_ListeLabel *tll)
{
    int sensMonter = pBdd->getSensMonter(tll->numLecteur);

    //sens de montée = rapprochement
    if (sensMonter == 1){

        //RSSI plus petit donc aller
        if (tll->sdp[tll->numLecteur] < tll->sdpMem[tll->numLecteur]){
            tll->etat |= AR;
            tll->sdpMem[tll->numLecteur] = tll->sdp[tll->numLecteur];   //sauvegarde
        }
        //RSSI plus grand donc retour
        if (tll->sdp[tll->numLecteur] > tll->sdpMem[tll->numLecteur]){
            tll->etat &= ~AR;
            tll->sdpMem[tll->numLecteur] = tll->sdp[tll->numLecteur];   //sauvegarde
        }

        //zone
        if (tll->sdp[tll->numLecteur]>0)
            tll->zone = tll->numLecteur;

        return true;
    //sens de montée = éloignement
    }else if (sensMonter == 2){

        //RSSSI plus petit donc retour
        if (tll->sdp[tll->numLecteur] < tll->sdpMem[tll->numLecteur]){
            tll->etat &= ~AR;
            tll->sdpMem[tll->numLecteur] = tll->sdp[tll->numLecteur];   //sauvegarde
        }
        //RSSI plus grand donc aller
        if (tll->sdp[tll->numLecteur] > tll->sdpMem[tll->numLecteur]){
            tll->etat |= AR;
            tll->sdpMem[tll->numLecteur] = tll->sdp[tll->numLecteur];   //sauvegarde
        }

        //zone
        if (tll->sdp[tll->numLecteur]>0)
            tll->zone = tll->numLecteur;

        return true;
    //sens de montée = zone contigüe
    }else if (sensMonter == 3){

        //détermination de la zone contigüe
        if (tll->sdp[tll->numLecteur+1]>0)
            tll->zone = tll->numLecteur*11+1;

        if (tll->sdp[tll->numLecteur-1]>0)
            tll->zone = (tll->numLecteur-1)*11+1;

        return true;
    //pas de sens de passage
    }else{
        tll->zone = -1;
        return false;
    }

}


///////////////////////////////////////////////////////////////
int Ihm::calculerMoyenne(T_ListeLabel *tll)
{
    // calcul de la moyenne de la sensibilité
    int sumMoy=0;
    for (int i=0 ; i<MAXVAL ; i++)
        sumMoy += tll->moySens[tll->numLecteur][i];
    return sumMoy / MAXVAL;

} // method




///////////////////////////////////////////////////////////
void Ihm::timerMouv() {
    T_ListeLabel *tll;
    int nbB = listeLabel.size();
    for (int i=0 ; i<nbB ; i++) {
        tll = listeLabel.at(i);
        //si Timer n'est pas actif
        if (!tll->tpsMouv->isActive()) {
            tll->etat |= MOUV;   // homme en danger
        }
    }
}
///////////////////////////////////////////////////////////
void Ihm::timerRec() {
    T_ListeLabel *tll;
    int nbB = listeLabel.size();
    //parcours des badges
    for (int i=0 ; i<nbB ; i++) {
        tll = listeLabel.at(i);
        for (int num_lecteur=1 ; num_lecteur<MAXLECTEURS ; num_lecteur++) {
            if (tll->tpsSens[num_lecteur])
                //
                if (!tll->tpsSens[num_lecteur]->isActive() && pBdd->getEtatLect(num_lecteur)) {
                    ui->txtAlarme->textCursor().insertText(QString::fromUtf8("<ALARME> Perte de réception du badge ")+ QString("%1").arg(tll->numBadge,0,16));
                    //Historique des événements (log) : perte réception
                    pBdd->setLog(2, i); //2=perte de réception
                    //signal perte de réception
                    emit signalPerteReception(tll->numBadge, num_lecteur, tll);

                    //arrêt du timer de mouvement
                    tll->tpsMouv->stop();
                    //mise à jour état
                    tll->etat |= REC;
                    //perte du badge dans BDD
                    pBdd->setBadgePerdu(tll->numBadge);
                    //mise à jour tableaux pour sens de passage
                    tll->sdp[num_lecteur] = 0;
                    tll->sdpMem[num_lecteur] = 0;
                }
        }
    }
}

/////////////////////
/*** DESTRUCTEUR ***/
Ihm::~Ihm()
{
    //déclaration QList
    QList<T_Badge> listeBadge;

    //récupération des infos sur les onglets
    pBdd->badgeExistant(&listeBadge);

    if(!listeBadge.empty()){
        for(int i = 0; i < listeBadge.count(); i++) {
            int num_badge = listeBadge.at(i).numBadge;

            //mettre badge inactif
            pBdd->setBadgePerdu(num_badge);
        }
    }

    delete pLecteur;        //a supprimer à l'intégration
    delete pDynamique;
    delete pBdd;
    delete ui;
}
//////////////////////////////
/*** SLOT LECTEUR INCONNU ***/
void Ihm::lecteurInconnu(){
    //ajout texte Ihm
    ui->txtAlarme->textCursor().insertText(QString::fromUtf8("<Erreur> Quelque chose a tenté de se connecter\n"));
}
//////////////////////////////
/*** SLOT LECTEUR INACTIF ***/
void Ihm::lecteurInactif(Lecteur *pLecteur){
    //obtenir le numéro de lecteur grâce à la classe Lecteur
    //int numLecteur = pLecteur->getnum_lecteur();
    int numLecteur = 1; //doit disparaitre à l'intégration

    //déclaration QList
    QList<T_TupleLecteurS> listeTupleL;

    pBdd->getVueFctLect(numLecteur, &listeTupleL);

    //récupération des infos dans la liste
    if(!listeTupleL.empty()){
        for(int i = 0; i < listeTupleL.count(); i++) {
            int num_vue = listeTupleL.at(i).num_vue;
            //suppression d'un lecteur (en dynamique)
            this->suppLecteur(numLecteur, num_vue);
            //listeTupleL.removeAt(i);    //A VERIFIER
        }
    }

}
/////////////////////////////////////
/*** méthode SUPPRESSION LECTEUR ***/
void Ihm::suppLecteur(int numLecteur, int num_vue){

    //se placer sur le bon onglet
    QWidget *onglet;
    onglet = pDynamique->onglet[num_vue];
    //test valeur
    //qDebug() << "valeur pointeur onglet" << onglet << endl;

    //se placer sur le bon label du lecteur
    QLabel *labelL;
    labelL = pDynamique->labelL[num_vue][numLecteur];

    //supprimer le label
    labelL->clear();
    delete labelL;

    //message d'avertissement (Alarmes)
    QString numLecteurS = QString::number(numLecteur);
    QString numVueS = QString::number(num_vue);
    QString supLecteur = "<Lecteur ";
    supLecteur += numLecteurS;
    supLecteur += "><Vue ";
    supLecteur += numVueS;
    supLecteur += QString::fromUtf8("> vient de se déconnecter");
    ui->txtAlarme->textCursor().insertText(supLecteur + "\n");

    //mettre à jour la BDD
    int etat = 0;   //état déconnexion
    pBdd->setEtatLect(numLecteur, etat);
}
////////////////////////////
/*** SLOT LECTEUR ACTIF ***/
void Ihm::lecteurActif(Lecteur *pLecteur){
    //obtenir le numéro de lecteur grâce à la classe Lecteur
    //int numLecteur = pLecteur->getnum_lecteur();
    int numLecteur = 3; //doit disparaitre à l'intégration

    //déclaration QList
    QList<T_TupleLecteurE> listeTupleLA;

    pBdd->getVuePosFctLect(numLecteur, &listeTupleLA);

    //récupération des infos dans la liste
    if(!listeTupleLA.empty()){
        for(int i = 0; i < listeTupleLA.count(); i++) {
            int num_vue = listeTupleLA.at(i).num_vue;
            int x = listeTupleLA.at(i).x;
            int y = listeTupleLA.at(i).y;

            //ajout d'un lecteur (en dynamique)
            this->ajoutLecteur(numLecteur, num_vue, x, y);
        }
    }

}
///////////////////////////////
/*** méthode AJOUT LECTEUR ***/
void Ihm::ajoutLecteur(int numLecteur, int num_vue, int x, int y){

    //se placer sur le bon onglet
    QWidget *onglet;
    onglet = pDynamique->onglet[num_vue];
    //test valeur
   // qDebug() << "valeur pointeur onglet" << onglet << endl;

    //nouveau label dynamique pour mettre l'image correspondant
    QLabel *labelL = new QLabel(onglet);
    //différente taille d'images utilisées
    if(num_vue == 1){
        labelL->setPixmap(QPixmap("../ressources/lecteur_actif_petit.jpg"));
    }else{
        labelL->setPixmap(QPixmap("../ressources/lecteur_actif.jpg"));
        }
    labelL->setGeometry(x, y, 15, 42); // largeur hauteur à définir

    //sauvegarde du pointeur du label du lecteur
    pDynamique->labelL[num_vue][numLecteur] = labelL;

    //mettre à jour la BDD
    int etat = 1;   //état connexion
    pBdd->setEtatLect(numLecteur, etat);

}
//////////////////////////////
/*** méthode AJOUT ONGLET ***/
void Ihm::ajoutOnglet(int num_vue, QString legende, QString image)
{
    //nouveau onglet dynamique avec légende
    ContenuOnglet *pContenuOnglet = new ContenuOnglet(0, image);
    ui->tabWidget->insertTab(num_vue, pContenuOnglet, legende);

    //sauvegarde du pointeur onglet
    pDynamique->onglet[num_vue] = pContenuOnglet;
  //  qDebug() << "valeur dans la classe" << pDynamique->onglet[num_vue] << endl;

}


