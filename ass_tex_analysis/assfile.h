#ifndef ASSFILE_H
#define ASSFILE_H

#include <QObject>
#include <QString>
#include "texture.h"

class AssFile : public QObject
{
    Q_OBJECT
public:
    explicit AssFile(QObject *parent = 0);
    AssFile(QString file, QObject *parent = 0);

    void setFile(QString file);
    QString file();

    void load();
    void close();
    void save();
    void saveAs(QString n_file);
    bool isLoaded();

    QList<Texture *> allTextures();
    void updateTextures(QList<Texture *> texs);
    void updateTexture(Texture *tex);

signals:

public slots:

private:
    void loadPlugins();

    QString m_file;
    bool m_loaded;
};

#endif // ASSFILE_H
