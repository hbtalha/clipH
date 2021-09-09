#include "customkeysequenceedit.h"

#include <QKeyEvent>

CustomKeySequenceEdit::CustomKeySequenceEdit(QWidget *parent) : QKeySequenceEdit(parent) { }

CustomKeySequenceEdit::~CustomKeySequenceEdit() { }

void CustomKeySequenceEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Backspace)
    {
        this->clear();
    }
    else
    {
        QKeySequenceEdit::keyPressEvent(event);
        QString strKeySequence = keySequence().toString().split(",").first();
        QKeySequence seq(QKeySequence::fromString(strKeySequence));
        setKeySequence(seq);
    }
}
