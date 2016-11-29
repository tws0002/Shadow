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

    void aiBegin();
    void aiEnd();
    QList<Texture *> allTextures();

signals:

public slots:

private:
    QString m_file;
};

#endif // ASSFILE_H
