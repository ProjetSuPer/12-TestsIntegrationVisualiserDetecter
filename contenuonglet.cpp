#include "contenuonglet.h"

ContenuOnglet::ContenuOnglet(QWidget *parent, QString image) :
    QWidget(parent)
{
    //nouveau label dynamique pour mettre l'image correspondant
    QLabel *labelO = new QLabel(this);
    labelO->setPixmap(QPixmap(image));
}
