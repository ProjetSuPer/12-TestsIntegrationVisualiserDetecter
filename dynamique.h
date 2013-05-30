#ifndef DYNAMIQUE_H
#define DYNAMIQUE_H

#include <QObject>
#include <QLabel>

// masque de l'octet d'état
#define MOUV 1
#define REC  2
#define AR 4
#define MOUV0 8  //mouv=0 avant alarme

#define MAXONGLETS 20   //maximum d'onglets possible
#define MAXLECTEURS 40  //maximum de lecteurs possible (NB : 1 lieu = 1 lecteur)
#define MAXBADGES 40    //maximum de badges possible
#define MAXPERSONNES 200    //maximum de personnes possible
#define MAXVAL 20 //nombre de valeur pour faire la moyenne

// structure pour le positionnement X, Y du label badge
typedef struct {
    int x,y;
} T_Point;

// Permet le mise en correspondance d'un QLabel de position avec un badge.
// création dynamique des logos affichés représentant les personnes badgées.
typedef struct s_listeLabel {
    QLabel *labelB[MAXONGLETS][MAXBADGES];      //pointeur sur le label du badge
    QTimer *tpsMouv;                //timer sur le mouvement
    QTimer *tpsSens[MAXLECTEURS];   //timer sur la réception

    int numBadge;           //numéro de badge
    int numLecteur;      //numéro de lecteur lu
    int etat;    // bit0:Mouv(0:RAS, 1:TO)   bit1:REC(0:RAS, 1:TO)  bit2:SENS(0:haut, 1:bas) bit3:MOUV0
    int zone;   // lieu géographique d'affichage du badge
    int moySens[MAXLECTEURS][MAXVAL];   //moyenne réception
    int indMoy[MAXLECTEURS];    //indice moyenne
    int sdp[MAXLECTEURS];       //sens de passage
    int sdpMem[MAXLECTEURS];    //sens de passage mémorisé (sdp -1)

    /*---Personne---*/
    QString nom[MAXPERSONNES];      //sauvegarde identité personne, nom
    QString prenom[MAXPERSONNES];   //prenom
    QString societe[MAXPERSONNES];  //societe
    QString photo[MAXPERSONNES];    //photographie

    /*---Points et coordonnées---*/
    T_Point ptA, ptB, ptBadge[MAXONGLETS];

} T_ListeLabel;


//////////////////////////////////////////////////////////////
/*** CLASSE pour sauvegarder les objets créé dynamiquement***/
class Dynamique : public QObject
{
    Q_OBJECT
public:
    explicit Dynamique(QObject *parent = 0);

    /*---Pointeurs---*/
    QWidget *onglet[MAXONGLETS];    //pointeur sur l'onglet
    //QLabel *labelL[MAXONGLETS][MAXLECTEURS];    //pointeur sur le label du lecteur

    /*---Badge---*/
    bool BadgeActif[MAXBADGES];     //badge actif ou non

    /*---Personne---*/
    QString nom[MAXPERSONNES];      //sauvegarde identité personne, nom
    QString prenom[MAXPERSONNES];   //prenom
    QString societe[MAXPERSONNES];  //societe
    QString photo[MAXPERSONNES];    //photographie
signals:
    
public slots:
    
};

#endif // DYNAMIQUE_H
