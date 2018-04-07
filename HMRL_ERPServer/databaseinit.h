#ifndef DATABASEINIT_H
#define DATABASEINIT_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

//#include <QSettings>
#include <QMessageBox>
#include "Protocol/citycode.h"

void OpenDatabase();
void Create_UserInfoTable(QSqlDatabase db);
void Create_factorsTable(QSqlDatabase db);
void readGobalConf();

void Create_UserInfoTable(QSqlDatabase db)
{

    //query.exec("select * from tables where table_schema=onlineDatabase and table_name=userInfo");
    //query.exec("select table_name from onlineDatabase.tables where table_name='userInfo'");
    QSqlQuery query(db);
    query.exec("SELECT * FROM userinfo");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("userInfo表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表userInfo不存在,准备创建表");
         bool success=query.exec("create table userInfo(ID int(11) primary key,UserName text(20),Password text(15),sex varchar(2),MobilePhone varchar(12),QQ varchar(11),E_mail text(30),CreateDate text(14))");
         if(success)
         {
             qDebug()<<QObject::tr("数据表创建成功！\n");

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
//添加一条数据
  //  query.exec("INSERT INTO `userinfo` (`ID`, `UserName`, `Password`, `sex`) VALUES ('3', '史泰龙', '123456', '1')");
   //修改一条数据
     //query.exec(“UPDATE `userinfo` SET `UserName`='刘德华' WHERE `ID`='1'");
}

void Create_STCodeTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM ST_Code");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("ST_Code表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表ST_Code不存在,准备创建表");
         bool success=query.exec("create table ST_Code(ID int(3) primary key not null auto_increment,ST_Num text(3),ST_Description text(20))");
         if(success)
         {
             qDebug()<<QObject::tr("数据表创建成功！\n");
             //添加一条数据
             query.prepare("insert into ST_Code values (?,?,?)");
             QVariantList ID;
             //ID<<QVariant(QVariant::String);
             ID << 1 << 2 << 3 << 4<<5<<6<<7<<8<<9<<10<<11<<12<<13<<14<<15<<16<<17<<18<<19; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ID);
             QVariantList ST_Num;
             ST_Num << "21" << "22" << "23" << "24"<<"25"<<"26"<<"31"<<"32"<<"33"<<"34"<<"35"<<"36"<<"37"<<"38"<<"39"<<"41"<<"51"<<"52"<<"91"; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ST_Num);
             QVariantList ST_Description;
             ST_Description << "地表水监测" <<"空气质量监测" <<"区域环境噪声监测"<<"地下水环境质量监测"<<"土壤环境质量监测"<<"海水水质监测"<<"大气环境污染源"<<"地表水体环境污染源"
                            <<"地下水体环境污染源"<<"海洋环境污染源"<<"土壤环境污染源"<<"声环境污染源"<<"振动环境污染源"<<"放射性环境污染源"<<"工地扬尘监测"<<"电磁环境污染源"
                            <<"火电厂烟气排放过程监控"<<"城镇污水处理厂污染源排放过程监控"<<"系统交互";
             query.addBindValue(ST_Description);
             if (!query.execBatch()) //进行批处理，如果出错就输出错误
             qDebug() << query.lastError();

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

   //修改一条数据
     //query.exec(“UPDATE `userinfo` SET `UserName`='刘德华' WHERE `ID`='1'");
}

void Create_IPAddressTable(QSqlDatabase db)
{

    QSqlQuery query(db);
    query.exec("SELECT * FROM IPAddress");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("IPAddress表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表IPAddress不存在,准备创建表");
         bool success=query.exec("create table IPAddress(SelectStatus TINYINT(1),IPAddress text(15),Port text(5),ConnectionStatus TINYINT(1))");
         qDebug() << query.lastError();
         if(success)
         {
             qDebug()<<QObject::tr("数据表创建成功！\n");
         }
         else
         {
             qDebug()<<QObject::tr("数据表创建失败\n");
         }

    }
    else
    {
        qDebug()<<QObject::tr("IP表已存在\n");
    }
}

