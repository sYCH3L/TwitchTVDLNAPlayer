#ifndef DLNAFINDER_H
#define DLNAFINDER_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QNetworkDatagram>
#include <QList>

#include "dlnaclient.h"

class DLNAFinder : public QObject
{
    Q_OBJECT
public:
    explicit DLNAFinder(QObject *parent = nullptr);
    void find();
    ~DLNAFinder();

signals:

public slots:
    void readResponse();
private:
    QUdpSocket *sock;
    QList<DLNAClient> *list;
};

#endif // DLNAFINDER_H
