#include "dlnaservice.h"

#include "dlnaexception.h"

DLNAService::DLNAService(QString cntrlURL, QString scpdURL, QString evntsubURL, QString sType, QString sID)
{

    this->controlURL = cntrlURL;
    this->Scpdurl = scpdURL;
    this->EventSubURL = evntsubURL;
    this->ServiceType = sType;
    this->ServiceID = sID;
}

QString DLNAService::getServiceID() const
{
    return ServiceID;
}

QString DLNAService::getControlURL() const
{
    return controlURL;
}

QMap<QString, DLNAService> ReadService(QString HTML)
{
    QMap<QString, DLNAService> Dic;

    QXmlStreamReader xml(HTML);
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement) {
            QString name = xml.name().toString();
            if(name == "service")
            {
                QString sType, sID, scpdURL, cntrlURL, evntsubURL;
                qDebug() << "Found Service";
                do {
                    xml.readNextStartElement();
                    name = xml.name().toString();
                    qDebug() << "element name: '" << name  << "'"
                             << ", text: '" << xml.readElementText()
                             << "'" << endl;
                    if(name == "serviceType") { sType = xml.readElementText(); }
                    else if(name == "serviceId") { sID = xml.readElementText(); }
                    else if(name == "SCPDURL") { scpdURL = xml.readElementText(); }
                    else if(name == "controlURL") { cntrlURL = xml.readElementText(); }
                    else if(name == "eventSubURL") { evntsubURL = xml.readElementText();}

                } while (name != "service");

                DLNAService s(cntrlURL,scpdURL,evntsubURL,sType,sID);
                Dic.insert(s.getServiceID(),s);
            }
            continue;
        }
    }

    if (xml.hasError())
    {
        qDebug() << "XML error: " << xml.errorString() << endl;
    }
    else if (xml.atEnd())
    {
        qDebug() << "Reached end, done" << endl;
    }

    return Dic;
}
