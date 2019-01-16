#ifndef DLNASERVICE_H
#define DLNASERVICE_H

#include <QString>
#include <QMap>
#include <QtXML>

#include "helpers.h"

class DLNAService
{
public:
    DLNAService(QString cntrlURL, QString scpdURL, QString evntsubURL, QString sType, QString sID);
    QMap<QString, DLNAService> ReadService(QString HTML);

    QString getServiceID() const;

    QString getControlURL() const;

private:
    QString controlURL = "";
    QString Scpdurl = "";
    QString EventSubURL = "";
    QString ServiceType = "";
    QString ServiceID = "";
};

#endif // DLNASERVICE_H
