#include "connector.h"
#include <QMessageBox>

const int TIMEOUT = 2000;

Connector::Connector(QObject *parent) : QObject(parent)
{
    absSocket = 0;
    udpSocket = new QUdpSocket(this);
    connect(udpSocket,SIGNAL(connected()),this,SLOT(socketConnect()));

}

bool Connector::connectToServer(const QString& addr, quint16 port)
{
    if (QHostAddress(addr).isNull())
    {
        QMessageBox::critical(0, "", "\n\nEnter a valid IP address    \n\n", QMessageBox::Ok);
        return false;
    }

    absSocket = udpSocket;
    if (absSocket)
    {
        while (true)
        {
            bool ok = true;
            absSocket->connectToHost(QHostAddress(addr), port);
            if (!absSocket->waitForConnected(TIMEOUT * 2))
            {
                QMessageBox::critical(0, "", "\n\nCannot establish a connection to Quack    \n\n",
                                      QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry);
                ok=false;
            }

            if (ok)
            {
                socketConnect();
                return true;
            }
            return false;

        }
    }
    return false;
}

bool Connector::send(const QString &command)
{
    QMutexLocker locker(&mutex);

    if (absSocket)
    {
        if (absSocket->socketType() == QAbstractSocket::UdpSocket)
        {
            absSocket->write((command + "\n").toLatin1());
            emit connectionOperating(command, OP_SUCCEED);
            return true;
        }
    }
    return false;
}

bool Connector::send(const QString &command, au::UICmdResponse &response)
{
    QString result;

    bool s = send(command, result);

    if (s)
        response = au::UICmdResponse(result);

    return s;
}

bool Connector::send(const QString &command, QString &result)
{
    QMutexLocker locker(&mutex);
    if (absSocket)
    {
        if (absSocket->socketType() == QAbstractSocket::UdpSocket)
        {
           // emit connectionOperating(command, OP_SENDING);
            absSocket->write((command + "\n").toLatin1());
            if (absSocket->socketType() == QAbstractSocket::UdpSocket ||
                    absSocket->waitForBytesWritten(TIMEOUT))
            {
               // emit connectionOperating(command, OP_RECEIVING);
                if (absSocket->waitForReadyRead(TIMEOUT))
                {
                    //emit connectionOperating(command, OP_SUCCEED);
                    result = QString(absSocket->readAll());
                    return true;
                }
            }
        }
    }
    return false;

}


void Connector::socketConnect()
{
    emit connectionConnected();
}

bool Connector::disconnectFromServer()
{
    udpSocket->disconnectFromHost();
//    absSocket->disconnectFromHost();
    absSocket = 0;
    emit connectionDisconnected();
    return true;
}
