#ifndef LPRSHOWWINDOW_H
#define LPRSHOWWINDOW_H

#include <memory>

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QListWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaContent>
#include "udpprocess.h"
#include "rtspprocess.h"
#include "utilityGUI/customWindow/mytextbrowser.h"

class LPRShowWindow : public QWidget
{
    Q_OBJECT

public:
    LPRShowWindow(std::shared_ptr<UdpProcess> &udp, QWidget *parent = 0);
    ~LPRShowWindow();

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void slotOpenVideo();
    void slotGetData();
    void slotDownload();
    void slotRecvMsg(QString msg);
    void slotPlayErrorMessage(QMediaPlayer::Error err);
    void slotShowImage(QImage image);

private:

    QPushButton *openVideoButton;
    QPushButton *getDataButton;
    QPushButton *downloadButton;

    QListWidget *dataListWidget;

    QLabel *videoShowLabel;

    MyTextBrowser *commandText;//输出黑匣子指令
    QVBoxLayout *mainLayout;//主布局
    QScrollArea *scrollArea;//滚动区域

    QVideoWidget *videoWidget;
    QMediaPlayer *videoPlayer;
    QMediaPlayer::State playerState;

    RTSPProcess *rtspProcess;

    QImage currentImage;

    bool isOpen;//是否打开视频

    std::shared_ptr<UdpProcess> udpProcess;

    void initUI();
    void initData();
    void initConnect();
};


#endif // LPRSHOWWINDOW_H
