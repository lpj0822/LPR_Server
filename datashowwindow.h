#ifndef DATASHOWWINDOW_H
#define DATASHOWWINDOW_H

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
#include <QListWidget>
#include <QProcess>
#include "utilityGUI/customWindow/mytextbrowser.h"
#include "udpprocess.h"

class DataShowWindow : public QWidget
{
    Q_OBJECT

public:
    DataShowWindow(std::shared_ptr<UdpProcess> &udp, QWidget *parent = 0);
    ~DataShowWindow();

public slots:
    void slotGetLog();
    void slotDownloadLog();
    void slotGetImage();
    void slotDownloadImage();

    void slotDF();
    void slotRM();
    void slotRMLog();

    void slotClearInit();

    void slotRecvMsg(QString msg);
    void slotReadyReadStandardOutput();
    void slotReadyReadStandardError();
    // void slotFinished(int exitCode);
    void slotFinished(int exitCode, QProcess::ExitStatus exitStatus);


private:

    QPushButton *getLogButton;
    QPushButton *downloadLogButton;
    QPushButton *getImageButton;
    QPushButton *downloadImageButton;

    QPushButton *dfButton;
    QPushButton *rmDataButton;
    QPushButton *rmLogButton;

    QPushButton *clearInitButton;

    QListWidget *dataListWidget;

    MyTextBrowser *commandText;//输出黑匣子指令
    QVBoxLayout *mainLayout;//主布局
    QScrollArea *scrollArea;//滚动区域

    std::shared_ptr<UdpProcess> udpProcess;
    QProcess cmdProcess;

    QString address;
    QString saveDir;

    void initUI();
    void initData();
    void initConnect();
};


#endif // DATASHOWWINDOW_H
