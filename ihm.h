#ifndef IHM_H
#define IHM_H

#include <QMainWindow>

#include "dynamique.h"
#include "bdd.h"
#include "server.h"
#include "clientconnection.h"
#include "affichealarme.h"

////////////////////////////////
namespace Ui {
class Ihm;
}

class Ihm : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Ihm(Server* server, QWidget *parent = 0);
    ~Ihm();


private:
    Ui::Ihm *ui;

    //Ihm dynamique (initialisation + visualisation lecteur)
    void ajoutOnglet(int num_vue, QString legende, QString image);
    void ajoutLecteur(int numLecteur, int num_vue, int x, int y, ClientConnection *cCL);
    void suppLecteur(int numLecteur, int num_vue);

    //Init nouveau badge
    void ajoutBadge(int numLecteur, int num_vue);

    /*---Config de Super---*/
    int tempoM; // ms tempo pour le timer mouvement
    int tempoR; // ms tempo pour le timer de rÃ©ception
    void setTempo(int, int);

    QList<T_ListeLabel *> listeLabel;

    int calculerMoyenne(T_ListeLabel *tll); //calculer moyenne sensibilitÃ©
    bool sensDePassage(T_ListeLabel *tll);  //sens de passage
    void calculerDroite(int sens, T_Point pointA, T_Point pointB, T_Point *pointF);    //calcul position exacte

    //pointeur sur la classe Dynamique (sauvegarde de l'affichage dynamique)
    Dynamique *pDynamique;
    //pointeur sur la classe BDD
    Bdd *pBdd;
    Server* _server;

signals:
    void signalHommeEnDanger(QString &);    //alarme homme en danger
    void signalPerteReception(int numBadge, int numLecteur, T_ListeLabel *);    //perte de rÃ©ception

public slots:
    //liÃ© Ã  la connexion/dÃ©connexion des lecteurs
    void lecteurActif(Reader Lecteur);
    void lecteurInactif(int);
    void lecteurInconnu(QString);

    //traitement de la trame
    bool traitementTrame(QString trame);

    //timer mouvement (homme en danger)
    void timerMouv();
    //timer rÃ©ception (perte de badge)
    void timerRec();

private slots:
    //homme en danger (alarme)
    void hommeEnDanger(QString &);
    //perte de rÃ©ception
    void perteReception(int, int, T_ListeLabel *);

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

#endif // IHM_H
