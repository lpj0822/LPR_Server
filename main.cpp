#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif
#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QSplashScreen>
#include <QDesktopWidget>
#include "simplecontrolwindow.h"
#include "lprshowwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    app.setApplicationVersion("1.0");
    app.setApplicationName("LPR Server");

    //load qss file
    QFile file( ":/style/style/style.qss");
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
    qApp->setWindowIcon(QIcon(":/logo.ico"));

    SimpleControlWindow w;
    w.show();
    w.move((QApplication::desktop()->width() - w.width())/2, (QApplication::desktop()->height() - w.height())/2);

    return app.exec();
}
