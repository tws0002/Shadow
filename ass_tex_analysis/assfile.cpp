#include "assfile.h"
#include "ai.h"
#include <QDir>

AssFile::AssFile(QObject *parent) : QObject(parent)
{

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

void AssFile::aiBegin()
{
    AiBegin();
    auto current_path = QDir::currentPath();
    current_path += QString("\\plugin");
    AiLoadPlugins(current_path.replace("/", "\\").toLatin1());
    AiASSLoad(m_file.toLatin1());
}

void AssFile::aiEnd()
{
    AiEnd();
}

QList<Texture *> AssFile::allTextures()
{
    QList<Texture *> texs;

    if(!m_file.isEmpty())
    {
        aiBegin();

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
        aiEnd();
    }
    return texs;
}
