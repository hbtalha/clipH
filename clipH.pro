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
    src/customkeysequenceedit.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/qhotkey.cpp \
    src/qhotkey_x11.cpp \
    src/setshortcut.cpp \
    src/singleinstance.cpp \
    src/x11platformwindow.cpp


HEADERS += \
    src/clipboardmanagermodel.h \
    src/customkeysequenceedit.h \
    src/mainwindow.h \
    src/platformwindow.h \
    src/qhotkey.h \
    src/qhotkey_p.h \
    src/setshortcut.h \
    src/singleinstance.h \
    src/sleeptimer.h \
    src/x11platformwindow.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    src/mainwindow.ui \
    src/setshortcut.ui

DISTFILES +=

RESOURCES +=