void Create_factorsTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM factorsInfo");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("factorsInfo表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表factorsInfo不存在,准备创建表");

         bool success=query.exec("create table factorsInfo(ID int(4) primary key,ST_CODE varchar(4),factors_name varchar(20),UNIT varchar(10),HJ212_2017 varchar(10),HJ212_2005 varchar(10),HB_V2_5 varchar(10))");
         if(success)
         {
             qDebug()<<QObject::tr("数据表创建成功！\n");
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
void Create_UserLogin(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM UserLogin");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("UserLogin表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表UserLogin不存在,准备创建表");

         //bool success=query.exec("create table UserLogin(ID int(11) primary key not null auto_increment,LoginTime TIMESTAMP(6),LoginName varchar(10),LogoutTime TIMESTAMP(6))");
         bool success=query.exec("create table UserLogin(ID int(11) primary key not null auto_increment,LoginTime TEXT(14),LoginName varchar(10),LogoutTime TEXT(14))");
         if(success)
         {
             qDebug()<<QObject::tr("UserLogin表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("UserLogin表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("UserLogin表已存在\n");
    }
}

void Create_SIMCardRecharge(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM SIMCardRecharge");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("SIMCardRecharge表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表SIMCardRecharge不存在,准备创建表");
         bool success=query.exec("create table SIMCardRecharge(ID int(11) primary key not null auto_increment,DataTime TIMESTAMP(6),SIM_Number varchar(12),Amount SMALLINT)");
         if(success)
         {
             qDebug()<<QObject::tr("SIMCardRecharge表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("SIMCardRecharge表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("SIMCardRecharge表已存在\n");
    }
}

void Create_RemoteControlTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM RemoteControlTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("RemoteControlTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表RemoteControlTable不存在,准备创建表");
         bool success=query.exec("create table RemoteControlTable(ID int(11) primary key  NOT NULL AUTO_INCREMENT,DataTime TIMESTAMP(6),UserName varchar(20),MN_Code varchar(28),CN_Code varchar(8),ControlCommand varchar(1024))");
         if(success)
         {
             qDebug()<<QObject::tr("RemoteControlTable表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("RemoteControlTable表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("RemoteControlTable表已存在\n");
    }
}
void Create_Facilitytable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM FacilityInfo");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("FacilityInfo表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表FacilityInfo不存在,准备创建表");
         bool success=query.exec("create table FacilityInfo(ID int(5) primary key not null auto_increment,Facility_Name varchar(14),Manufacturer text(32),ProductModel varchar(20),DetectionMethod text(20),DetectionLowlimit varchar(10),DetectionUplimit varchar(10),Accuracy varchar(10),unit varchar(8),CMC_Num varchar(20),CMC_StartTime varchar(20),CMC_EndTime varchar(20),CCAEPI_Num varchar(20),CCAEPI_StartTime varchar(20),CCAEPI_EndTime varchar(20),LoggingTime varchar(20),LoggingPersonal varchar(10));");
         if(success)
         {
             qDebug()<<QObject::tr("数据表创建成功！\n");
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
void Create_MonitorPointTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM MonitorPointInfo");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("MonitorPointInfo表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表MonitorInfo不存在,准备创建表");                                                                                                                                                                                                                                                                                                                                                                                            //
         bool success=query.exec("create table MonitorPointInfo(ID int(8) primary key not null auto_increment,CompanyName TEXT(30),MonitorName varchar(20),EnableStatus varchar(6),DischargeRegular text(8),PollutantDirection text(10),OutletType varchar(12),FlatSeg varchar(10),ChannelWidth varchar(10),ThroatWidth varchar(10),MaxFlow varchar(12),StandardText TEXT(30),MN_Code varchar(24) unique,Password text(15),SIM_Card varchar(12),RTU_Manufacturer text(30),RTU_Model varchar(20),RTU_SN varchar(20),ST_Code text(3),Protocol varchar(12),RTU_StartTime TIMESTAMP(6) NOT NULL DEFAULT '0000-00-00 00:00:00' ,RTU_InstallTime TIMESTAMP(6),OnlineTime TIMESTAMP(6),LogIPAddress Text(10),LogPort text(5),LogStatus TINYINT(1),LastLoginTime TIMESTAMP(6),LoggingTime TIMESTAMP(6),LoggingPersonal varchar(10),UpdataTime TIMESTAMP(6),UpdataPersonal varchar(10));");
         if(success)
         {
             qDebug()<<QObject::tr("MonitorPointInfo数据表创建成功！\n");
         }
         else
         {
             qDebug()<<QObject::tr("MonitorPointInfo数据表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("MonitorPointInfo用户表已存在\n");
    }
}


void Create_MonitorfactorTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM MonitorfactorInfo");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("MonitorfactorInfo表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表MonitorfactorInfo不存在,准备创建表");

         bool success=query.exec("create table MonitorfactorInfo(ID int(8) primary key not null auto_increment,CompanyName TEXT(30),MN_Code varchar(24),factorName varchar(20),factordescription varchar(20),factorCode varchar(10),factorNum varchar(2),Manufacturer text(30),ProductModel varchar(20),ProductSN varchar(20),factorUnit varchar(10),WarnlowLimt varchar(10),warnUplimt varchar(10),ProductionDate TIMESTAMP(6) NOT NULL DEFAULT '0000-00-00 00:00:00',InstallDate TIMESTAMP(6),ApplyEnable varchar(4),LoggingTime TIMESTAMP(6),LoggingPersonal varchar(10),UpdataTime TIMESTAMP(6),UpdataPersonal varchar(10));");
         if(success)
         {
             qDebug()<<QObject::tr("MonitorfactorInfo数据表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("MonitorfactorInfo数据表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("MonitorfactorInfo用户表已存在\n");
    }
}
void Create_EnterPriseInfo(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM EnterPriseInfo");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("EnterPriseInfo表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表EnterPriseInfo不存在,准备创建表");
         bool success=query.exec("create table EnterPriseInfo(ID int(11) primary key not null auto_increment,CompanyName TEXT(30),CompanyAddress TEXT(30),CompanyType varchar(5),RegulatorLevel varchar(5), CreditCode varchar(18),RegisteredCapital varchar(10),RegisteredDate text(15),Province TEXT(14),City TEXT(14),Town TEXT(14),WebAddress TEXT(30),EmailAddress TEXT(30),Longitudo TEXT(14),Latitude TEXT(14),Functionary01 TEXT(10),Mobile01 varchar(12),Functionary02 TEXT(10),Mobile02 varchar(12),Telphone varchar(12),Fax varchar(12),CompanyPicture varchar(40),BusinessScope text(500),CreateTime TIMESTAMP(6) NOT NULL DEFAULT '0000-00-00 00:00:00',LoggingPersonal varchar(10),UpdataTime TIMESTAMP(6),UpdataPersonal varchar(10));");
         if(success)
         {
             qDebug()<<QObject::tr("EnterPriseInfo表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("EnterPriseInfo表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("EnterPriseInfo表已存在\n");
    }
}
void Create_ListenPort(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM ListenPortTable");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("ListenPortTable表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表ListenPortTable不存在,准备创建表");
         bool success=query.exec("create table ListenPortTable(SelectStatus TINYINT,PortNum int(5),Description TEXT(20))");
         if(success)
         {
             qDebug()<<QObject::tr("ListenPortTable表创建成功！\n");
         }
         else
         {

             qDebug()<<QObject::tr("ListenPortTable表创建失败%1\n").arg(db.lastError().text());
         }
    }
    else
    {
        qDebug()<<QObject::tr("ListenPortTable表已存在\n");
    }
}


void Create_TempReceiverTable(QSqlDatabase db)
{

    QSqlQuery query(db);
    query.exec("SELECT * FROM TempReceiver");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("TempReceiver表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表TempReceiver不存在,准备创建表");
         bool success=query.exec("create table TempReceiver(DataTime text(15),MN_CODE varchar(24),ReceiverData varchar(1024),Flag varchar(2))");
         qDebug() << query.lastError();
         if(success)
         {
             qDebug()<<QObject::tr("数据表创建成功！\n");
         }
         else
         {
             qDebug()<<QObject::tr("数据表创建失败\n");
         }
    }
    else
    {
        qDebug()<<QObject::tr("TempReceiver表已存在\n");
    }
}
void CreateView(QSqlDatabase db)
{
    QSqlQuery query(db);
    //建立排
    query.exec("create view `onlinedatabase`.MonitorInfo (ST_Code,CompanyName,MN_Code,factorName,factorCode,ApplyEnable,ListenPort) as select  `onlinedatabase`.monitorpointinfo.ST_Code,`onlinedatabase`.monitorpointinfo.CompanyName,`onlinedatabase`.monitorfactorinfo.MN_Code,`onlinedatabase`.monitorfactorinfo.factorName,`onlinedatabase`.monitorfactorinfo.factorCode,`onlinedatabase`.monitorfactorinfo.ApplyEnable,`onlinedatabase`.listenporttable.PortNum from `onlinedatabase`.monitorpointinfo  LEFT outer join `onlinedatabase`.monitorfactorinfo on `onlinedatabase`.monitorpointinfo.MN_Code= `onlinedatabase`.monitorfactorinfo.MN_Code left outer join `onlinedatabase`.enterpriseinfo on `onlinedatabase`.monitorpointinfo.CompanyName=`onlinedatabase`.enterpriseinfo.CompanyName left outer join `onlinedatabase`.listenporttable on `onlinedatabase`.enterpriseinfo.City REGEXP `onlinedatabase`.listenporttable.Description order by CompanyName;");
    //创建排污企业视图
    query.exec("create view EnterpriseList (CompanyName,RegulatorLevel,Province,City,Town,ListenPort,MonitorName,EnableStatus,ST_Code,MN_Code,Password,OnlineTime,LogIPAddress,LogPort,LogStatus,LastLoginTime) as select enterpriseinfo.CompanyName,enterpriseinfo.RegulatorLevel,enterpriseinfo.Province,enterpriseinfo.City,enterpriseinfo.Town,listenporttable.PortNum,monitorpointinfo.MonitorName,monitorpointinfo.EnableStatus,monitorpointinfo.ST_Code,monitorpointinfo.MN_Code,monitorpointinfo.Password,monitorpointinfo.OnlineTime,monitorpointinfo.LogIPAddress,monitorpointinfo.LogPort,monitorpointinfo.LogStatus,monitorpointinfo.LastLoginTime from enterpriseinfo  LEFT outer JOIN  monitorpointinfo on enterpriseinfo.CompanyName=monitorpointinfo.CompanyName left join listenporttable on  listenporttable.Description=enterpriseinfo.city  where enterpriseinfo.CompanyType='排污单位' order by listenporttable.Description;");
    query.exec("CREATE VIEW factorInfo (CompanyName,MN_Code,factordescription,Facility_Name,ProductModel,DetectionMethod,DetectionLowlimit,DetectionUplimit,CMC_Num,CMC_StartTime,CMC_EndTime,CCAEPI_Num,CCAEPI_StartTime,CCAEPI_EndTime,ProductSN,InstallDate,enterpriseName,CompanyAddress,WebAddress,Telphone,Mobile01,Functionary02) as select monitorfactorinfo.CompanyName,monitorfactorinfo.MN_Code,monitorfactorinfo.factordescription,facilityinfo.Facility_Name,facilityinfo.ProductModel,facilityinfo.DetectionMethod,facilityinfo.DetectionLowlimit,facilityinfo.DetectionUplimit,facilityinfo.CMC_Num,facilityinfo.CMC_StartTime,facilityinfo.CMC_EndTime,facilityinfo.CCAEPI_Num,facilityinfo.CCAEPI_StartTime,facilityinfo.CCAEPI_EndTime,monitorfactorinfo.ProductSN,monitorfactorinfo.InstallDate,enterpriseinfo.CompanyName,enterpriseinfo.CompanyAddress,enterpriseinfo.WebAddress,enterpriseinfo.Telphone,enterpriseinfo.Mobile01,enterpriseinfo.Functionary02 from facilityinfo,monitorfactorinfo,enterpriseinfo where facilityinfo.Facility_Name=monitorfactorinfo.factorName and monitorfactorinfo.Manufacturer=enterpriseinfo.CompanyName and monitorfactorinfo.ProductModel=facilityinfo.ProductModel;");

}
/****************************创建现场端信息编码*********************************/
void Create_DevInfoCodeTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM DevInfoCode");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("DevInfoCode表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表DevInfoCode不存在,准备创建表");
         bool success=query.exec("create table DevInfoCode(ID int(4) primary key not null auto_increment,DevType int(2),InfoType int(2),InfoCode varchar(10),InfoName varchar(100),infoUnit varchar(10),ExtendCode varchar(10));");
         if(success)
         {
             qDebug()<<QObject::tr("DevInfoCode数据表创建成功！\n");
             //添加一条数据
             query.prepare("insert into DevInfoCode values (?,?,?,?,?,?,?)");
             QVariantList ID;
             //ID<<QVariant(QVariant::String);
             for(int i=1;i<=79;i++)
             {
                 ID<<QString::number(i);
             } // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ID);
             QVariantList DevType;
             DevType <<"1"<<"1"<<"1"<<"1"<<"1"<<"1"<<"1"<<"1"
                     <<"1"<<"1"<<"1"<<"1"<<"1"<<"1"<<"1"<<"1"
                     <<"1"<<"1"<<"1"<<"1"<<"1"<<"1"
                     <<"2"<<"2"<<"2"<<"2"<<"2"<<"2"
                     <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                     <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                     <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                     <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                     <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                     <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                     <<"3"<<"3"<<"3"; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(DevType);
             QVariantList InfoType;
             InfoType <<"1"<<"2"<<"2"<<"2"<<"2"<<"2"<<"3"<<"3"
                      <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                      <<"5"<<"6"<<"6"<<"6"<<"6"<<"6"
                      <<"1"<<"2"<<"2"<<"2"<<"2"<<"3"
                      <<"1"<<"2"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                      <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                      <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                      <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                      <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                      <<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"<<"3"
                      <<"3"<<"3"<<"3";
             query.addBindValue(InfoType);

             QVariantList InfoCode;
             InfoCode <<"i11001"<<"i12001"<<"i12002"<<"i12003"<<"i12010"<<"i13001"<<"i13002"<<"i13003"
                      <<"i13004"<<"i13005"<<"i13006"<<"i13007"<<"i13008"<<"i13009"<<"i13100"<<"i13101"
                      <<"i15013"<<"i16001"<<"i16002"<<"i16003"<<"i16004"<<"i16010"
                      <<"i21001"<<"i22001"<<"i22002"<<"i22003"<<"i22101"<<"i23001"
                      <<"i31001"<<"i32001"<<"i33001"<<"i33002"<<"i33101"<<"i33102"<<"i33103"<<"i33104"
                      <<"i33105"<<"i33106"<<"i33107"<<"i33200"<<"i33201"<<"i33202"<<"i33311"<<"i33312"
                      <<"i33313"<<"i33314"<<"i33315"<<"i33316"<<"i33317"<<"i33318"<<"i33319"<<"i33321"
                      <<"i33322"<<"i33323"<<"i33324"<<"i33325"<<"i33326"<<"i33327"<<"i33328"<<"i33329"
                      <<"i33331"<<"i33332"<<"i33333"<<"i33334"<<"i33335"<<"i33336"<<"i33337"<<"i33338"
                      <<"i33339"<<"i33341"<<"i33342"<<"i33343"<<"i33344"<<"i33345"<<"i33346"<<"i33347"
                      <<"i33348"<<"i33349"<<"i33400"; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(InfoCode);

             QVariantList InfoName;
             InfoName <<"运行日志"<<"工作状态"<<"分析仪与数采仪通讯状态"<<"分析仪报警状态"<<"分析当前动作"<<"测量量程"<<"测量精度"<<"测量间隔"
                      <<"消解温度"<<"消解时长"<<"校准时间"<<"截距"<<"斜率"<<"测量检出限"<<"当前曲线相关系数"<<"仪器当前温度"
                      <<"标1浓度"<<"实时光电信号与实时吸光度信号"<<"水样吸光度值"<<"零样吸光度值"<<"标样吸光度值"<<"20mA输出值"
                      <<"运行日志"<<"工作状态"<<"用户状态"<<"数采仪与上位机通读状态"<<"数采仪通道通讯状态"<<"本地大气压力"
                      <<"门禁日志"<<"门禁状态"<<"CEMS伴热管温度"<<"CEMS冷凝温度"<<"监测站房温度"<<"监测站房湿度"<<"监测站房电压"<<"监测站房原水压力"
                      <<"监测站房进样压力1"<<"监测站房进样压力2"<<"沉砂池清洗时间"<<"污水处理站(厂)电流量"<<"污水处理站(厂)累计耗电量"<<"污水处理站(厂)日耗电量"<<"炉膛内上部焚烧温度1"<<"炉膛内上部焚烧温度2"
                      <<"炉膛内上部焚烧温度3"<<"炉膛内上部焚烧温度4"<<"炉膛内上部焚烧温度5"<<"炉膛内上部焚烧温度6"<<"炉膛内上部焚烧温度7"<<"炉膛内上部焚烧温度8"<<"炉膛内上部焚烧温度9"<<"炉膛内中部焚烧温度1"
                      <<"炉膛内中部焚烧温度2"<<"炉膛内中部焚烧温度3"<<"炉膛内中部焚烧温度4"<<"炉膛内中部焚烧温度5"<<"炉膛内中部焚烧温度6"<<"炉膛内中部焚烧温度7"<<"炉膛内中部焚烧温度8"<<"炉膛内中部焚烧温度9"
                      <<"炉膛内下部焚烧温度1"<<"炉膛内下部焚烧温度2"<<"炉膛内下部焚烧温度3"<<"炉膛内下部焚烧温度4"<<"炉膛内下部焚烧温度5"<<"炉膛内下部焚烧温度3"<<"炉膛内下部焚烧温度7"<<"炉膛内下部焚烧温度8"
                      <<"炉膛内下部焚烧温度9"<<"炉膛内二次空气喷入点温度1"<<"炉膛内二次空气喷入点温度2"<<"炉膛内二次空气喷入点温度3"<<"炉膛内二次空气喷入点温度4"<<"炉膛内二次空气喷入点温度5"<<"炉膛内二次空气喷入点温度6"<<"炉膛内二次空气喷入点温度7"
                      <<"炉膛内二次空气喷入点温度8"<<"炉膛内二次空气喷入点温度9"<<"生产负荷"; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(InfoName);

             QVariantList infoUnit;
             infoUnit <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<"分钟"
                     <<"℃"<<"分钟"<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<"℃"
                     <<"mg/L"<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<"mg/L"
                     <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<"KPa"
                     <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<"℃"<<"℃"<<"℃"<<"%"<<"V"<<"KPa"
                     <<"KPa"<<"KPa"<<"秒"<<"A"<<"KWh"<<"KWh"<<"℃"<<"℃"
                     <<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"
                     <<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"
                     <<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"
                     <<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"<<"℃"
                     <<"℃"<<"℃"<<"%"; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(infoUnit);

             QVariantList ExtendCode;
             for(int i=1;i<=79;i++)
             {
                 ExtendCode<<QVariant(QVariant::String);
             } // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ExtendCode);

             if (!query.execBatch()) //进行批处理，如果出错就输出错误
             qDebug() << query.lastError();

         }
         else
         {

             qDebug()<<QObject::tr("DevInfoCode表创建失败\n");
         }

    }
    else
    {
        qDebug()<<QObject::tr("DevInfoCode表已存在\n");
    }
}

/****************************创建现场端信息描述*********************************/
void Create_DevInfoDescTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM DevInfoDesc");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("DevInfoDesc表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表DevInfoDesc不存在,准备创建表");
         bool success=query.exec("create table DevInfoDesc(ID int(4) primary key not null auto_increment,PollCode varchar(10),InfoCode varchar(10),InfoValue int(3),PollInfoDesc varchar(48),ReasonCode int(3));");
         if(success)
         {
             qDebug()<<QObject::tr("DevInfoDesc数据表创建成功！\n");
             //添加一条数据
             query.prepare("insert into DevInfoDesc values (?,?,?,?,?,?)");
             QVariantList ID;
             //ID<<QVariant(QVariant::String);
             for(int i=1;i<=105;i++)
             {
                 ID<<QString::number(i);
             } // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ID);
             QVariantList PollCode;
             PollCode <<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"
                      <<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"
                      <<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"
                      <<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"
                      <<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"
                      <<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"
                      <<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"<<"w01018"
                      <<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"<<"w21003"
                      <<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"<<"w21011"
                        ; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(PollCode);

             QVariantList InfoCode;
             InfoCode <<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"
                      <<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"
                      <<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"<<"i12001"
                      <<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"
                      <<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"
                      <<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"<<"i12010"
                      <<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"
                      <<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"
                      <<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"<<"i12003"
                        ; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(InfoCode);

             QVariantList InfoValue;
             InfoValue <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"
                       <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"
                       <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"
                       <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"
                       <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"
                       <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"
                       <<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"<<"13"<<"14"
                       <<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"<<"13"<<"14"
                       <<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"<<"13"<<"14"
                        ;
             query.addBindValue(InfoValue);

             QVariantList PollInfoDesc;
             PollInfoDesc <<"单次测量"<<"等间隔循环测量"<<"指定时间测量"<<"远程控制测量"<<"空闲"<<"手动控制"<<"校准"<<"异常停机"
                          <<"单次测量"<<"等间隔循环测量"<<"指定时间测量"<<"远程控制测量"<<"空闲"<<"手动控制"<<"校准"<<"异常停机"
                          <<"单次测量"<<"等间隔循环测量"<<"指定时间测量"<<"远程控制测量"<<"空闲"<<"手动控制"<<"校准"<<"异常停机"
                          <<"待机"<<"测量准备"<<"首次清洗"<<"进水样"<<"进掩敝剂"<<"进氧化剂"<<"进催化剂"<<"加热恒温"<<"冷却"<<"排液"<<"未次清洗"<<"待机"<<"清洗"
                          <<"待机"<<"测量准备"<<"首次清洗"<<"进水样"<<"进掩敝剂"<<"进氧化剂"<<"进催化剂"<<"加热恒温"<<"冷却"<<"排液"<<"未次清洗"<<"待机"<<"清洗"
                          <<"待机"<<"测量准备"<<"首次清洗"<<"进水样"<<"进掩敝剂"<<"进氧化剂"<<"进催化剂"<<"加热恒温"<<"冷却"<<"排液"<<"未次清洗"<<"待机"<<"清洗"
                          <<"运行正常"<<"未采到水样"<<"加热超过设定值"<<"加热达不到设定值"<<"未采到氧化剂"<<"未采到催化剂"<<"未采到掩蔽剂"<<"未采到量标样"<<"未采到零标样"<<"消解杯管路异常"<<"排液异常"<<"光检器件故障"<<"废液桶满"<<"水样管内压力过大"
                          <<"运行正常"<<"未采到水样"<<"加热超过设定值"<<"加热达不到设定值"<<"未采到氧化剂"<<"未采到催化剂"<<"未采到掩蔽剂"<<"未采到量标样"<<"未采到零标样"<<"消解杯管路异常"<<"排液异常"<<"光检器件故障"<<"废液桶满"<<"水样管内压力过大"
                          <<"运行正常"<<"未采到水样"<<"加热超过设定值"<<"加热达不到设定值"<<"未采到氧化剂"<<"未采到催化剂"<<"未采到掩蔽剂"<<"未采到量标样"<<"未采到零标样"<<"消解杯管路异常"<<"排液异常"<<"光检器件故障"<<"废液桶满"<<"水样管内压力过大"

                            ; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(PollInfoDesc);

             QVariantList ReasonCode;
             ReasonCode <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)
                       <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)
                       <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)
                       <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)
                       <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)
                       <<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)<<QVariant(QVariant::String)
                       <<QVariant(QVariant::String)<<"1"<<"2"<<"1"<<"3"<<"3"<<"3"<<"3"<<"3"<<"6"<<"3"<<"5"<<"7"<<"8"
                       <<QVariant(QVariant::String)<<"1"<<"2"<<"1"<<"3"<<"3"<<"3"<<"3"<<"3"<<"6"<<"3"<<"5"<<"7"<<"8"
                       <<QVariant(QVariant::String)<<"1"<<"2"<<"1"<<"3"<<"3"<<"3"<<"3"<<"3"<<"6"<<"3"<<"5"<<"7"<<"8"
                           ; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ReasonCode);


             if (!query.execBatch()) //进行批处理，如果出错就输出错误
             qDebug() << query.lastError();

         }
         else
         {

             qDebug()<<QObject::tr("DevInfoDesc表创建失败\n");
         }

    }
    else
    {
        qDebug()<<QObject::tr("DevInfoDesc表已存在\n");
    }
}

