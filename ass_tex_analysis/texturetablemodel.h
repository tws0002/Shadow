#ifndef TEXTURETABLEMODEL_H
#define TEXTURETABLEMODEL_H

#include <QAbstractTableModel>

class QModelIndex;
class QVariant;

class TextureTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TextureTableModel(QObject *parent = 0);
    ~TextureTableModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // TEXTURETABLEMODEL_H
