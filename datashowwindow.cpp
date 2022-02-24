#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#include "datashowwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

DataShowWindow::DataShowWindow(std::shared_ptr<UdpProcess> &udp, QWidget *parent)
    : QWidget(parent), udpProcess(udp)
{
    initData();
    initUI();
    initConnect();
}

DataShowWindow::~DataShowWindow()
{
    cmdProcess.close();
    cmdProcess.waitForFinished();
}

void DataShowWindow::slotGetLog()
{
    QString sendMsg = "101|get log";
    udpProcess->udpSend(sendMsg);
    commandText->append(sendMsg);
}

void DataShowWindow::slotDownloadLog()
{
    QStringList argList;
    QDir makeDir;
    QString tempDir = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), saveDir, QFileDialog::ShowDirsOnly);
    if(tempDir.trimmed().isEmpty())
    {
        return;
    }
    if(!makeDir.exists(tempDir))
    {
        if(!makeDir.mkpath(tempDir))
        {
            commandText->append(tr("Download log fail!"));
            qDebug() << "make" << tempDir << "fail!" << endl;
            return;
        }
    }
    QString url = "root@" + address + ":/data/glog_file/*";
    argList << url << tempDir;
    qDebug() << argList << endl ;
    cmdProcess.start("scp", argList);
    cmdProcess.waitForFinished();
    commandText->append(tr("Download log success!"));

    saveDir = tempDir;
}

void DataShowWindow::slotGetImage()
{
    QString sendMsg = "102|get crop";
    udpProcess->udpSend(sendMsg);
    commandText->append(sendMsg);
}

void DataShowWindow::slotDownloadImage()
{
    QDir makeDir;
    QString tempDir = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), saveDir, QFileDialog::ShowDirsOnly);
    if(tempDir.trimmed().isEmpty())
    {
        return;
    }
    if(!makeDir.exists(tempDir))
    {
        if(!makeDir.mkpath(tempDir))
        {
            qDebug() << "make" << tempDir << "fail!" << endl;
            return;
        }
    }

    QStringList argList1;
    QString url1 = "root@" + address;
    argList1 << url1 << "killall test_lpr";
    qDebug() << argList1 << endl ;
    cmdProcess.start("ssh", argList1);
    cmdProcess.waitForFinished();
    commandText->append(tr("Stop Save Image Success!"));

    QStringList argList2;
    QString url2 = "root@" + address + ":/data/save_data/*";
    argList2 << "-r" << url2 << tempDir;
    qDebug() << argList2 << endl ;
    cmdProcess.start("scp", argList2);
    cmdProcess.waitForFinished();
    commandText->append(tr("Download Image Success!"));

    saveDir = tempDir;
}

void DataShowWindow::slotDF()
{
    QStringList argList;
    QString url = "root@" + address;
    argList << "-o StrictHostKeyChecking=no" << url << "df -h";
    qDebug() << argList << endl ;
    cmdProcess.start("ssh", argList);
    cmdProcess.waitForFinished();
}

void DataShowWindow::slotRM()
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("Data Server"),QString(tr("是否删除所有截图？")),
                                 QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::Yes)
    {
        QStringList argList;
        QString url = "root@" + address;
        argList << url << "rm -rf /data/save_data/*";
        qDebug() << argList << endl ;
        cmdProcess.start("ssh", argList);
        cmdProcess.waitForFinished();
        commandText->append(tr("Remove Image Success!"));
    }
}

void DataShowWindow::slotRMLog()
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("Data Server"),QString(tr("是否删除所有Log？")),
                                 QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::Yes)
    {
        QStringList argList;
        QString url = "root@" + address;
        argList << url << "rm -rf /data/glog_file/*";
        qDebug() << argList << endl ;
        cmdProcess.start("ssh", argList);
        cmdProcess.waitForFinished();
        commandText->append(tr("Remove Log Success!"));
    }
}

void DataShowWindow::slotClearInit()
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("Data Server"),QString(tr("是否清除初始化数据？")),
                                 QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::Yes)
    {
        QStringList argList;
        QString url = "root@" + address;
        argList << url << "rm -rf /data/bg.bin";
        qDebug() << argList << endl ;
        cmdProcess.start("ssh", argList);
        cmdProcess.waitForFinished();
        commandText->append(tr("Clear Init Data Success!"));
    }
}

