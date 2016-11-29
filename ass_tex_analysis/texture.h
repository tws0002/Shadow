#ifndef TEXTURE_H
#define TEXTURE_H

#include <QObject>

class Texture : public QObject
{
    Q_OBJECT
public:
    explicit Texture(QObject *parent = 0);

    void setAssFile(QString file);
    void setNodeName(QString name);
    void setNodeType(QString type);
    void setFileName(QString name);

    QString assFile();
    QString nodeName();
    QString nodeType();
    QString fileName();

signals:

public slots:

private:
    QString ass_file;
    QString node_name;
    QString node_type;
    QString file_name;
};

#endif // TEXTURE_H