/****************************创建现场端问题解决方案*********************************/
void Create_DevSolutionTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM DevSolution");
    QSqlRecord rec = query.record();
    qDebug() << QObject::tr("DevSolution表字段数: ")<<rec.count();
    if(rec.count()==0)
    {
         qDebug()<<QObject::tr("表DevSolution不存在,准备创建表");
         bool success=query.exec("create table DevSolution(ID int(4) primary key not null auto_increment,ProductModel varchar(300),ReasonCode int(3),ReasonAnalysis varchar(300),SolutionDesc varchar(1024));");
         if(success)
         {
             qDebug()<<QObject::tr("DevSolution数据表创建成功！\n");
             //添加一条数据
             query.prepare("insert into DevSolution values (?,?,?,?,?)");
             QVariantList ID;
             //ID<<QVariant(QVariant::String);
             for(int i=1;i<=8;i++)
             {
                 ID<<QString::number(i);
             } // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ID);
             QVariantList ProductModel;
             ProductModel <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                          <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                          <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                          <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                          <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                          <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                          <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                          <<"SW-OAWQ-COD,SW-OAWQ-NH3,STEP-NH3-N,SW-OAWQ-TP"
                        ; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(ProductModel);

             QVariantList ReasonCode;
             ReasonCode <<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"
                        ;
             query.addBindValue(ReasonCode);

             QVariantList ReasonAnalysis;
             ReasonAnalysis <<"1、无温度显示或显示温度偏差较大 2、加热丝故障 3、主板故障"
                            <<"1、进样过量"
                           <<"1、无相应的试剂 2、管路堵塞（水样悬浮物、药剂未完全溶解）3、密封圈失效、管路漏气 4、液位传感器故障 5、蠕动故障 6、电机驱动器故障 7、电磁阀故障 8、主板故障"
                           <<"1、冷却风扇故障 2、环境温度过高 3、铂电阻故障 4、主板故障"
                           <<"1、光检板故障 2、LED不亮 3、主板故障 4、光检输出信号饱和 5、光检板供电异常 6、样品浓度过高"
                           <<"1、检查消解电磁阀是否损坏 2、导气瓶内有水"
                           <<"1、废液传感器损坏 2、废液桶满"
                           <<"1、排液阀损坏 2、采样管路水压过大 3、液位传感器故障 4、计量管内有异物"
                        ;
             query.addBindValue(ReasonAnalysis);

             QVariantList SolutionDesc;
             SolutionDesc <<"1、检查温度传器接线或更换温度传感器 2、检查连线或更换加热丝 3、检查前后门是否关好、检查主板或更换主板"
                          <<"调整采水管路水压"
                          <<"1、补足相应试剂 2、更换堵塞管路，更换相应的试剂，确保外接采水泵的2个出水口畅通。 3、更换密封圈、疏通管路、上紧管路接头；检查计量模块和消解模块的紧密螺丝是否正确上紧； 4、检查连线或更换液位传感器 5、检查蠕动泵及蠕动泵软管老化和连接状况以确定更换蠕动泵或软管、重新连接； 6、检查连线或更换驱动器 7、检查相应的阀路是否畅通；检查相应的连线是否正确；检查相应的电磁阀是否老化失效。 8、检查或更换主板"
                          <<"1、检查连线是否正常或更换风扇 2、设法降低环境温度 3、检查连线或更换铂电阻 4、检查连线或更换主板"
                          <<"1、检查或更换光检板 2、检查连线或更换LED 3、检查或更换主板 4、调节光检板的反馈电阻或更换光检板 5、更换开关电源 6、切换工作量程"
                          <<"1、更换电磁阀 2、清理导气瓶"
                          <<"1、更换废液检测传感器 3、清理废液"
                          <<"1、更换排液阀 2、调整采水管路水压 3、更换液位传感器 4、清洗液位计量客"
                           ; // 如果要提交空串，用QVariant(QVariant::String)代替名字
             query.addBindValue(SolutionDesc);


             if (!query.execBatch()) //进行批处理，如果出错就输出错误
             qDebug() << query.lastError();

         }
         else
         {

             qDebug()<<QObject::tr("DevSolution表创建失败\n");
         }

    }
    else
    {
        qDebug()<<QObject::tr("DevSolution表已存在\n");
    }
}

