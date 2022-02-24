#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#include "lprshowwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

LPRShowWindow::LPRShowWindow(std::shared_ptr<UdpProcess> &udp, QWidget *parent)
    : QWidget(parent), udpProcess(udp)
{
    initData();
    initUI();
    initConnect();
}

LPRShowWindow::~LPRShowWindow()
{
    if(rtspProcess != nullptr)
    {
        rtspProcess->stop();
        delete  rtspProcess;
        rtspProcess = nullptr;
    }
    if(videoPlayer != nullptr)
    {
        videoPlayer->stop();
    }
    isOpen = false;
}

void LPRShowWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
}

void LPRShowWindow::slotOpenVideo()
{
//    videoPlayer->setMedia(QMediaContent(QUrl(urlPath)));
//    playerState = videoPlayer->state();
//    switch (playerState) {
//    case QMediaPlayer::StoppedState:
//    case QMediaPlayer::PausedState:
//        player->play();
//        commandText->append(tr("播放视频中"));
//        break;
//    case QMediaPlayer::PlayingState:
//        player->pause();
//        commandText->append(tr("暂停播放"));
//        break;
//    }
    if(openVideoButton->text() == QString(tr("打开视频流")))
    {
        isOpen = rtspProcess->start(udpProcess->getUrl());
        if(!isOpen)
        {
            QMessageBox::information(this, tr("LPR Server"), tr("打开视频流失败"));
        }
        else
        {
            openVideoButton->setText(tr("关闭视频流"));
        }
    }
    else if(openVideoButton->text() == QString(tr("关闭视频流")))
    {
        rtspProcess->stop();
        isOpen = false;
        this->currentImage = QImage(tr(":/images/play.png"));
        slotShowImage(this->currentImage);
        openVideoButton->setText(tr("打开视频流"));
    }
}

void LPRShowWindow::slotGetData()
{

}

void LPRShowWindow::slotDownload()
{

}

void LPRShowWindow::slotRecvMsg(QString msg)
{
    //    qDebug() << commandText->toPlainText()<<endl;
    //    commandText->clear();
    commandText->append(msg);
}

void LPRShowWindow::slotPlayErrorMessage(QMediaPlayer::Error err)
{
    // commandText->append(videoPlayer->errorString());
}

void LPRShowWindow::slotShowImage(QImage image)
{
    videoShowLabel->setPixmap(QPixmap::fromImage(image));
}

void LPRShowWindow::initUI()
{
    openVideoButton = new QPushButton(tr("打开视频流"));
    getDataButton = new QPushButton(tr("获取数据"));
    downloadButton = new QPushButton(tr("下载数据"));

    QHBoxLayout *topLayout=new QHBoxLayout();
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->setSpacing(30);
    topLayout->addWidget(openVideoButton);
    topLayout->addWidget(getDataButton);
    topLayout->addWidget(downloadButton);

    scrollArea = new QScrollArea(this);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setBackgroundRole(QPalette::Dark);

//    QtAV::Widgets::registerRenderers();
//    renderer = new QtAV::GLWidgetRenderer2(this);
//    if (!renderer->widget()) {
//        QMessageBox::warning(this, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
    videoShowLabel = new QLabel();
    QPixmap tempImage = QPixmap::fromImage(currentImage);
    videoShowLabel->setPixmap(tempImage);
    videoShowLabel->setAlignment(Qt::AlignCenter);
    // videoShowLabel->setScaledContents(true);

    scrollArea->setWidget(videoShowLabel);
    scrollArea->setWidgetResizable(true);
//    }
//    else
//    {
//        player = new QtAV::AVPlayer(this);
//        player->setFrameRate(0.0);
//        player->setInterruptOnTimeout(true);
//        player->setInterruptTimeout(30000.0);
//        player->setBufferMode(QtAV::BufferPackets);
//        player->setBufferValue(-1);

//        player->setRenderer(renderer);
//        scrollArea->setWidget(renderer->widget());
//        scrollArea->setWidgetResizable(true);
//    }

//    videoPlayer = new QMediaPlayer(this);
//    videoWidget = new QVideoWidget(this);
//    scrollArea->setWidget(videoWidget);
//    scrollArea->setWidgetResizable(true);
//    videoPlayer->setVideoOutput(videoWidget);

    dataListWidget = new QListWidget();

    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addWidget(scrollArea);
    centerLayout->addWidget(dataListWidget);
    centerLayout->setStretchFactor(scrollArea, 4);
    centerLayout->setStretchFactor(dataListWidget, 1);

    commandText = new MyTextBrowser();
    commandText->setFixedHeight(150);
    commandText->setReadOnly(true);

    mainLayout = new QVBoxLayout();//主布局

    mainLayout->setMargin(10); //设置这个对话框的边距
    mainLayout->setSpacing(10);  //设置各个控件之间的边距
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(centerLayout);
    mainLayout->addWidget(commandText);
    this->setLayout(mainLayout);
    //this->setMaximumSize(700,520);
    this->setMinimumSize(1200, 700);
    this->setWindowTitle(tr("LPR Server"));
}

void LPRShowWindow::initConnect()
{
    connect(openVideoButton, &QPushButton::clicked, this, &LPRShowWindow::slotOpenVideo);
    connect(getDataButton, &QPushButton::clicked, this, &LPRShowWindow::slotGetData);
    connect(downloadButton, &QPushButton::clicked, this, &LPRShowWindow::slotDownload);
    connect(udpProcess.get(), &UdpProcess::signalsResultMsg, this, &LPRShowWindow::slotRecvMsg);
    // connect(videoPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(slotPlayErrorMessage()));
    connect(rtspProcess, &RTSPProcess::signalsMsg, this, &LPRShowWindow::slotRecvMsg);
    connect(rtspProcess, &RTSPProcess::signalsImage, this, &LPRShowWindow::slotShowImage);
}

void LPRShowWindow::initData()
{
    this->currentImage = QImage(tr(":/images/play.png"));
    isOpen = false;
    videoPlayer = nullptr;
    playerState = QMediaPlayer::StoppedState;
    rtspProcess = new RTSPProcess();
}
