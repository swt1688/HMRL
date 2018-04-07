#ifndef INITDATABASE_H
#define INITDATABASE_H

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>


#include "helperclass.h"

static QSqlDatabase globalDB(QSqlDatabase::addDatabase("QMYSQL"));

static void Create_UserInfoTable(QSqlDatabase db)
{

    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.UserInfoTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("UserInfoTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表UserInfoTable不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.UserInfoTable(ID int(11) primary key NOT NULL AUTO_INCREMENT,UserSN varchar(20),UserName Varchar(20),Password varchar(30),userLevel varchar(20),sex varchar(2),MobilePhone varchar(12),QQ varchar(11),E_mail text(30),AccePermi varchar(10),CreateDate text(14),OnlineStatus varchar(2),IP_Address varchar(30),IP_Port varchar(8),loginTime timestamp(6))");
         if(success)
         {
             qDebug()<<QObject::tr("数据表创建成功！\n");
             QString pw="123456";
             pw=pw.toUtf8().toBase64();
             query.exec("INSERT INTO `hmrl_erp`.UserInfoTable (`ID`,`UserSN`, `UserName`, `Password`,`userLevel`,`sex`) VALUES ('1','a102252', 'admin', '"+pw+"','店长', '1');");

         }
         else
         {

             qDebug()<<QObject::tr("数据表创建失败\n");
         }

    }
    else
    {
        qDebug()<<QObject::tr("用户表已存在\n");
    }
}
static void Create_UserLoginTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.UserLoginTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("UserLoginTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表UserLoginTable不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.UserLoginTable(ID int(11) primary key not null auto_increment,LoginName varchar(20),LoginTime TIMESTAMP(6) NOT NULL DEFAULT '0000-00-00 00:00:00',LogoutTime TIMESTAMP(6))");
         if(success)
         {
             qDebug()<<QObject::tr("UserLoginTable表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("UserLoginTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("UserLoginTable表已存在\n");
    }
}
/****************************创建入库记录******************************************/
static void Create_StorageRecordTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.StorageRecordTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("StorageRecordTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表UserLogin不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.StorageRecordTable(ID int(11) primary key not null auto_increment,DataTime TIMESTAMP(6) NOT NULL DEFAULT '0000-00-00 00:00:00',ProductName varchar(30),ProductSN varchar(30),SpecificationLevel varchar(10),Unit varchar(8),Weight float,MetagePerson varchar(20),ColdStoreName varchar(20),ColdStoreSN varchar(20),UnitPrice float,TotalAmount double,StroageType varchar(6),RecordPerson varchar(20));");
         if(success)
         {
             qDebug()<<QObject::tr("StorageRecordTable表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("StorageRecordTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("StorageRecordTable表已存在\n");
    }
}

/****************************创建入库记录******************************************/
static void Create_CurrentStorageTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.CurrentStorageTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("CurrentStorageTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表UserLogin不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.CurrentStorageTable(ID int(11) primary key not null auto_increment,ProductName varchar(30),ProductSN varchar(30),SpecificationLevel varchar(10),Unit varchar(8),Weight float,ColdStoreName varchar(20),ColdStoreSN varchar(20),UnitPrice float,TotalAmount double);");
         if(success)
         {
             qDebug()<<QObject::tr("CurrentStorageTable表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("CurrentStorageTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("CurrentStorageTable表已存在\n");
    }
}

/****************************创建冷库记录******************************************/
static void Create_ColdStoreTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.ColdStoreTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("ColdStoreTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表ColdStoreTable不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.ColdStoreTable(ID int(11) primary key not null auto_increment,ColdStoreName varchar(20),ColdStoreSN varchar(20),DataTime TIMESTAMP(6));");
         if(success)
         {
             qDebug()<<QObject::tr("ColdStoreTable表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("ColdStoreTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("ColdStoreTable表已存在\n");
    }
}

/****************************创建产品类型******************************************/
static void Create_PorductInfoTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.PorductInfoTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("PorductInfoTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表PorductInfoTable不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.PorductInfoTable(ID int(11) primary key not null auto_increment,ProductName varchar(20),DataTime TIMESTAMP(6));");
         if(success)
         {
             qDebug()<<QObject::tr("PorductInfoTable表创建成功！\n");
         }
         else
         {
             qDebug()<<QObject::tr("PorductInfoTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("PorductInfoTable表已存在\n");
    }
}

/****************************创建产品价格数据******************************************/
static void Create_PorductPriceTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.PorductPriceTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("PorductPriceTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表PorductPriceTable不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.PorductPriceTable(ID int(11) primary key not null auto_increment,ProductName varchar(20),Quality varchar(20),UnitPrice float,UpdataPerson varchar(20),UpdataTime TIMESTAMP(6));");
         if(success)
         {
             qDebug()<<QObject::tr("PorductPriceTable表创建成功！\n");
         }
         else
         {
             qDebug()<<QObject::tr("PorductPriceTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("PorductPriceTable表已存在\n");
    }
}
/****************************添加客户信息******************************************/
static void Create_CustomerTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM `hmrl_erp`.CustomerTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("CustomerTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表CustomerTable不存在,准备创建表");
         bool success=query.exec("create table `hmrl_erp`.CustomerTable(ID int(11) primary key not null auto_increment,CustomerName varchar(40),CreditCode varchar(20),CustomerAddress varchar(50),BankName varchar(40),BankAccount varchar(15),ContactName varchar(12),ContactPhone varchar(14),UpdataPerson varchar(20),UpdataTime TIMESTAMP(6));");
         if(success)
         {
             qDebug()<<QObject::tr("CustomerTable表创建成功！\n");
         }
         else
         {
             qDebug()<<QObject::tr("CustomerTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("CustomerTable表已存在\n");
    }
}

/*连接数据库,查询已存在的数据据和表*/
static void OpenDatabase()
{
    qDebug()<<QSqlDatabase::drivers()<<endl;

    globalDB.close();
    //db.setHostName("localhost");      //远程IP地址：139.159.225.248<-->本地：localhost
    globalDB.setHostName("139.159.225.248");
    globalDB.setPort(3306);
    globalDB.setUserName("swt518");              //远程不可使用root账户登录,
    globalDB.setPassword("swt123456789");
    globalDB.setDatabaseName("hmrl_erp");

    if(!globalDB.open())
    {
        //HelperClass::ShowMessageBoxError("程序已运行!");
        QMessageBox::critical(0,QObject::tr("Connect to DataBase error:"),globalDB.lastError().text());
        qDebug()<<QObject::tr("Connect to MySql error:")<<globalDB.lastError()<<endl;

    }
    else
    {
        qDebug()<<"OK"<<endl;
        Create_UserInfoTable(globalDB);
        Create_UserLoginTable(globalDB);
        Create_StorageRecordTable(globalDB);
        Create_CurrentStorageTable(globalDB);
        Create_ColdStoreTable(globalDB);
        Create_PorductInfoTable(globalDB);
        Create_PorductPriceTable(globalDB);
        Create_CustomerTable(globalDB);
    }
}


#endif // INITDATABASE_H
