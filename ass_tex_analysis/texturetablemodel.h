#ifndef TEXTURETABLEMODEL_H
#define TEXTURETABLEMODEL_H

#include <QAbstractTableModel>
#include "texture.h"
#include "assfile.h"

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
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setAssFile(AssFile *file);
    void addTexture(Texture *tex);
    void setTextures(QList<Texture *> list);
    void clearAllTexture();

    QList<Texture *> getMany(QList<int> rows);
    void setPath(QString path, QList<int> rows);
    void setFile(QString file, QList<int> rows);

private:
    QStringList header;
    QList<Texture *> textures;
    AssFile *ass_file;
};

#endif // TEXTURETABLEMODEL_H
