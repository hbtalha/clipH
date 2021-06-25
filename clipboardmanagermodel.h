#ifndef CLIPBOARDMANAGERMODEL_H
#define CLIPBOARDMANAGERMODEL_H

#include <QAbstractTableModel>

class ClipboardManagerModel : public QAbstractTableModel
{
public:
    ClipboardManagerModel();

    void add(QMimeData data);
};

#endif // CLIPBOARDMANAGERMODEL_H
