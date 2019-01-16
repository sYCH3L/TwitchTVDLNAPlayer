#ifndef DLNACLIENT_H
#define DLNACLIENT_H

#include <QString>
#include <QDebug>
#include <QRegularExpression>
#include <QStringList>
#include <QTcpSocket>
#include <QDateTime>

#include "dlnaservice.h"

class DLNAClient
{
public:
    DLNAClient(QString data);
    QString GetPosition();
    bool isConnected();
    QString Desc();
    QString TryToPlayFile(QString UrlToPlay);
    QString UploadFileToPlay(QString ControlURL, QString UrlToPlay);
    QString StartPlay(QString ControlURL, int Instance);
    QString StopPlay(bool ClearQueue);
    QString StopPlay(QString ControlURL, int Instance);
    bool AddToQueue(QString UrlToPlay, bool &NewTrackPlaying);
    int TotalSeconds(QString Value);
    int PlayNextQueue(bool Force);
    int PlayPreviousQueue();
    QString Pause(QString ControlURL, int Instance);
private:
    QString ip;
    QString SMP;
    bool connected;
    QString StrPort;
    QString ControlURL;
    QString XMLHead = "<?xml version=\"1.0\"?>\n<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n<SOAP-ENV:Body>\n";
    QString XMLFoot = "</SOAP-ENV:Body>\n</SOAP-ENV:Envelope>\n";
    int NoPlayCount = 0;
    int PlayListPointer = 0;
    QMap<int,QString> PlayListQueue;
    int ReturnCode = 0;
    QString Location = "";
    QString Server = "";
    QString USN = "";
    QString ST = "";
    QString HTML = "";
    QString FriendlyName = "";
    QMap<QString, DLNAService> Services;


};

#endif // DLNACLIENT_H
