QT       += core websockets concurrent
QT       -= gui

TARGET = MWMonitor

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

OBJECTS_DIR = ./obj
MOC_DIR     = ./moc

SOURCES += \
    main.cpp \
    WebSockServer.cpp \
    ConfigMgr.cpp \
    RequestHandler.cpp \
    MWMonitor.cpp

HEADERS += \
    WebSockServer.h \
    ConfigMgr.h \
    RequestHandler.h \
    MWMonitor.h

EXAMPLE_FILES += WebServerClient.html

DISTFILES += \
    WebServerClient.html \
    Visa4EncoderConfigOption.json \
    MWMonitor.ini

include(../QtWebApp/QtWebApp/httpserver/httpserver.pri)
