#include "globalinfo.h"

#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QCryptographicHash>

#include "databaseinit.h"
#include "DBProcess/dbconnectionpool.h"
//#include "spollserver.h"



GlobalConfig::GlobalConfig()
{

}
void GlobalConfig::readGlobalConf()
{

    // 创建数据库连接的这些信息在实际开发的时都需要通过读取配置文件得到，
    // 这里为了演示方便所以写死在了代码里。
    QString FileName=QApplication::applicationDirPath()+"/localization/config.conf";
    qDebug()<<"文件路劲"<<FileName;
    QFile tempFile(FileName);
    if(tempFile.exists())
    {
        QSettings *set = new QSettings(FileName,QSettings::IniFormat);
        set->beginGroup("BaseSetting");

        DBConnectionPool::DB_HostAddress=set->value("DB_HostAddress").toString();//数据库IP地址
        DBConnectionPool::DB_Name=set->value("DB_Name").toString();//数据库名称
        DBConnectionPool::DB_UserName=set->value("DB_UserName").toString();//数据库名称
        DBConnectionPool::DB_Port=set->value("DB_Port").toInt();//数据库端口
        DBConnectionPool::DB_Password=QByteArray::fromBase64(set->value("DB_PW").toByteArray());//数据库密码
        qDebug()<<"PW_FB64"<<DBConnectionPool::DB_Password;

        //对数据库密码进行MD5加密，但不可解密，在此存在问题
        //QByteArray hash_byte_array = QCryptographicHash::hash(DatabasePW.toUtf8(), QCryptographicHash::Md5);
        //QString md5 = hash_byte_array.toHex();

        //*******对数据库密码进行解密
        set->endGroup();
    }
    else
    {

        QSettings *set = new QSettings(FileName,QSettings::IniFormat);
        set->beginGroup("BaseSetting");
        set->setValue("DB_HostAddress",DBConnectionPool::DB_HostAddress);
        set->setValue("DB_Name",DBConnectionPool::DB_Name);
        set->setValue("DB_UserName",DBConnectionPool::DB_UserName);
        set->setValue("DB_Port",DBConnectionPool::DB_Port);
        QString pw=DBConnectionPool::DB_Password.toUtf8().toBase64();
        set->setValue("DB_PW",pw);

        qDebug()<<"DB_PW TO64"<<pw;

        set->setValue("LoginTime",QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        set->endGroup();
        qDebug()<<"文件写入完毕"<<endl;
    }
//    QSqlDatabase db = DBConnectionPool::openConnection();
//    qDebug() << "In thread run():" << db.connectionName();
    OpenDatabase(DBConnectionPool::DB_HostAddress,DBConnectionPool::DB_Port,DBConnectionPool::DB_UserName,DBConnectionPool::DB_Password,DBConnectionPool::DB_Name);
//    QSqlQuery query(db);
//    QString strs="SELECT * FROM `onlinedatabase`.ListenPortTable where selectStatus ='1';";
//    query.exec(strs);
//    while(query.next())
//    {
//        SPollServer::ListenPortlist.append(query.value(1).toUInt());
//    }
//    strs="SELECT * FROM `onlinedatabase`.ipaddress where selectStatus ='1';";
//    query.exec(strs);
//    while(query.next())
//    {
//        SPollServer::UploadIP.append(tr("%1:%2").arg(query.value(1).toString()).arg(query.value(2).toInt()));
//    }
}
