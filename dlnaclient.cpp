#include "dlnaclient.h"

#include "dlnaexception.h"
#include "helpers.h"

#include <unistd.h>

DLNAClient::DLNAClient(QString data)
{
    QStringList l = data.split("\r\n");
    foreach(const QString &str, l)
    {
        if(str.contains("LOCATION:"))
        {
            QString e = str;
            QStringList f = e.remove("LOCATION:").remove("http://").split(":");
            ip = f[0];
            StrPort = f[1].split("/")[0];
            SMP = f[1].split("/")[1];
        }
    }
}

QString DLNAClient::GetPosition()
{//Returns the current position for the track that is playing on the DLNA server
    QString XML = XMLHead + "<m:GetPositionInfo xmlns:m=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">0</InstanceID></m:GetPositionInfo>" + XMLFoot + "\n";
    QString Request = HelperDLNA::MakeRequest("POST", this->ControlURL, XML.length(), "urn:schemas-upnp-org:service:AVTransport:1#GetPositionInfo", this->ip, this->StrPort) + XML;
    return HelperDLNA::makeSocketGetReply(this->ip, this->StrPort, Request);
}

bool DLNAClient::isConnected()
{
    this->connected = false;
    QString request = HelperDLNA::MakeRequest("GET", this->SMP, 0, "", this->ip, this->StrPort);
    QString response = HelperDLNA::makeSocketGetReply(this->ip, this->StrPort, request);
    this->ReturnCode = HelperDLNA::GetResponseCode(response);
    if(this->ReturnCode != 200) { return false;}
    this->FriendlyName = HelperDLNA::getTag("friendlyname",HelperDLNA::removeHttpHeader(response));

    for (QMap<QString, DLNAService>::iterator it = Services.begin(); it != Services.end(); ++it)
    {
        if(it.key().contains("avtransport"))
        {
            this->ControlURL = it.value().getControlURL();
            this->connected = true;
            return true;
        }
    }
    return false;
}
QString DLNAClient::Desc()
{//Gets a description of the DLNA server
    QString XML="<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:r=\"urn:schemas-rinconnetworks-com:metadata-1-0/\" xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\">\n";
    XML+="<item>\n";
    XML+="<dc:title>Capital Edinburgh "  + QString::number(QDateTime::currentDateTime().time().msec()); + "</dc:title>\n";
    XML+="<upnp:class>object.item.audioItem.audioBroadcast</upnp:class>\n";
    XML+="<desc id=\"cdudn\" nameSpace=\"urn:schemas-rinconnetworks-com:metadata-1-0/\">SA_RINCON65031_</desc>\n";
    XML+="</item>\n";
    XML += "</DIDL-Lite>\n";
    return XML;
}

QString DLNAClient::TryToPlayFile(QString UrlToPlay)
{
    if (!this->connected) this->connected = this->isConnected();//Someone might have turned the TV Off !
    if (!this->connected) throw DLNAException("Error: Not Connected!");

    for (QMap<QString, DLNAService>::iterator it = Services.begin(); it != Services.end(); ++it)
    {
        if(it.key().contains("avtransport"))
        {//This is the service we are using so upload the file and then start playing
            QString AddPlay = UploadFileToPlay(it.value().getControlURL(),UrlToPlay);
            if (this->ReturnCode != 200) throw DLNAException("Cannot upload file");
            QString PlayNow = StartPlay(it.value().getControlURL(),0);
            if (this->ReturnCode == 200) return "OK"; else throw DLNAException("Error");
        }
    }
    throw DLNAException("Couldnt find service");

}

QString DLNAClient::StartPlay(QString ControlURL, int Instance)
{//Start playing the new upload film or music track
    QString XML = XMLHead;
    XML += "<u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>"+ QString::number(Instance) + "</InstanceID><Speed>1</Speed></u:Play>\n";
    XML += XMLFoot + "\n";
    QString Request = HelperDLNA::MakeRequest("POST", this->ControlURL, XML.length(), "urn:schemas-upnp-org:service:AVTransport:1#Play", this->ip, this->StrPort) + XML;
    return HelperDLNA::makeSocketGetReply(this->ip,this->StrPort,Request);
}

QString DLNAClient::StopPlay(bool ClearQueue)
{//If we are playing music tracks and not just a movie then clear our queue of tracks
    if (!this->connected) this->connected = this->isConnected();//Someone might have turned the TV Off !
    if (!this->connected) throw DLNAException("Error: Not Connected!");
    if (ClearQueue)
    {
        this->PlayListQueue.clear();
        this->PlayListPointer = 0;
    }
    return StopPlay(this->ControlURL , 0);
}
QString DLNAClient::StopPlay(QString ControlURL, int Instance)
{//Called to stop playing a movie or a music track
    QString XML = XMLHead;
    XML += "<u:Stop xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>" + QString::number(Instance) + "</InstanceID></u:Stop>\n";
    XML += XMLFoot + "\n";
    QString Request = HelperDLNA::MakeRequest("POST", ControlURL, XML.length(), "urn:schemas-upnp-org:service:AVTransport:1#Stop", this->ip, this->StrPort) + XML;
    return HelperDLNA::makeSocketGetReply(this->ip,this->StrPort,Request);
}

