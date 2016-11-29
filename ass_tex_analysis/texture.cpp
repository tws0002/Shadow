#include "texture.h"

Texture::Texture(QObject *parent) : QObject(parent)
{

}

void Texture::setAssFile(QString file)
{
    ass_file = file;
}

void Texture::setNodeName(QString name)
{
    node_name = name;
}

void Texture::setNodeType(QString type)
{
    node_type = type;
}

void Texture::setFileName(QString name)
{
    file_name = name;
}

QString Texture::assFile()
{
    return ass_file;
}

QString Texture::nodeName()
{
    return node_name;
}

QString Texture::nodeType()
{
    return node_type;
}

QString Texture::fileName()
{
    return file_name;
}
