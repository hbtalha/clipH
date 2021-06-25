#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QMessageBox>


#include "singleinstance.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("HBatalha");
    QCoreApplication::setApplicationName("clipH");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    SingleInstance app("cliphsingleinstanceuniquekey");

    if(! app.run())
    {
        QMessageBox errorMessage;

        QApplication::beep();

        errorMessage.addButton(QMessageBox::Ok);
        errorMessage.setWindowTitle(QObject::tr("Error"));

        QSettings settings;
        QString shortcut = settings.value("shortcut", "Alt+H").toString();

        errorMessage.setText(QObject::tr("clipH already running"));
        errorMessage.setInformativeText(QObject::tr("Press ") + shortcut + QObject::tr(" to show"));

        errorMessage.setStyleSheet("QLabel{min-width: 400px;}");

        errorMessage.exec();

        exit(0);
    }
    else
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
}
