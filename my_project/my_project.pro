#-------------------------------------------------
#
# Project created by QtCreator 2018-10-25T22:21:32
#
#-------------------------------------------------

QT       += core gui widgets svg network

TARGET = my_project
TEMPLATE = app
LIBS += "./release/Qt5Network.dll"
LIBS += "./release/Qt5Core.dll"
LIBS += "./release/Qt5Gui.dll"
LIBS += "./release/Qt5Widgets.dll"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    photo_edit.cpp \
    paintscene.cpp \
    photo_paint.cpp \
    insta_parser.cpp \
    instagram_account.cpp \
    photo.cpp

HEADERS += \
        mainwindow.h \
    photo_edit.h \
    paintscene.h \
    photo_paint.h \
    insta_parser.h \
    instagram_account.h \
    photo.h

FORMS += \
        mainwindow.ui \
    photo_edit.ui \
    photo_paint.ui \
    insta_parser.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resourses.qrc