/*连接数据库,查询已存在的数据据和表*/
void OpenDatabase(QString HostName,int DB_Prot,QString userName,QString DB_PW,QString DB_Name)
{

    qDebug()<<QSqlDatabase::drivers()<<endl;
    QSqlDatabase db(QSqlDatabase::addDatabase("QMYSQL"));
    db.setHostName(HostName);      //远程IP地址：139.159.225.248<-->本地：localhost
    //db.setHostName("139.159.225.248");
    db.setPort(DB_Prot);
    db.setUserName(userName);              //远程不可使用root账户登录,
    db.setPassword(DB_PW);
    db.setDatabaseName(DB_Name);
    QSqlQuery query(db);
    //query.exec("CREATE SCHEMA `onlinedatabase`;");//创建数数库("CREATE SCHEMA `"+ui->ledMN->text()+"`;");//删了DROP DATABASE `4321012`;

    if(!db.open())
    {
        //HelperClass::ShowMessageBoxError("程序已运行!");
        QMessageBox::critical(0,QObject::tr("Connect to DataBase error:"),db.lastError().text());
        qDebug()<<QObject::tr("Connect to MySql error:")<<db.lastError()<<endl;

    }
    else
    {
        //QMessageBox::information(0, QObject::tr("Tips"), QObject::tr(" 连接数据库成功！！！ "));
        qDebug()<<"OK"<<endl;
    }

    //query.exec("CREATE DATABASE swtDB");//创建数据库
    query.exec("show databases");//获取已存在的数据库名
    while (query.next()) {
        qDebug() << QObject::tr("已存在的数据库:") << query.value(0).toString();
    }
    query.exec("show tables;");//获取数据库中所有的表名
    while (query.next())
    {
        qDebug() << QObject::tr("已存在的表名:") << query.value(0).toString();
    }
    Create_UserInfoTable(db);
    Create_factorsTable(db);
    Create_UserLogin(db);
    Create_SIMCardRecharge(db);
    Create_RemoteControlTable(db);
    Create_STCodeTable(db);
    Create_IPAddressTable(db);
    Create_EnterPriseInfo(db);
    Create_ChinaCity(db);
    Create_Facilitytable(db);
    Create_MonitorPointTable(db);
    Create_MonitorfactorTable(db);
    Create_ListenPort(db);
    Create_DevInfoCodeTable(db);
    Create_DevInfoDescTable(db);
    Create_DevSolutionTable(db);
    //Create_TempReceiverTable(db);//创建接收到的临时表
    CreateView(db);
    //每次启动时将所有企业设置为离线状态，服务器端才需设置此功能
    query.exec("UPDATE `onlinedatabase`.monitorpointinfo SET  LogStatus ='0';");
    //每次启动时将所有IP端口设为未连接，服务器端才需设置此功能
    query.exec("UPDATE `onlinedatabase`.IPAddress SET  ConnectionStatus ='0';");
}



#endif // DATABASEINIT_H