void DataShowWindow::slotRecvMsg(QString msg)
{
    bool ok = false;
    QStringList dataList = msg.split('|');
    commandText->append(msg);
    if(dataList.count() > 1)
    {
        address = dataList[0];
        int code = dataList[1].toInt(&ok);
        if(ok)
        {
            if(code == 9 || code == 10)
            {
                dataListWidget->clear();
                for(int index = 2; index < dataList.count(); index++)
                {
                    QString temp_str = dataList[index];
                    if(!temp_str.isEmpty())
                    {
                        QListWidgetItem *item = new QListWidgetItem(QIcon(":/qss_icons/style/rc/checkbox_checked_focus.png"),
                                                                    temp_str);
                        item->setData(Qt::UserRole, 1);
                        dataListWidget->insertItem(index, item);
                    }
                }
            }
            else
            {
                commandText->append("(ERROR) " + msg);
            }
        }
        else
        {
            commandText->append("(ERROR) " + msg);
        }
    }
    else
    {
        commandText->append("(ERROR) " + msg);
    }
}

void DataShowWindow::slotReadyReadStandardOutput()
{
    QByteArray cmdout = cmdProcess.readAllStandardOutput();
    if(!cmdout.isEmpty()){
        commandText->append(QString::fromLocal8Bit(cmdout));
    }
}

void DataShowWindow::slotReadyReadStandardError()
{
    QByteArray cmdout = cmdProcess.readAllStandardError();
    if(!cmdout.isEmpty()){
        commandText->append(QString::fromLocal8Bit(cmdout));
    }
}

void DataShowWindow::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{

}


void DataShowWindow::initUI()
{
    getLogButton = new QPushButton(tr("获取Log"));
    downloadLogButton = new QPushButton(tr("下载所有Log"));

    getImageButton = new QPushButton(tr("获取保存的截图"));
    downloadImageButton = new QPushButton(tr("下载所有截图"));

    dfButton = new QPushButton(tr("查看磁盘空间"));
    rmDataButton = new QPushButton(tr("删除所有截图"));
    rmLogButton = new QPushButton(tr("删除所有Log"));

    clearInitButton = new QPushButton(tr("清除初始化数据"));

    QHBoxLayout *topLayout=new QHBoxLayout();
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->setSpacing(30);
    topLayout->addWidget(getLogButton);
    topLayout->addWidget(downloadLogButton);
    topLayout->addWidget(getImageButton);
    topLayout->addWidget(downloadImageButton);
    topLayout->addWidget(dfButton);
    topLayout->addWidget(rmDataButton);
    topLayout->addWidget(rmLogButton);
    topLayout->addWidget(clearInitButton);

    scrollArea = new QScrollArea(this);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setBackgroundRole(QPalette::Dark);

    dataListWidget = new QListWidget();

    scrollArea->setWidget(dataListWidget);
    scrollArea->setWidgetResizable(true);

    commandText = new MyTextBrowser();
    commandText->setFixedHeight(150);
    commandText->setReadOnly(true);

    mainLayout = new QVBoxLayout();//主布局

    mainLayout->setMargin(10); //设置这个对话框的边距
    mainLayout->setSpacing(10);  //设置各个控件之间的边距
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(commandText);
    mainLayout->setStretchFactor(scrollArea, 5);
    mainLayout->setStretchFactor(dataListWidget, 1);
    this->setLayout(mainLayout);
    //this->setMaximumSize(700,520);
    this->setMinimumSize(1200, 700);
    this->setWindowTitle(tr("Data Server"));
}

void DataShowWindow::initConnect()
{
    connect(getLogButton, &QPushButton::clicked, this, &DataShowWindow::slotGetLog);
    connect(downloadLogButton, &QPushButton::clicked, this, &DataShowWindow::slotDownloadLog);
    connect(getImageButton, &QPushButton::clicked, this, &DataShowWindow::slotGetImage);
    connect(downloadImageButton, &QPushButton::clicked, this, &DataShowWindow::slotDownloadImage);
    connect(dfButton, &QPushButton::clicked, this, &DataShowWindow::slotDF);
    connect(rmDataButton, &QPushButton::clicked, this, &DataShowWindow::slotRM);
    connect(rmLogButton, &QPushButton::clicked, this, &DataShowWindow::slotRMLog);
    connect(clearInitButton, &QPushButton::clicked, this, &DataShowWindow::slotClearInit);

    connect(udpProcess.get(), &UdpProcess::signalsLogMsg, this, &DataShowWindow::slotRecvMsg);

    connect(&cmdProcess, &QProcess::readyReadStandardOutput, this, &DataShowWindow::slotReadyReadStandardOutput);
    connect(&cmdProcess, &QProcess::readyReadStandardError, this, &DataShowWindow::slotReadyReadStandardError);
    // connect(cmdProcess,SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(slotFinished(int, QProcess::ExitStatus)));
}

void DataShowWindow::initData()
{
    address = udpProcess->getDestAddress();
    saveDir = "./";
#ifdef WIN32
    cmdProcess.setProgram("cmd");
#else
    cmdProcess.setProgram("bash");
#endif
}