QString DLNAClient::UploadFileToPlay(QString ControlURL, QString UrlToPlay)
{///Later we will send a message to the DLNA server to start the file playing
    QString XML = XMLHead;
    XML += "<u:SetAVTransportURI xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\">\n";
    XML += "<InstanceID>0</InstanceID>\n";
    XML += "<CurrentURI>" + UrlToPlay.replace(" ", "%20") + "</CurrentURI>\n";
    XML += "<CurrentURIMetaData>" + Desc() + "</CurrentURIMetaData>\n";
    XML += "</u:SetAVTransportURI>\n";
    XML += XMLFoot + "\n";
    QString Request = HelperDLNA::MakeRequest("POST", ControlURL, XML.length(), "urn:schemas-upnp-org:service:AVTransport:1#SetAVTransportURI", this->ip, this->StrPort) + XML;
    return HelperDLNA::makeSocketGetReply(this->ip,this->StrPort,Request);
}

bool DLNAClient::AddToQueue(QString UrlToPlay, bool &NewTrackPlaying)
{//We add music tracks to a play list queue and then we poll the server so we know when to send the next track in the queue to play
    if (!this->connected) this->connected = this->isConnected();//Someone might have turned the TV Off !
    if (!this->connected) return false;
    foreach (const QString &Url, PlayListQueue.values())
    {
        if (Url.toLower() == UrlToPlay.toLower())
            return false;
    }
    PlayListQueue.insert(PlayListQueue.count() + 1, UrlToPlay);
    if (!NewTrackPlaying)
    {
        PlayListPointer = PlayListQueue.count() + 1;
        StopPlay(false);
        TryToPlayFile(UrlToPlay);
        NewTrackPlaying = true;
    }
    return false;
}
int DLNAClient::TotalSeconds(QString Value)
{//Convert the time left for the track to play back to seconds
    QStringList val = Value.split(".");
    if(!val.empty())
    {
        int Mins = val[1].split(':')[0].toInt();
        int Secs = val[1].split(':')[1].toInt();
        return Mins * 60 + Secs;
    }

    return 0;
}
int DLNAClient::PlayNextQueue(bool Force)
{//Play the next track in our queue but only if the current track is about to end or unless we are being forced
    if (Force)
    {//Looks like someone has pressed the next track button
        PlayListPointer++;
        if (PlayListQueue.count() == 0) return 0;
        if (PlayListPointer > PlayListQueue.count())
            PlayListPointer = 1;
        QString Url = PlayListQueue[PlayListPointer];
        StopPlay(false);
        TryToPlayFile(Url);//Just play it
        NoPlayCount = 0;
        return 310;//Just guess for now how long the track is
    }
    else
    {
        QString HTMLPosition = GetPosition();
        if (HTMLPosition.length() < 50) return 0;
        QString TrackDuration = HelperDLNA::getTag("TrackDuration",HTMLPosition).toStdString().substr(2).c_str();
        QString RelTime = HelperDLNA::getTag("RelTime",HTMLPosition).toStdString().substr(2).c_str();
        int RTime = TotalSeconds(RelTime);
        int TTime = TotalSeconds(TrackDuration);
        if (RTime < 3 || TTime < 2)
        {
            NoPlayCount++;
            if (NoPlayCount > 3)
            {
                StopPlay(false);
                return PlayNextQueue(true);//Force the next track to start because the current track is about to end
            }
            else
                return 0;

        }
        int SecondsToPlay = TTime - RTime - 5;
        if (SecondsToPlay < 0) SecondsToPlay = 0;//Just a safeguard
        if (SecondsToPlay <10)
        {//Current track is about to end so wait a few seconds and then force the next track in our queue to play
            usleep(((SecondsToPlay * 1000) +100)*1000);
            return PlayNextQueue(true);
        }
        return SecondsToPlay;//Will have to wait to be polled again before playing the next track in our queue
    }
}
QString DLNAClient::Pause(QString ControlURL, int Instance)
{//Called to pause playing a movie or a music track
    QString XML = XMLHead;
    XML += "<u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>" + QString::number(Instance) + "</InstanceID></u:Pause>\n";
    XML += XMLFoot + "\n";
    QString Request = HelperDLNA::MakeRequest("POST", ControlURL, XML.length(), "urn:schemas-upnp-org:service:AVTransport:1#Pause", this->ip, this->StrPort) + XML;
    return HelperDLNA::makeSocketGetReply(this->ip,this->StrPort,Request);
}


int DLNAClient::PlayPreviousQueue()
{//Play the previous track in our queue, we don't care if the current track has not completed or not, just do it
    PlayListPointer--;
    if (PlayListQueue.count() == 0) return 0;
    if (PlayListPointer == 0)
        PlayListPointer = PlayListQueue.count();
    QString Url = PlayListQueue[PlayListPointer];
    StopPlay(false);
    TryToPlayFile(Url);
    return 310;
}

