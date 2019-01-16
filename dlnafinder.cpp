#include "dlnafinder.h"

DLNAFinder::DLNAFinder(QObject *parent) : QObject(parent)
{
    sock = new QUdpSocket(this);
    sock->bind(QHostAddress(QHostAddress::AnyIPv4), 6000);

    list = new QList<DLNAClient>();

    connect(sock, SIGNAL(readyRead()),
            this, SLOT(readResponse()));
}
DLNAFinder::~DLNAFinder()
{
    delete sock;
    delete list;
}

void DLNAFinder::find()
{
    QByteArray data = "M-SEARCH * HTTP/1.1\r\n"
                      "HOST:239.255.255.250:1900\r\n"
                      "MAN:\"ssdp:discover\"\r\n"
                      "ST:ssdp:all\r\n"
                      "MX:3\r\n\r\n";

    sock->writeDatagram(data,QHostAddress(QString("239.255.255.250")),1900);
}

void DLNAFinder::readResponse()
{
    while (sock->hasPendingDatagrams()) {
        QNetworkDatagram datagram = sock->receiveDatagram();

        DLNAClient temp(QString::fromUtf8(datagram.data()));

        list->push_back(temp);
    }
}
