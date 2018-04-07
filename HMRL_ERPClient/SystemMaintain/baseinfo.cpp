#include "baseinfo.h"
#include "ui_baseinfo.h"
#include "logoin.h"
#include "helperclass.h"
#include "addenterprise.h"

#include "QFrame/tableview.h"


QString BaseInfo::CompanyName="湖北省麻城市惠民肉联有限公司";         //公司名称
QString BaseInfo::CompanyAddress="";     //当前用户密码

QString BaseInfo::Province="";           //当前省份
QString BaseInfo::City="";
QString BaseInfo::Town="";

QString BaseInfo::Longitudo="";            //经度
QString BaseInfo::Latitude="";            //经度
QString BaseInfo::MN_Searil="";            //经度
QString BaseInfo::MN_Password="";
QString BaseInfo::IPAddress="";
int BaseInfo::PortNum=9012;
QString BaseInfo::clientID="";
int BaseInfo::ClientPort=9001;
bool BaseInfo::OnLineStatus=false;
QString BaseInfo::DataBaseDrive="";
//void readBaseInfoConf();

BaseInfo::BaseInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaseInfo)
{
    ui->setupUi(this);

    //显示所有企业信息
    modelShow = new QSqlQueryModel;

    this->Init();
    this->setAttribute (Qt::WA_DeleteOnClose);//QWidget及其派生类的对象，设置QT::WA_DeleteOnClose,当close时会析构该对象
}

BaseInfo::~BaseInfo()
{
    delete ui;
}
void BaseInfo::Init()
{

    modelShow->setQuery("SELECT CompanyName,City,MonitorName,MN_Code,OnlineTime,LogIPAddress,LogPort,LogStatus,LastLoginTime FROM enterpriselist;");
    int i=0;
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("企业名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("城市"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("监测点"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("MN码"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("上限时间"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("IP地址"));
    ui->tableView->setColumnWidth(i,80);
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("端品号"));
    ui->tableView->setColumnWidth(i,80);
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("在线状态"));
     ui->tableView->setColumnWidth(i,80);
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("离线时间"));


    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableView->setModel(modelShow);

    //隔行变色,以下是视觉方面的效果，不加也没影响
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents ();//根据内容自动调整所有列的列宽  ui->tabUserInfo->resizeColumnToContents (0);



}
void BaseInfo::on_btnAffrim_clicked()
{

}

void BaseInfo::on_btnCancel_clicked()
{
    this->close();
}
void BaseInfo::readBaseInfoConf()
{
    QString fileName = Logoin::AppPath+"/localization/config.conf";
    //如果配置文件不存在,则以初始值继续运行
    //没有这个判断的话,配置赋值都为空
    if(!HelperClass::FileIsExist(fileName))
    {
        Logoin::CurrentUserName = Logoin::CurrentUserName.toLatin1();
        return;
    }
    QSettings set(fileName,QSettings::IniFormat);
    set.beginGroup("BaseSetting");

    BaseInfo::CompanyName=set.value("CompanyName").toString();
    BaseInfo::CompanyAddress=set.value("CompanyAddress").toString();

    BaseInfo::Province=set.value("Province").toString();
    BaseInfo::City=set.value("City").toString();
    BaseInfo::Town=set.value("Town").toString();

    BaseInfo::Longitudo=set.value("Longitudo").toString();
    BaseInfo::Latitude=set.value("Latitude").toString();
    BaseInfo::MN_Searil=set.value("MN_Searil").toString();
    BaseInfo::MN_Password=set.value("MN_Password").toString();
    BaseInfo::DataBaseDrive=set.value("DataBaseDrive").toString();

    BaseInfo::ClientPort=set.value("ClientPort").toInt();
    //qDebug()<<"Port"<<BaseInfo::ClientPort;

    set.endGroup();
}

void BaseInfo::on_tableView_clicked(const QModelIndex &index)
{
    //QSqlQueryModel *modelShow = new QSqlQueryModel;
    //modelShow->setQuery("SELECT CompanyName FROM onlinedatabase.enterpriseinfo where CompanyType = '排污单位';");

    recordData = modelShow->record(index.row());
    CompanyName=recordData.value(0).toString();

    qDebug()<<"企业名称"<<CompanyName;
    AddEnterprise *addEnterprise=new AddEnterprise(this,CompanyName);
    //ui->textBrowser->append(tr("设备名称:%1").arg(FactorInfo.uniqueKeys().at(index.row())));
    addEnterprise->exec();
    emit CompanyUpdata(UpdataEnterprise,CompanyName);
    qDebug()<<"Baseinfo又开始运行了哦";
    Init();

}

void BaseInfo::on_btnAdd_clicked()
{
    AddEnterprise *addEnterprise=new AddEnterprise(this,"");
    if(addEnterprise->exec()!= QDialog::Accepted)
    {
        emit CompanyUpdata(1,"");
        //emit AddCompany();
        Init();
    }
}

void BaseInfo::on_btnDelCompany_clicked()
{
    if(recordData.value(0).toString().isEmpty())
    {
        QMessageBox::information(this,tr("提示"),tr("请选择你要删除的企业!"));
        return;
    }
    //
    QString delRow="DELETE FROM `onlinedatabase`.enterpriseinfo  WHERE CompanyName ='"+recordData.value(0).toString()+"';";
    modelShow->setQuery(delRow);
    delRow="DELETE FROM `onlinedatabase`.monitorpointinfo  WHERE CompanyName ='"+recordData.value(0).toString()+"';";
    modelShow->setQuery(delRow);
    delRow="DELETE FROM `onlinedatabase`.monitorfactorinfo  WHERE CompanyName ='"+recordData.value(0).toString()+"';";
    modelShow->setQuery(delRow);
    emit CompanyUpdata(DelEnterprise,recordData.value(3).toString());
    Init();
}
