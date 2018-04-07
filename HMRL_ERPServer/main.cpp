#include "spollserver.h"

#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>

#include "globalinfo.h"
#include "DBProcess/dbconnectionpool.h"
#include "helperclass.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setQuitOnLastWindowClosed(false);

    //创建共享内存,判断是否已经运行程序
    QSharedMemory mem("SourcePollutionServer");
    if(!mem.create(1)){
        HelperClass::ShowMessageBoxError("程序已运行!");
        return 1;
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0, QObject::tr("瑞孚雷森"),
            QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }

    GlobalConfig::readGlobalConf();

    SPollServer w;
    QObject::connect(&w, &SPollServer::us_Close, qApp, &QApplication::quit);
    w.show();
    DBConnectionPool::release(); // 程序结束时关闭连接，以免造成连接泄漏
    return a.exec();
}
