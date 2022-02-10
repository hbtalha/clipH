#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QMessageBox>
#include <X11/Xlib.h>


#include "singleinstance.h"

static int xerr;
static int ErrorHandler (Display *dpy, XErrorEvent *event)
{
    qDebug() << "ERRORENTER";
    // we don't really care about the error
    // let's hope for the best
    if(event)
    {
        xerr=event->error_code;

        if ( xerr != BadWindow )
        {
            char buf[256];
            XGetErrorText (dpy, xerr, buf, sizeof(buf));
            qDebug () << "Xwin: %s\n" << buf;
        }
        else
        {
            qDebug () << "Xwin: BadWindow (%d)\n" << xerr;
        }
    }
    return (0);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    XSetErrorHandler(ErrorHandler);


    QCoreApplication::setOrganizationName("HBatalha");
    QCoreApplication::setApplicationName("clipH");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    SingleInstance app("cliphsingleinstanceuniquekey");

    if(! app.run())
    {
        QMessageBox errorMessage;

        QApplication::beep();

        errorMessage.addButton(QMessageBox::Ok);
        errorMessage.setWindowTitle(QObject::tr("Error"));

        QString shortcut = settings.value("shortcut", "Alt+H").toString();

        errorMessage.setText(QObject::tr("clipH already running"));
        errorMessage.setInformativeText(QObject::tr("Press ") + shortcut + QObject::tr(" to show"));

        errorMessage.setStyleSheet("QLabel{min-width: 400px;}");

        errorMessage.exec();

        exit(0);
    }
    else
    {
        bool firstTime   =  settings.value("first-time",        "true").toBool();
        bool startHidden =  settings.value("start_hidden",  "false").toBool();

        MainWindow w;

        if(firstTime || ! startHidden)
            w.show();

        return a.exec();
    }
}
