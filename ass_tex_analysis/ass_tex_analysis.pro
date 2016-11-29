#-------------------------------------------------
#
# Project created by QtCreator 2016-11-25T10:39:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ass_tex_analysis
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    texturetablemodel.cpp \
    texture.cpp \
    texturetable.cpp \
    assfile.cpp

HEADERS  += mainwindow.h \
    texturetablemodel.h \
    texture.h \
    texturetable.h \
    assfile.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Arnold-4.2.15.1-windows/lib/ -lai
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Arnold-4.2.15.1-windows/lib/ -lai

INCLUDEPATH += $$PWD/../../Arnold-4.2.15.1-windows/include
DEPENDPATH += $$PWD/../../Arnold-4.2.15.1-windows/include
