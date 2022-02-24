#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#include "udpprocess.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QFile>

UdpProcess::UdpProcess(QObject *parent) : QObject(parent)
{
    initData();
}

UdpProcess::~UdpProcess()
{
    saveConfig();
}

bool UdpProcess::connectSocket()
{
    bool result = true;
    loadConfig();
    result = broadcastSocket->bind(QHostAddress::AnyIPv4, broadcastPort, QUdpSocket::ShareAddress);
    result = socket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress);
    connect(broadcastSocket, &QUdpSocket::readyRead, this, &UdpProcess::slotBroadcastRecv);
    connect(socket, &QUdpSocket::readyRead, this, &UdpProcess::slotUdpRecv);
    return result;
}

bool UdpProcess::stopConnect()
{
    bool result = true;
    broadcastSocket->abort();
    socket->abort();
    disconnect(broadcastSocket, &QUdpSocket::readyRead, this, &UdpProcess::slotBroadcastRecv);
    disconnect(socket, &QUdpSocket::readyRead, this, &UdpProcess::slotUdpRecv);
    return result;
}

void UdpProcess::udpSend(const QString &msg)
{
    socket->writeDatagram(msg.toUtf8(), QHostAddress(destAddress), destPort);
}

QString UdpProcess::getDestAddress()
{
    return destAddress;
}

QString UdpProcess::getUrl()
{
    return urlPath;
}

void UdpProcess::slotBroadcastRecv()
{
    QHostAddress clientAddress; //client ip addr
    while(broadcastSocket->hasPendingDatagrams())
    {
        QString tempStr;
        QByteArray datagram;
        datagram.resize(broadcastSocket->pendingDatagramSize());
        broadcastSocket->readDatagram(datagram.data(), datagram.size(), &clientAddress);
        tempStr = clientAddress.toString() + "|" + QString(datagram);
        emit signalsBroadcastMsg(tempStr);
    }
}

void UdpProcess::slotUdpRecv()
{
    QHostAddress clientAddress; //client ip addr
    while (socket->hasPendingDatagrams())
    {
        QString tempStr;
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size(), &clientAddress);
        parseData(clientAddress.toString(), datagram);
    }
}

void UdpProcess::initData()
{
    destAddress = "10.0.0.2";
    destPort = 9999;

    broadcastPort = 8888;
    port = 9998;
    broadcastSocket = new QUdpSocket(this);
    socket = new QUdpSocket(this);

    urlPath = "rtsp://10.0.0.2/stream3";

    loadConfig();
}

void UdpProcess::parseData(const QString &address, QByteArray &data)
{
    bool ok = false;
    QString recvData(data);
    QStringList list1 = recvData.split('|');
    if(list1.count() > 0)
    {
        int code = list1[0].toInt(&ok);
        if(ok)
        {
            if(code == 9 || code == 10)
            {
                QString tempStr;
                tempStr = address + "|" + recvData;
                emit signalsLogMsg(tempStr);
            }
            else
            {
                QString tempStr;
                tempStr = address + "|" + recvData;
                emit signalsResultMsg(tempStr);
            }
        }
        else
        {
            QString tempStr;
            tempStr = "ERROR: " + address + "|" + recvData;
            emit signalsBroadcastMsg(tempStr);
        }
    }
    else
    {
        QString tempStr;
        tempStr = "ERROR " + address + ":" + recvData;
        emit signalsBroadcastMsg(tempStr);
    }
}

int UdpProcess::loadConfig()
{
    QByteArray data;
    QFile file;
    file.setFileName("./network_config.json");
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        return -1;
    }
    data = file.readAll();
    file.close();
    QJsonParseError jsonError;
    QJsonDocument parseDoucment = QJsonDocument::fromJson(QString(data).toUtf8(), &jsonError);
    if(jsonError.error == QJsonParseError::NoError)
    {
        if (!(parseDoucment.isNull() || parseDoucment.isEmpty()))
        {
            if (parseDoucment.isObject())
            {
                QJsonObject jsonObject = parseDoucment.object();
                if(jsonObject.contains("broadcastSocket"))
                {
                    broadcastPort = jsonObject.take("broadcastSocket").toVariant().toInt();
                }
                if(jsonObject.contains("port"))
                {
                    port = jsonObject.take("port").toVariant().toInt();
                }
                if(jsonObject.contains("destPort"))
                {
                    destPort = jsonObject.take("destPort").toVariant().toInt();
                }
                if(jsonObject.contains("destAddress"))
                {
                    destAddress = jsonObject.take("destAddress").toVariant().toString();
                }
                if(jsonObject.contains("urlPath"))
                {
                    urlPath = jsonObject.take("urlPath").toVariant().toString();
                }

            }
        }
    }
    else
    {
        return -2;
    }
    return 0;
}

int UdpProcess::saveConfig()
{
    QJsonDocument doc;
    QByteArray data;
    QJsonObject jsonData;
    QFile file("./network_config.json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
    {
        return -1;
    }
    jsonData.insert("broadcastPort", QString::number(broadcastPort));
    jsonData.insert("port", QString::number(port));
    jsonData.insert("destPort", QString::number(destPort));
    jsonData.insert("destAddress", destAddress);
    jsonData.insert("urlPath", urlPath);

    doc.setObject(jsonData);
    data = doc.toJson();
    file.write(data);
    file.close();
    return 0;
}
