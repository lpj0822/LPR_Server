#ifndef CMDSHOWWINDOW_H
#define CMDSHOWWINDOW_H

#include <memory>

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QTextBrowser>
#include <QVector>
#include <QPoint>
#include <QLineEdit>
#include <QProcess>
#include "utilityGUI/customWindow/mytextbrowser.h"
#include "udpprocess.h"

class CommandShowWindow : public QWidget
{
    Q_OBJECT

public:
    CommandShowWindow(std::shared_ptr<UdpProcess> &udp, QWidget *parent = 0);
    ~CommandShowWindow();

public slots:
    void slotRecord();
    void slotSendData();
    void slotRecvMsg(QString msg);
    void slotSendMsg();

    void slotReadyReadStandardOutput();
    void slotReadyReadStandardError();

private:
    MyTextBrowser *commandText;//输出黑匣子指令
    QVBoxLayout *mainLayout;//主布局
    QScrollArea *scrollArea;//滚动区域

    QPushButton *recordButton;
    QPushButton *dataSendButton;

    QLabel *inputLabel;
    QLineEdit *editText;
    QPushButton *sendButton;

    std::shared_ptr<UdpProcess> udpProcess;
    QProcess cmdProcess;

    QString address;


    void initUI();
    void initData();
    void initConnect();
};


#endif // CMDSHOWWINDOW_H
