#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <memory>

#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QProcess>
#include "udpprocess.h"
#include "lprshowwindow.h"
#include "datashowwindow.h"
#include "cmdshowwindow.h"

class SimpleControlWindow : public QWidget
{
    Q_OBJECT

public:
    SimpleControlWindow(QWidget *parent = 0);
    ~SimpleControlWindow();

signals:

public slots:
    void slotOK();//点击确定

private:

    QVBoxLayout *mainLayout;//布局
    QTabWidget *tabweight;//tab
    QDialogButtonBox *buttonBox;

    LPRShowWindow *lprShowWindow;
    DataShowWindow *dataShowWindow;
    CommandShowWindow *cmdShowWindow;

    std::shared_ptr<UdpProcess> udpProcess;

    QProcess cmdProcess;

    void init();
    void initUI();
    void initConnect();
};
#endif // CONTROLWINDOW_H
