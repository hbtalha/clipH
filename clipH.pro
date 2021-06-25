QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


unix:{

    QT       += x11extras
    LIBS += -lX11

    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=

    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib\'"
}

SOURCES += \
    customkeysequenceedit.cpp \
    main.cpp \
    mainwindow.cpp \
    qhotkey.cpp \
    qhotkey_x11.cpp \
    setshortcut.cpp \
    singleinstance.cpp \
    x11platformwindow.cpp

HEADERS += \
    clipboardmanagermodel.h \
    customkeysequenceedit.h \
    mainwindow.h \
    platformwindow.h \
    qhotkey.h \
    qhotkey_p.h \
    setshortcut.h \
    singleinstance.h \
    sleeptimer.h \
    x11platformwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    mainwindow.ui \
    setshortcut.ui

DISTFILES +=

RESOURCES +=

