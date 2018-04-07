#include "maininterface.h"
#include "logoin.h"
#include "helperclass.h"
#include "initdatabase.h"
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QSharedMemory>

/***************** 说明:主函数入口文件*****************************
 * 功能:打开数据库,初始化配置文件等信息
 * 作者:史文涛  QQ:5649742
 * 时间:2016-09-27  检查:2017-2-21
 ***************************************************************/

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    //QApplication::addLibraryPath("./plugins");
    QString strLibPath(QDir::toNativeSeparators(QApplication::applicationDirPath())+QDir::separator()+"plugins");
    QApplication::addLibraryPath(strLibPath);
    a.setApplicationName("SourceOfPollution");        //设置应用程序名称
    a.setApplicationVersion("V1.0");    //设置应用程序版本
    //a.setWindowIcon(QIcon(":/tree.ico"));

    //创建共享内存,判断是否已经运行程序
    QSharedMemory mem("HMRL_ERPSystem");
    if(!mem.create(1)){
        HelperClass::ShowMessageBoxError("程序已运行!");
        return 1;
    }

    OpenDatabase();                    //连接数据库

    Logoin w;
    HelperClass::SetStyle("blue_Style");//蓝色风格
    w.exec();


    return a.exec();
}


