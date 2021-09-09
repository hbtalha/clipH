#ifndef SETSHORTCUT_H
#define SETSHORTCUT_H

#include <QDialog>

namespace Ui
{
class SetShortcut;
}

class SetShortcut : public QDialog
{
    Q_OBJECT

public:
    explicit SetShortcut(QWidget *parent = nullptr);
    ~SetShortcut();

    void showCurrentShortcut(QKeySequence keySequence);

signals:
    void shortcutSet(QKeySequence);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SetShortcut *ui;
};

#endif // SETSHORTCUT_H
