#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#include "simplecontrolwindow.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>

SimpleControlWindow::SimpleControlWindow(QWidget *parent)
    : QWidget(parent)
{
    init();
    initUI();
    initConnect();
}

SimpleControlWindow::~SimpleControlWindow()
{
    udpProcess->stopConnect();
    lprShowWindow->deleteLater();
    dataShowWindow->deleteLater();
}

void SimpleControlWindow::slotOK()
{
    if(!udpProcess->connectSocket())
    {
        udpProcess->stopConnect();
        QMessageBox::information(this, tr("LPR Server"), tr("连接摄像头失败"));
    }
    else
    {
        QStringList argList;
        QString url = "root@" + udpProcess->getDestAddress();
        argList << "-o StrictHostKeyChecking=no" << url << "pwd";
        qDebug() << argList << endl ;
        cmdProcess.start("ssh", argList);
        if(cmdProcess.waitForFinished(10000))
        {
            QMessageBox::information(this, tr("LPR Server"), tr("连接摄像头成功"));
            buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            lprShowWindow->setEnabled(true);
            dataShowWindow->setEnabled(true);
            cmdShowWindow->setEnabled(true);
        }
        else
        {
            qDebug() << cmdProcess.readAllStandardError();
            cmdProcess.kill();
            udpProcess->stopConnect();
            QMessageBox::information(this, tr("LPR Server"), tr("连接摄像头失败"));
        }
    }
}

void SimpleControlWindow::init()
{
    udpProcess = std::make_shared<UdpProcess>();

#ifdef WIN32
    cmdProcess.setProgram("cmd");
#else
    cmdProcess.setProgram("bash");
#endif
}

void SimpleControlWindow::initUI()
{
    tabweight = new QTabWidget();

    lprShowWindow = new LPRShowWindow(udpProcess);
    lprShowWindow->setEnabled(false);
    tabweight->addTab(lprShowWindow, tr("LPR Show"));

    dataShowWindow = new DataShowWindow(udpProcess);
    dataShowWindow->setEnabled(false);
    tabweight->addTab(dataShowWindow, tr("Data Show"));

    cmdShowWindow = new CommandShowWindow(udpProcess);
    cmdShowWindow->setEnabled(false);
    tabweight->addTab(cmdShowWindow, tr("Command Show"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); // 创建QDialogButtonBox
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("连接摄像头"));//将buttonbox中的ok 变成汉化
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

    mainLayout=new QVBoxLayout();
    mainLayout->addWidget(tabweight);
    mainLayout->addWidget(buttonBox);

    this->setLayout(mainLayout);
    //this->setMaximumSize(700,520);
    this->setMinimumSize(1200, 800);
    this->setWindowTitle(tr("LPR Server"));
}

void SimpleControlWindow::initConnect()
{
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SimpleControlWindow::slotOK);  // 为button定义连接信号槽
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SimpleControlWindow::close);
}
