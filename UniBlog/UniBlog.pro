#-------------------------------------------------
#
# Project created by QtCreator 2021-06-04T14:37:42
#
#-------------------------------------------------

QT       += core gui network sql
RC_ICONS = uniblog2.ico
DEFINES += _AMD64_

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UniBlog
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/basewidget/basewidget.pri)
include($$PWD/blogtext/blogtext.pri)
include($$PWD/event/event.pri)
include($$PWD/blogSystem/blogSystem.pri)
include($$PWD/SqlDatabase/SqlDatabase.pri)
include($$PWD/File/File.pri)
include($$PWD/user/user.pri)
SOURCES += \
        main.cpp \
        bloglogin.cpp \
    blogfunction.cpp \
    myapp.cpp \
    mysocket.cpp \
    threadworker.cpp \
    blogregister.cpp

HEADERS += \
        bloglogin.h \
    blogfunction.h \
    myapp.h \
    mysocket.h \
    threadworker.h \
    blogregister.h

FORMS += \
        bloglogin.ui \
    blogfunction.ui \
    blogregister.ui

RESOURCES += \
    resource.qrc
