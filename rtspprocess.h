#ifndef RTSPPROCESS_H
#define RTSPPROCESS_H

#include <QObject>
#include <QTimer>
#include <QImage>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

class RTSPProcess : public QObject
{
    Q_OBJECT
public:
    explicit RTSPProcess(QObject *parent = nullptr);
    ~RTSPProcess();

    bool start(const QString &url);
    bool stop();

signals:
    void signalsMsg(QString msg);
    void signalsImage(QImage frame);

public slots:
    void getFrame();

private:
    QTimer *timer;
    cv::VideoCapture capture;
    cv::Mat srcFrame;
    QImage srcImage;
};

#endif // RTSPPROCESS_H
