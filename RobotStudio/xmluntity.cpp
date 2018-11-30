#include "xmluntity.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

XmlUntity::XmlUntity(QString fname,QString rname):filename(fname),rootname(rname)
{
    createXml();
    QString docstr = doc.toString();
//    qDebug() << "XmlUntity" << docstr;
}


bool XmlUntity::createXml()
{
    QFile file(filename);
    if(file.exists())
        file.remove();
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
        return false;
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement(rootname);
    doc.appendChild(root);
    QTextStream out(&file);
    doc.save(out,4);
    QString docstr = doc.toString();
    qDebug() << "createXml " << docstr;
    file.close();
    return true;
}


bool XmlUntity::readXmlDoc()
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "open file error";
        return false;
    }
    if(!doc.setContent(&file)){
        qDebug() << "set content error";
        file.close();
        return false;
    }
    return true;
}

bool XmlUntity::writeXmlDoc()
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
        return false;
    QTextStream out(&file);
    doc.save(out,4);
    file.close();
    return true;
}
