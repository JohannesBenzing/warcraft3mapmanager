QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = warcraft3mapmanager
TEMPLATE = app


SOURCES += main.cpp \
    window.cpp \
    QtWaitingSpinner.cpp \
    StarRatingDelegate.cpp

HEADERS  += \
    window.h \
    QtWaitingSpinner.h \
    StarRatingDelegate.h \
    PreventEditingDelegate.h

FORMS    += \
    window.ui

RC_FILE = exeicon.rc

RESOURCES += \
    myresources.qrc
