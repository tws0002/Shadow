
#include "texturetablemodel.h"

#include <QtCore>
#include <QAbstractTableModel>
#include <QModelIndex>

TextureTableModel::TextureTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

TextureTableModel::~TextureTableModel()
{

}

int TextureTableModel::rowCount(const QModelIndex &parent) const
{
    return 3;
}

int TextureTableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TextureTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }

    if(role == Qt::DisplayRole)
    {
        return QString("aaa");
    }

    return QVariant();
}
