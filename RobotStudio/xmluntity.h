#ifndef XMLUNTITY_H
#define XMLUNTITY_H
#include <QDomDocument>

class XmlUntity
{
public:
    XmlUntity(QString fname,QString rname);
    bool createXml();
    bool readXmlDoc();
    bool writeXmlDoc();
public:
    QString filename,rootname;
    QDomDocument doc;
};

#endif // XMLUNTITY_H
