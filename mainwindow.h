#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include <QMimeData>
#include <QPalette>
#include <QImage>
#include <QProcess>
#include <QListWidgetItem>
#include <QSettings>

//#include "x11platformwindow.h"
#include "setshortcut.h"
#include "qhotkey.h"


static QPalette darkTheme()
{
    QPalette darkPalette;
    QColor darkColor = QColor(22,22,24);
    QColor disabledColor = QColor(127,127,127);

    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(30,30,30));
    darkPalette.setColor(QPalette::AlternateBase, QColor(40,40,40));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

    return darkPalette;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void clipboardChanged();
    void popupMenuTableShow(const QPoint &pos);

    void on_actionQuit_triggered();

    void on_actionSet_Show_Shortcut_triggered();

    void updateFocusWindows();
    void on_actionAuto_start_triggered();

    void on_pushButton_clicked();

    void on_actionStart_Hidden_triggered();

private:
    Ui::MainWindow *ui;

    void setTheme(/*Preferences::Theme t*/);
    void setupList();
    void addText(QString txt);
    void addImage(QImage image);
    void setTextToClipboard(QListWidgetItem* item);
    void firstTimeUser();
    void loadAndSetSettings();


    QClipboard* clipboard;
    QProcess* process;
    QSettings settings;

    QHotkey* shortcut;

    SetShortcut setShortcut;

    bool isAddingBlocked;
    bool shouldHide;
    unsigned long lastWindow;
};
#endif // MAINWINDOW_H
