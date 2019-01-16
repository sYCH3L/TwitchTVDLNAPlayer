#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include <QSysInfo>
#include <QTcpSocket>
#include <QtXml>
#include <QStringList>
#include "dlnaexception.h"

namespace HelperDLNA {

static QString MakeRequest(QString Methord, QString Url, int ContentLength, QString SOAPAction, QString IP, QString Port)
{//Make a request that is sent out to the DLNA server on the LAN using TCP
    QString R = Methord.toUpper() + " /" + Url + " HTTP/1.1" + "\n";
    R += "Cache-Control: no-cache\n";
    R += "Connection: Close\n";
    R += "Pragma: no-cache\n";
    R += "Host: " + IP + ":" + Port + "\n";
    R += "User-Agent: Microsoft-Windows/6.3 UPnP/1.0 Microsoft-DLNA DLNADOC/1.50\n";
    R += "FriendlyName.DLNA.ORG: " + QSysInfo::machineHostName() + "\n";
    if (ContentLength > 0)
    {
        R += "Content-Length: " + QString::number(ContentLength) + "\n";
        R += "Content-Type: text/xml; charset=\"utf-8\"\n";
    }
    if (SOAPAction.length() > 0)
        R += "SOAPAction: \"" + SOAPAction + "\"\n";
    R += "\n";
    return R;
}

static QString makeSocketGetReply(QString ip, QString port, QString data)
{
    QTcpSocket sock;

    sock.connectToHost(ip,port.toUShort());

    if(sock.waitForConnected())
    {
        sock.write(data.toLatin1());
    }
    else {
        throw DLNAException("Couldn't connect!");
    }

    if(!sock.waitForBytesWritten())
    {
        throw DLNAException("Couldn't write");
    }

    if(sock.waitForReadyRead())
    {
        return QString::fromUtf8(sock.readAll());
    }
    else {
        throw DLNAException("Couldn't get an answer!");
    }
}

static QString getTag(QString tag, QString data)
{
    QXmlStreamReader xml(data);
    /* We'll parse the XML until we reach end of it.*/
    while(!xml.atEnd() && !xml.hasError())
    {
        /* Read next element.*/
        QXmlStreamReader::TokenType token = xml.readNext();
        /* If token is just StartDocument, we'll go to next.*/
        if(token == QXmlStreamReader::StartDocument)
            continue;

        /* If token is StartElement, we'll see if we can read it.*/
        if(token == QXmlStreamReader::StartElement) {
            QString name = xml.name().toString();
            if(name == tag)
            {
                return xml.readElementText();
            }
            continue;
        }
    }

    if (xml.hasError())
    {
        throw DLNAException(xml.errorString());
    }
    else if (xml.atEnd())
    {
        return "";
    }
    return "";
}

static int GetResponseCode(QString HTML)
{

    QStringList l = HTML.split(" ");

    bool ok = false;

    int returnCode = l[1].toInt(&ok);

    if(ok)
    {
         return returnCode;
    }

    return -1;
}

static QString removeHttpHeader(QString HTML)
{
    return HTML.split("\r\n\r\n")[1];
}


}




#endif // HELPERS_H
