# -------------------------------------------------
# Project created by QtCreator 2013-03-28T15:57:49
# -------------------------------------------------
QT += core \
    gui \
    network \
    sql
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
TARGET = MonoThread
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    server.cpp \
    tcpserver.cpp \
    thread.cpp \
    tcpsocket.cpp \
    clientconnection.cpp \
    reader.cpp \
    clientconnectionwindow.cpp
HEADERS += mainwindow.h \
    server.h \
    tcpserver.h \
    thread.h \
    tcpsocket.h \
    clientconnection.h \
    reader.h \
    clientconnectionwindow.h \
    bddConfig.h
FORMS += mainwindow.ui \
    clientconnectionwindow.ui
