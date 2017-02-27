#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QObject>
#include <QAbstractSocket>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QMutexLocker>
#include "UICmdResponse.h"

class Connector : public QObject
{
    Q_OBJECT
public:
    enum Operation
    {
        OP_SENDING,
        OP_RECEIVING,
        OP_SUCCEED,
        OP_FAILED
    };
public:
    Connector(QObject *parent = 0);
    bool connectToServer(const QString &addr, quint16 port);
    bool disconnectFromServer();
    bool send(const QString &command);
    bool send(const QString &command, QString &result);
    bool send(const QString &command, au::UICmdResponse &response);

signals:
    void connectionConnected();
    void connectionDisconnected();
    void connectionOperating(const QString &command, int op);

public slots:
    void socketConnect();

private:
    QAbstractSocket *absSocket;
    QUdpSocket *udpSocket;
    QMutex mutex;
};

#endif // CONNECTOR_H
