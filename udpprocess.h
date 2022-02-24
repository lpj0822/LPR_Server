#ifndef UDPPROCESS_H
#define UDPPROCESS_H

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>

class UdpProcess : public QObject
{
    Q_OBJECT
public:
    UdpProcess(QObject *parent = nullptr);
    ~UdpProcess();

    bool connectSocket();
    bool stopConnect();
    void udpSend(const QString &msg);

    QString getDestAddress();
    QString getUrl();

signals:
    void signalsBroadcastMsg(QString msg);
    void signalsResultMsg(QString msg);
    void signalsLogMsg(QString msg);

public slots:

    void slotBroadcastRecv();
    void slotUdpRecv();


private:
    QUdpSocket* broadcastSocket;
    int broadcastPort;

    QUdpSocket* socket;
    int port;

    QString destAddress;
    int destPort;

    QString urlPath;

private:

    void initData();

    void parseData(const QString &address, QByteArray &data);

    int loadConfig();
    int saveConfig();
};

#endif // UDPPROCESS_H
