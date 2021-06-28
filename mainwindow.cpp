#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStyleFactory>
#include <QScrollBar>
#include <QDebug>
#include <QTimer>
#include <QShortcut>
#include <fstream>
#include <QFile>
#include <QDir>
#include <unistd.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

#include "x11platformwindow.h"

void pressKey(int keycode, int modifiers, Window winFocus);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("");
    this->setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setFixedSize(this->size());

    loadAndSetSettings();

    clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::clipboardChanged);

    QString shct = settings.value("shortcut", "alt+h").toString();

    shortcut = new QHotkey(QKeySequence(shct), true, this);
    connect(shortcut, &QHotkey::activated, qApp, [this]()
    {
        this->raise();
        this->activateWindow();
        this->show();
    });

    QShortcut * enterShortcut = new QShortcut(QKeySequence("return"), this);
    connect(enterShortcut, &QShortcut::activated, qApp, [this]()
    {
        if(ui->listWidget->count() > 0 )
        {
            if(ui->listWidget->selectedItems().count() > 0)
                setTextToClipboard(ui->listWidget->selectedItems().at(0));
            else
                setTextToClipboard(ui->listWidget->item(0));
        }
        else
            this->hide();
    });

    connect(qApp, &QApplication::focusChanged, [this]
    {
        if(! this->isActiveWindow())
        {
            if(shouldHide)
                this->hide();
        }
    });


    connect(&setShortcut, &SetShortcut::shortcutSet, [this] (QKeySequence keySequece)
    {
        shortcut->setShortcut(keySequece, true);
        settings.setValue("shortcut", keySequece.toString());
    });

    isAddingBlocked = false;
    shouldHide =      true;

    setTheme();

    setupList();

    updateFocusWindows();
    QTimer * timer = new QTimer;
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFocusWindows);
    timer->start(2000);

    ui->label->hide();
    ui->pushButton->hide();

    firstTimeUser();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupList()
{
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::setTextToClipboard);

    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->setStyleSheet("QListWidget::item {height: 50px; background-color: #606060; }"
                                  "QListWidget {background-color: rgb(22,22,24);}"
                                  "QListWidget::item:selected{ background-color: #3498DB }");
    ui->listWidget->setAlternatingRowColors(true);
    ui->listWidget->setWrapping(false);
    ui->listWidget->setSpacing(3);
    ui->listWidget->horizontalScrollBar()->hide();
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::popupMenuTableShow);
}

void MainWindow::setTextToClipboard(QListWidgetItem *item)
{
    QListWidgetItem* it = item;
    isAddingBlocked = true;
    if(ui->listWidget->row(item) != 0)
    {
        it = ui->listWidget->takeItem(ui->listWidget->row(item));
        ui->listWidget->insertItem(0, it);
    }

    if(item->data(Qt::DecorationRole).isNull())
        clipboard->setText(it->text());
    else
        clipboard->setPixmap(it->icon().pixmap(1920, 1080));

    this->hide();

    X11PlatformWindow window(lastWindow);

    window.raise();

    isAddingBlocked = false;
    window.pasteClipboard();
}

void MainWindow::firstTimeUser()
{
    bool firstTime = settings.value("first-time", "true").toBool();

    if(firstTime)
    {
        shouldHide = false;

        ui->label->show();
        ui->pushButton->show();
        ui->listWidget->hide();

        ui->label->setText("clipH\n\n"
                           "alt+h - Show clipH\n"
                           "Press Enter to copy the item on top");
        ui->label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        ui->label->setStyleSheet("font: 12pt;");
    }

    settings.setValue("first-time", "false");
}

void MainWindow::on_pushButton_clicked()
{
    shouldHide = true;

    ui->label->hide();
    ui->pushButton->hide();
    ui->listWidget->show();

    this->hide();
}

void MainWindow::clipboardChanged()
{
    if( ! isAddingBlocked)
    {
        const QMimeData* mimeData = clipboard->mimeData();

        if(mimeData->hasText())
        {
            addText(mimeData->text());
        }
        else if(mimeData->hasImage())
            addImage(qvariant_cast<QImage>(mimeData->imageData()));
    }
}

void MainWindow::addText(QString txt)
{
    QListWidgetItem * item = new QListWidgetItem(txt);
    item->setTextAlignment(Qt::AlignVCenter);

    ui->listWidget->insertItem(0, item);
}

void MainWindow::addImage(QImage image)
{
    QListWidgetItem * item = new QListWidgetItem();
    item->setIcon(QIcon(QPixmap::fromImage(image)));

    ui->listWidget->insertItem(0, item);
}

void MainWindow::popupMenuTableShow(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget->itemAt(pos);

    if(item != nullptr)
    {
        QMenu contextMenu;

        QAction* remove = new QAction(tr("Remove"), this);

        connect(remove, &QAction::triggered, [this, item]()
        {
            ui->listWidget->takeItem(ui->listWidget->row(item));
        });

        QAction* clear = new QAction(tr("Clear"), this);

        connect(clear, &QAction::triggered, [this, item]()
        {
            ui->listWidget->clear();
        });

        contextMenu.addAction(remove);
        contextMenu.addAction(clear);

        contextMenu.exec(QCursor::pos());
    }
}

void MainWindow::setTheme(/*Preferences::Theme t*/)
{
    qApp->setPalette(darkTheme());
    qApp->setStyle(QStyleFactory::create("fusion"));
}

void MainWindow::on_actionSet_Show_Shortcut_triggered()
{
    shouldHide = false;

    setShortcut.showCurrentShortcut(shortcut->keyCode() | shortcut->modifiers());
    setShortcut.exec();

    shouldHide = true;
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::updateFocusWindows()
{
    if ( QApplication::activeWindow() )
        return;

    lastWindow = getCurrentWindow();
}

void MainWindow::loadAndSetSettings()
{
    bool autoStart   =  settings.value("auto_star",    "true").toBool();
    bool startHidden =  settings.value("start_hidden", "false").toBool();

    ui->actionAuto_start->setChecked(autoStart);
    ui->actionStart_Hidden->setChecked(startHidden);

    on_actionAuto_start_triggered();
}

void MainWindow::on_actionAuto_start_triggered()
{
    QString path = QDir::homePath() + "/.config/autostart/";
    QString name = QCoreApplication::applicationName() +".desktop";
    QFile file(path+name);

    file.remove();

    if(ui->actionAuto_start->isChecked())
    {
        QDir dir(path);
        if(!dir.exists())
        {
            dir.mkpath(path);
        }

        if (file.open(QIODevice::ReadWrite))
        {
            QTextStream stream(&file);
            stream << "[Desktop Entry]" << Qt::endl;
            stream << "Name=clipH" << Qt::endl;
            stream << "Icon=clipH" << Qt::endl;
            stream << "Type=Application" << Qt::endl;
            stream << "Exec=" << QCoreApplication::applicationFilePath() << Qt::endl;
        }

        settings.setValue("auto_start", "true");
    }
    else
        settings.setValue("auto_start", "false");
}

void MainWindow::on_actionStart_Hidden_triggered()
{
    if(ui->actionStart_Hidden->isChecked())
        settings.setValue("start_hidden", "true");
    else
        settings.setValue("start_hidden", "false");
}
