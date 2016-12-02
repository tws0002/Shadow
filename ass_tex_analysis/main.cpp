#include "mainwindow.h"
#include <QApplication>
//#include <QTextCodec>

int main(int argc, char *argv[])
{
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(800, 600);
    w.show();

    return a.exec();
}
