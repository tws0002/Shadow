
#include "texturetablemodel.h"

#include <QtCore>
#include <QAbstractTableModel>
#include <QModelIndex>

TextureTableModel::TextureTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    header << QString("Node Name") << QString("Node Type") << QString("File");
}

TextureTableModel::~TextureTableModel()
{

}

int TextureTableModel::rowCount(const QModelIndex &parent) const
{
    if(!textures.isEmpty())
    {
        return textures.size();
    }
    return 0;
}

int TextureTableModel::columnCount(const QModelIndex &parent) const
{
    return header.size();
}

QVariant TextureTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();
    auto item = textures[row];

    if(role == Qt::DisplayRole)
    {
        if(0 == column)
        {
            return item->nodeName();
        }
        else if(1 == column)
        {
            return item->nodeType();
        }
        else if(2 == column)
        {
            return item->fileName();
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

QVariant TextureTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            return header[section];
        }
        else
            return QVariant();
    }
    else
    {
        return QVariant();
    }
}

void TextureTableModel::addTexture(Texture *tex)
{
    beginResetModel();
    textures.append(tex);
    endResetModel();
}

void TextureTableModel::setTextures(QList<Texture *> list)
{
    beginResetModel();
    textures = list;
    endResetModel();
}

void TextureTableModel::clearAllTexture()
{
    beginResetModel();
    while(!textures.isEmpty())
    {
        delete textures.takeFirst();
    }
    endResetModel();
}

QList<Texture *> TextureTableModel::getMany(QList<int> rows)
{
    QList<Texture *> texs;

    for(int i = 0; i < rows.size(); ++i)
    {
        auto row = rows[i];
        texs.append(textures[row]);
    }
    return texs;
}
