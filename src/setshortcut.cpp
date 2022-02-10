#include "setshortcut.h"
#include "ui_setshortcut.h"

#include "customkeysequenceedit.h"

SetShortcut::SetShortcut(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetShortcut)
{
    ui->setupUi(this);

    this->setFixedSize(this->size());
}

SetShortcut::~SetShortcut()
{
    delete ui;
}

void SetShortcut::showCurrentShortcut(QKeySequence keySequence)
{
    ui->label->setText(QString(tr("Current: ") + keySequence.toString()));
    ui->keySequenceEdit->clear();
}

void SetShortcut::on_buttonBox_accepted()
{
    if( ! ui->keySequenceEdit->keySequence().isEmpty() )
        emit shortcutSet(ui->keySequenceEdit->keySequence());
}

