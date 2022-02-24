#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#include "cmdshowwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

CommandShowWindow::CommandShowWindow(std::shared_ptr<UdpProcess> &udp, QWidget *parent)
    : QWidget(parent), udpProcess(udp)
{
    initData();
    initUI();
    initConnect();
}

CommandShowWindow::~CommandShowWindow()
{

}

void CommandShowWindow::slotRecord()
{
    if(recordButton->text() == QString(tr("开始录制数据")))
    {
        QStringList argList1;
        QString url1 = "root@" + address;
        argList1 << "-o StrictHostKeyChecking=no" << url1 << "killall test_lpr";
        qDebug() << argList1 << endl ;
        cmdProcess.start("ssh", argList1);
        cmdProcess.waitForFinished();
        commandText->append(tr("Stop LPR Success!"));

        QStringList argList;
        QString url = "root@" + address;
        argList << url << "/data/record_lpr.sh";
        qDebug() << argList << endl ;
        cmdProcess.start("ssh", argList);
        cmdProcess.waitForFinished();
        commandText->append(tr("Start Record Success!"));
        recordButton->setText(tr("停止录制数据"));
    }
    else if(recordButton->text() == QString(tr("停止录制数据")))
    {
        QStringList argList1;
        QString url1 = "root@" + address;
        argList1 << "-o StrictHostKeyChecking=no" << url1 << "killall record_lpr";
        qDebug() << argList1 << endl ;
        cmdProcess.start("ssh", argList1);
        cmdProcess.waitForFinished();
        commandText->append(tr("Stop Record Success!"));
        recordButton->setText(tr("开始录制数据"));
    }
}

void CommandShowWindow::slotSendData()
{
    if(dataSendButton->text() == QString(tr("实时传输数据")))
    {
        QStringList argList1;
        QString url1 = "root@" + address;
        argList1 << "-o StrictHostKeyChecking=no" << url1 << "killall test_lpr";
        qDebug() << argList1 << endl ;
        cmdProcess.start("ssh", argList1);
        cmdProcess.waitForFinished();
        commandText->append(tr("Stop LPR Success!"));

        QStringList argList;
        QString url = "root@" + address;
        argList << url << "/data/send_lpr.sh";
        qDebug() << argList << endl ;
        cmdProcess.start("ssh", argList);
        cmdProcess.waitForFinished();
        commandText->append(tr("Start Send Success!"));
        dataSendButton->setText(tr("停止传输数据"));
    }
    else if(dataSendButton->text() == QString(tr("停止传输数据")))
    {
        QStringList argList1;
        QString url1 = "root@" + address;
        argList1 << "-o StrictHostKeyChecking=no" << url1 << "killall send_lpr";
        qDebug() << argList1 << endl ;
        cmdProcess.start("ssh", argList1);
        cmdProcess.waitForFinished();
        commandText->append(tr("Stop Send Success!"));
        dataSendButton->setText(tr("实时传输数据"));
    }
}

void CommandShowWindow::slotRecvMsg(QString msg)
{
    QStringList dataList = msg.split('|');
    commandText->append("recv: " + msg);
    if(dataList.count() > 1)
    {
        address = dataList[0];
    }
}

void CommandShowWindow::slotSendMsg()
{
    if(!editText->text().isEmpty())
    {
        udpProcess->udpSend(editText->text());
        commandText->append("send: " + editText->text());
    }
    else
    {
        QMessageBox::information(this, tr("Command Server"), tr("请输入命令"));
    }
}

void CommandShowWindow::slotReadyReadStandardOutput()
{
    QByteArray cmdout = cmdProcess.readAllStandardOutput();
    if(!cmdout.isEmpty()){
        commandText->append(QString::fromLocal8Bit(cmdout));
    }
}

void CommandShowWindow::slotReadyReadStandardError()
{
    QByteArray cmdout = cmdProcess.readAllStandardError();
    if(!cmdout.isEmpty()){
        commandText->append(QString::fromLocal8Bit(cmdout));
    }
}

void CommandShowWindow::initUI()
{
    recordButton = new QPushButton(tr("开始录制数据"));
    dataSendButton = new QPushButton(tr("实时传输数据"));

    QHBoxLayout *topLayout=new QHBoxLayout();
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->setSpacing(30);
    topLayout->addWidget(recordButton);
    topLayout->addWidget(dataSendButton);

    scrollArea = new QScrollArea(this);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setBackgroundRole(QPalette::Dark);

    commandText = new MyTextBrowser();
    commandText->setReadOnly(true);

    scrollArea->setWidget(commandText);
    scrollArea->setWidgetResizable(true);

    inputLabel = new QLabel(tr("输入命令:"));
    editText = new QLineEdit("200|exit");
    editText->setClearButtonEnabled(true);
    sendButton = new QPushButton(tr("发送"));

    QHBoxLayout *bottomLayout=new QHBoxLayout();
    bottomLayout->setAlignment(Qt::AlignCenter);
    bottomLayout->setSpacing(30);
    bottomLayout->addWidget(inputLabel);
    bottomLayout->addWidget(editText);
    bottomLayout->addWidget(sendButton);

    mainLayout = new QVBoxLayout();//主布局

    mainLayout->setMargin(10); //设置这个对话框的边距
    mainLayout->setSpacing(10);  //设置各个控件之间的边距
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    mainLayout->addLayout(bottomLayout);
//    mainLayout->setStretchFactor(scrollArea, 5);
//    mainLayout->setStretchFactor(bottomLayout, 1);
    this->setLayout(mainLayout);
    //this->setMaximumSize(700,520);
    this->setMinimumSize(1200, 700);
    this->setWindowTitle(tr("Command Server"));
}

void CommandShowWindow::initConnect()
{
    connect(udpProcess.get(), &UdpProcess::signalsBroadcastMsg, this, &CommandShowWindow::slotRecvMsg);
    connect(sendButton, &QPushButton::clicked, this, &CommandShowWindow::slotSendMsg);

    connect(recordButton, &QPushButton::clicked, this, &CommandShowWindow::slotRecord);
    connect(dataSendButton, &QPushButton::clicked, this, &CommandShowWindow::slotSendData);

    connect(&cmdProcess, &QProcess::readyReadStandardOutput, this, &CommandShowWindow::slotReadyReadStandardOutput);
    connect(&cmdProcess, &QProcess::readyReadStandardError, this, &CommandShowWindow::slotReadyReadStandardError);
}

void CommandShowWindow::initData()
{
    address = udpProcess->getDestAddress();;
#ifdef WIN32
    cmdProcess.setProgram("cmd");
#else
    cmdProcess.setProgram("bash");
#endif
}
