#include "rtspprocess.h"

RTSPProcess::RTSPProcess(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
}

RTSPProcess::~RTSPProcess()
{
    timer->stop();
    timer->deleteLater();
    if(capture.isOpened())
    {
        capture.release();
    }
}

bool RTSPProcess::start(const QString &url)
{
    if(capture.isOpened())
    {
        timer->stop();
        capture.release();
        disconnect(timer, &QTimer::timeout, this, &RTSPProcess::getFrame);
    }
    if(!capture.open(url.toStdString().c_str()))
    {
        emit signalsMsg(QString("open %1 fail!").arg(url));
        return false;
    }
    if(capture.isOpened())
    {
        emit signalsMsg(QString("Video FPS %1\nVideo Size %2 %3").arg(capture.get(cv::CAP_PROP_FPS)) \
                        .arg(capture.get(cv::CAP_PROP_FRAME_WIDTH)). \
                        arg(capture.get(cv::CAP_PROP_FRAME_HEIGHT)));
        timer->start(10);
        connect(timer, &QTimer::timeout, this, &RTSPProcess::getFrame);
        return true;
    }
    else
    {
        emit signalsMsg(QString("open %1").arg(url));
        return false;
    }
}

bool RTSPProcess::stop()
{
    timer->stop();
    if(capture.isOpened())
    {
        capture.release();
    }
    disconnect(timer, &QTimer::timeout, this, &RTSPProcess::getFrame);
    return true;
}

void RTSPProcess::getFrame()
{
    if(! capture.read(srcFrame))
    {
        stop();
        emit signalsMsg("get fream fail!");
    }
    else
    {
        if(srcFrame.channels() == 3)
        {
            // RGB image
            cvtColor(srcFrame, srcFrame, cv::COLOR_BGR2RGB);
            srcImage = QImage((const uchar*)(srcFrame.data),  //(const unsigned char*)
                              srcFrame.cols, srcFrame.rows,
                              srcFrame.cols*srcFrame.channels(),   //new add
                              QImage::Format_RGB888);
        }
        else
        {
            // gray image
            srcImage = QImage((const uchar*)(srcFrame.data),
                              srcFrame.cols, srcFrame.rows,
                              srcFrame.cols*srcFrame.channels(),    //new add
                              QImage::Format_Indexed8);
        }
        emit signalsImage(srcImage);
    }
}

