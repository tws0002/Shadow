#include "assfile.h"
#include "ai.h"
#include <QDir>

AssFile::AssFile(QObject *parent) : QObject(parent)
{
    m_loaded = false;
}

AssFile::AssFile(QString file, QObject *parent) : QObject(parent)
{
    m_file = file;
}

void AssFile::setFile(QString file)
{
    m_file = file;
}

QString AssFile::file()
{
    return m_file;
}

void AssFile::load()
{
    if(!m_file.isEmpty())
    {
        AiBegin();
        loadPlugins();
        AiASSLoad(m_file.toLatin1());
        m_loaded = true;
    }
    else
    {
        m_loaded = false;
    }
}

void AssFile::close()
{
    if(m_loaded)
    {
        AiEnd();
        m_loaded = false;
    }
}

void AssFile::loadPlugins()
{
    auto current_path = QDir::currentPath();
    current_path += QString("\\plugin");
    AiLoadPlugins(current_path.replace("/", "\\").toLatin1());
}

bool AssFile::isLoaded()
{
    return m_loaded;
}

void AssFile::save()
{
    AiASSWrite(m_file.toLatin1());
}

void AssFile::saveAs(QString n_file)
{
    if(!n_file.isEmpty())
    {
        AiASSWrite(n_file.toLatin1());
    }
}

QList<Texture *> AssFile::allTextures()
{
    QList<Texture *> texs;

    if(m_loaded)
    {
        auto it = AiUniverseGetNodeIterator(AI_NODE_SHADER);
        while(!AiNodeIteratorFinished(it))
        {
            auto node = AiNodeIteratorGetNext(it);
            if(AiNodeIs(node, "image"))
            {
                Texture *tex = new Texture();
                tex->setAssFile(m_file);
                tex->setNodeName(AiNodeGetStr(node, "name"));
                tex->setNodeType("image");
                tex->setFileName(AiNodeGetStr(node, "filename"));
                texs.append(tex);
            }
            else if(AiNodeIs(node, "MayaFile"))
            {
                Texture *tex = new Texture();
                tex->setAssFile(m_file);
                tex->setNodeName(AiNodeGetStr(node, "name"));
                tex->setNodeType("MayaFile");
                tex->setFileName(AiNodeGetStr(node, "filename"));
                texs.append(tex);
            }
        }
    }
    return texs;
}

void AssFile::updateTextures(QList<Texture *> texs)
{
    for(int i = 0; i < texs.size(); ++i)
    {
        auto node = AiNodeLookUpByName(texs[i]->nodeName().toLatin1());
        AiNodeSetStr(node, "filename", texs[i]->fileName().toLatin1());
    }
}

void AssFile::updateTexture(Texture *tex)
{
    auto node = AiNodeLookUpByName(tex->nodeName().toLatin1());
    AiNodeSetStr(node, "filename", tex->fileName().toLatin1());
}

