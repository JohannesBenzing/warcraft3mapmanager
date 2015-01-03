TEMPLATE = app
QT       += core gui widgets sql
TARGET = wc3mapmanager

SOURCES  += \
    main.cpp \
    CustomWindow.cpp \
    StarRatingDelegate.cpp

HEADERS  += \
    PreventEditingDelegate.h \
    StarRatingDelegate.h \
    CustomWindow.h

FORMS    += \
    CustomWindow.ui

RESOURCES += \
    CustomWindow.qrc

OTHER_FILES +=

