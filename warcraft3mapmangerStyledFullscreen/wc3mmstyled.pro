TEMPLATE = app
QT       += core gui widgets sql
TARGET = wc3mapmanager

SOURCES  += \
    main.cpp \
    ModelSqlite.cpp \
    ViewCustomWindow.cpp \
    DelegateStarRating.cpp

HEADERS  += \
    ModelSqlite.h \
    DelegateStarRating.h \
    DelegatePreventEditing.h \
    ViewCustomWindow.h

FORMS    += \
    CustomWindow.ui

RESOURCES += \
    CustomWindow.qrc

OTHER_FILES +=

