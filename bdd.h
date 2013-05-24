#ifndef BDD_H
#define BDD_H

#include <QObject>
#include <QtSql>
#include <QList>
#include <QMessageBox>

#include "dynamique.h"

//////////////////////////////////////////
/*** STRUCTURE pour retour de fonction***/
//////////////////////////////////////////
//identité personne
typedef struct s_Personne {
    QString nom;      //sauvegarde identité personne, nom
    QString prenom;   //prenom
    QString societe;  //societe
    QString photo;    //photographie

} T_Personne;
//enregistrement d'un onglet
typedef struct s_TupleOnglet {
    int num_vue;
    QString legende;
    QString image;

} T_TupleOnglet;
//enregistrement d'un lecteur qui veut se déconnecter
typedef struct s_TupleLecteurS {
    int num_vue;
} T_TupleLecteurS;

//enregistrement d'un lecteur qui se connecte
typedef struct s_TupleLecteurE {
    int num_vue;
    int x;
    int y;
} T_TupleLecteurE;

//enregistrement des positions d'un lieu par rapport à une vue
typedef struct s_TuplePositionLieu {
    int xA;
    int yA;
    int xB;
    int yB;
} T_TuplePositionLieu;

//fichier log
typedef struct s_Log {
    QString date;
    QString legende;
    QString nom;
    QString prenom;
    QString societe;
} T_Log;

//badge existant
typedef struct s_Badge {
    int numBadge;
} T_Badge;

/////////////////////////////////////////////
/*** CLASSE pour gérer la Base de Données***/
class Bdd : public QObject
{
    Q_OBJECT
public:
    explicit Bdd(QObject *parent = 0);
    ~Bdd();

    int getVueMax();    //obtenir vue max
    bool getVueFctLect(int numLecteur, QList<T_TupleLecteurS> *); //obtenir la vue en fonction du lecteur
    bool getVuePosFctLect(int numLecteur, QList<T_TupleLecteurE> *);   //obtenir la vue et la position (x,y) en fonction du lecteur
    bool getVue(QList<T_TupleOnglet>*);   //obtenir toutes les vues
    bool badgeExiste(QString &num_badge);   //est-ce que ce badge existe ?
    int badgeIdentite(int num_badge_i, QList<T_Personne> *listePersonne);    //a qui appartient ce badge ?
  //  bool getPositionLieu(int num_vue, int num_lecteur, QList<T_TuplePositionLieu> *); //obtenir (xA,yA) et (xB,yB) en fonction du lieu et de la vue

    void setEtatLect(int numLecteur, bool etat);    //connexion/déconnexion lecteur
    bool getEtatLect(int numLecteur);                   //état lecteur (connecté ou non)

    int getSensMonter(int numLecteur);     //obtenir le type de zone
    bool getPointsZone(int vue, int zone, T_Point *pointA, T_Point *pointB);  //obtenir les points de la zone

    //perte badge (réception)
    void setBadgeActif(int numBadge);    //mettre badge actif
    void setBadgePerdu(int numBadge);   //mettre badge perdu
    bool badgeExistant(QList<T_Badge> *);   //obtenir les badges actifs

    //gestion log
    void setLog(int typeLog, int numBadge);  //mettre à jour historique des événements

    //configuration SuPer
    void getTempo(int *, int *);


private:
    //pointeurs sur structure
    T_TupleOnglet * pTupleOnglet;
    T_TupleLecteurS * pTupleLecteurS;
    T_TupleLecteurE * pTupleLecteurE;
    T_TuplePositionLieu * pTuplePositionLieu;
    T_Personne * pPersonne;
    T_Log * pLog;
    T_Badge * pBadge;

    //pointeur sur la classe Dynamique (sauvegarde de l'affichage dynamique)
    Dynamique *pDynamique;

    QSqlQuery * query;
    QSqlDatabase  database;
    QString requete;  // requête parfois utilisé
    
signals:
    
public slots:
    
};

#endif // BDD_H
