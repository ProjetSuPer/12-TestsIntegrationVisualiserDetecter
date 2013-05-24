#-------------------------------------------------
#
# Project created by QtCreator 2013-02-12T11:22:20
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SuPer_SCHERER
TEMPLATE = app


SOURCES += main.cpp\
        ihm.cpp \
    lecteur.cpp \
    contenuonglet.cpp \
    bdd.cpp \
    dynamique.cpp

HEADERS  += ihm.h \
    lecteur.h \
    contenuonglet.h \
    dynamique.h \
    bdd.h

FORMS    += ihm.ui
