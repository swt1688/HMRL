#include "facilityinfo.h"
#include "ui_facilityinfo.h"

#include "logoin.h"
#include "helperclass.h"
//#include <QSql>
#include <QSqlQuery>
#include <QSqlQueryModel>

FacilityInfo::FacilityInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FacilityInfo)
{
    ui->setupUi(this);

    InitFrom();
    QObject::connect(this,&FacilityInfo::AddFacility,this,&FacilityInfo::FacilityEdit);
    QObject::connect(this,&FacilityInfo::AddManufacturer,this,&FacilityInfo::ManufacturerEdit);
}

FacilityInfo::~FacilityInfo()
{
    delete ui;
}
void FacilityInfo::InitFrom()
{
    QSqlQuery query;
    query.exec("SELECT factors_name FROM factorsinfo");

    QStringList facilityName;
    facilityName<<"数据采集传输仪";
    while (query.next())
   {
       facilityName<<query.value(0).toString();
   }
    ui->cboxFacilityName->addItems(facilityName);
    ui->cboxFacilityName->setCurrentIndex(0);


    ManufacturerEdit();
    FacilityEdit();

}

void FacilityInfo::on_btnAdd_clicked()
{
    if(ui->cboxFacilityName->currentText().isEmpty()&& ui->cboxManufacturer->currentText().isEmpty()&&ui->txtFacilityModel->text().isEmpty())
    {
        HelperClass::ShowMessageBoxInfo(tr("必须填满带 * 的内容!"),this);
    }
    else
    {
        QSqlQuery query;//
        bool ok = query.prepare("INSERT INTO facilityinfo (Facility_Name,Manufacturer,ProductModel,DetectionMethod,DetectionLowlimit,DetectionUplimit,Accuracy,unit,CMC_Num,CMC_StartTime,CMC_EndTime,CCAEPI_Num,CCAEPI_StartTime,CCAEPI_EndTime,LoggingTime,LoggingPersonal) VALUES (:Facility_Name,:Manufacturer,:ProductModel,:DetectionMethod,:DetectionLowlimit,:DetectionUplimit,:Accuracy,:unit,:CMC_Num,:CMC_StartTime,:CMC_EndTime,:CCAEPI_Num,:CCAEPI_StartTime,:CCAEPI_EndTime,:LoggingTime,:LoggingPersonal)");

        query.bindValue(":Facility_Name",ui->cboxFacilityName->currentText());
        query.bindValue(":Manufacturer",ui->cboxManufacturer->currentText());
        query.bindValue(":ProductModel",ui->txtFacilityModel->text());

        query.bindValue(":DetectionMethod",ui->txtDetectionMethod->text());
        query.bindValue(":DetectionLowlimit",ui->txtDetectionLowlimit->text());
        query.bindValue(":DetectionUplimit",ui->txtDetectionUplimit->text());

        query.bindValue(":Accuracy",ui->txtAccuracy->text());
        query.bindValue(":unit",ui->txtunit->text());

        query.bindValue(":CMC_Num",ui->txtCMC_Num->text());
        query.bindValue(":CMC_StartTime",ui->cboxCMC_StartTime->date().toString("yyyy-MM-dd"));
        query.bindValue(":CMC_EndTime",ui->cboxCMC_EndTime->date().toString("yyyy-MM-dd"));

        query.bindValue(":CCAEPI_Num",ui->txtCCAEPI_Num->text());
        query.bindValue(":CCAEPI_StartTime",ui->cboxCCAEPI_StartTime->date().toString("yyyy-MM-dd"));
        query.bindValue(":CCAEPI_EndTime",ui->cboxCCAEPI_EndTime->date().toString("yyyy-MM-dd"));

        query.bindValue(":LoggingTime",QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        query.bindValue(":LoggingPersonal",Logoin::CurrentUserName);

        query.setForwardOnly(true);
        query.exec();

        emit AddFacility();
    }
}

void FacilityInfo::on_btnAddManufacturer_clicked()
{
    if(ui->txtManufacturerName->text().isEmpty())
    {
        HelperClass::ShowMessageBoxInfo(tr("必须填满带 * 的内容!"),this);
    }
    else
    {
        //保存到数据库
        QSqlQuery query;//
        SearchCompanyName();
        if(CompanyCount >= 1)
         {
            bool ok = query.exec("UPDATE `onlinedatabase`.enterpriseinfo SET CompanyName='"+ui->txtManufacturerName->text()+"', CompanyAddress='"+ui->txtManufacturerAddress->text()+"', CompanyType='设备厂商', CreditCode='"+ui->txtCreditCode->text()+"', RegisteredCapital='"+ui->txtRegisteredCapital->text()+"',RegisteredDate='"+ui->cboxRegisteredDate->date().toString("yyyy-MM-dd")+"',"
                                                                               "WebAddress='"+ui->txtManufacturerWebAddress->text()+"', EmailAddress='"+ui->txtEmailAddress->text()+"',Functionary01='"+ui->txtContactsName01->text()+"', Mobile01='"+ui->txtContactNumber->text()+"', Functionary02='"+ui->txtEngineer->text()+"', Mobile02='"+ui->txtEngineerNumber->text()+"', Telphone='"+ui->txtTelphone->text()+"', "
                                                                               " Fax='"+ui->txtFax->text()+"', BusinessScope='"+ui->txtBusinessScope->document()->toPlainText()+"', UpdataTime='"+QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")+"', UpdataPersonal='"+Logoin::CurrentUserName+"' where CompanyName = '"+ui->txtManufacturerName->text()+"';");
         }
        else
        {

            bool ok = query.exec("INSERT INTO `onlinedatabase`.enterpriseinfo ( CompanyName, CompanyAddress, CompanyType,  CreditCode,RegisteredCapital,RegisteredDate, WebAddress, EmailAddress,Functionary01, Mobile01, Functionary02, Mobile02, Telphone, Fax, BusinessScope, CreateTime, LoggingPersonal) "
                                                                             "VALUES ('"+ui->txtManufacturerName->text()+"','"+ui->txtManufacturerAddress->text()+"','设备厂商','"+ui->txtCreditCode->text()+"','"+ui->txtRegisteredCapital->text()+"','"+ui->cboxRegisteredDate->date().toString("yyyy-MM-dd")+"','"+ui->txtManufacturerWebAddress->text()+"','"+ui->txtEmailAddress->text()+"',"
                                                                                     "'"+ui->txtContactsName01->text()+"','"+ui->txtContactNumber->text()+"','"+ui->txtEngineer->text()+"','"+ui->txtEngineerNumber->text()+"',"
                                                                                      "'"+ui->txtTelphone->text()+"','"+ui->txtFax->text()+"','"+ui->txtBusinessScope->document()->toPlainText()+"','"+QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")+"','"+Logoin::CurrentUserName+"');");
        }
    }
    emit AddManufacturer();
}
void FacilityInfo::SearchCompanyName()
{
    QSqlQuery query;
    query.exec("SELECT * FROM enterpriseinfo where CompanyName='"+ui->txtManufacturerName->text()+"';");

    CompanyCount=query.numRowsAffected();

}
void FacilityInfo::FacilityEdit()
{
    QSqlQueryModel *modelShow = new QSqlQueryModel;
    modelShow->setQuery("SELECT * FROM FacilityInfo");

    modelShow->setHeaderData(0,Qt::Horizontal,QObject::tr("ID"));
    modelShow->setHeaderData(1,Qt::Horizontal,QObject::tr("设备名称"));
    modelShow->setHeaderData(2,Qt::Horizontal,QObject::tr("生产厂商"));
    modelShow->setHeaderData(3,Qt::Horizontal,QObject::tr("设备型号"));
    modelShow->setHeaderData(4,Qt::Horizontal,QObject::tr("分析方法"));
    modelShow->setHeaderData(5,Qt::Horizontal,QObject::tr("最低检出限"));
    modelShow->setHeaderData(6,Qt::Horizontal,QObject::tr("最高检出限"));
    modelShow->setHeaderData(7,Qt::Horizontal,QObject::tr("准确度"));
    modelShow->setHeaderData(8,Qt::Horizontal,QObject::tr("单位"));
    modelShow->setHeaderData(9,Qt::Horizontal,QObject::tr("计量器具许可证编号"));
    modelShow->setHeaderData(10,Qt::Horizontal,QObject::tr("发证日期"));
    modelShow->setHeaderData(11,Qt::Horizontal,QObject::tr("有效日期"));
    modelShow->setHeaderData(12,Qt::Horizontal,QObject::tr("环保产品认证编号"));
    modelShow->setHeaderData(13,Qt::Horizontal,QObject::tr("发证日期"));
    modelShow->setHeaderData(14,Qt::Horizontal,QObject::tr("有效日期"));
    modelShow->setHeaderData(15,Qt::Horizontal,QObject::tr("录入时间"));
    modelShow->setHeaderData(16,Qt::Horizontal,QObject::tr("录入人员"));
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableView->setModel(modelShow);

    ui->tableView->setAlternatingRowColors(true); //隔行变色

    int row_count = modelShow->rowCount();
    qDebug()<<QObject::tr("当前数据条数")<<row_count;

    ui->tableView->resizeColumnsToContents ();//根据内容自动调整所有列的列宽  ui->tabUserInfo->resizeColumnToContents (0);
}
void FacilityInfo::ManufacturerEdit()
{
    QSqlQueryModel *modelShow = new QSqlQueryModel;
    modelShow->setQuery("SELECT ID,CompanyName,CompanyAddress,CreditCode,RegisteredCapital,RegisteredDate,WebAddress,EmailAddress,"
                        "Functionary01, Mobile01, Functionary02, Mobile02,Telphone,Fax ,BusinessScope ,CreateTime,LoggingPersonal FROM enterpriseinfo where CompanyType='设备厂商';");
    int i=0;
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("ID"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("生产厂商"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("生产商地址"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("营业执照编号"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("注册资金"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("注册日期"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("生产商网址"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("电子邮箱"));

    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("联系人"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("联系电话"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("售后人员"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("售后电话"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("电话"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("传真"));


    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("营业范围"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("录入时间"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("录入人员"));

    ui->tableView_2->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableView_2->setModel(modelShow);

    ui->tableView_2->setAlternatingRowColors(true); //隔行变色

    int row_count = modelShow->rowCount();
    qDebug()<<QObject::tr("当前数据条数")<<row_count;

    ui->tableView_2->resizeColumnsToContents ();//根据内容自动调整所有列的列宽  ui->tabUserInfo->resizeColumnToContents (0);

    QSqlQuery query;//
    query.exec("SELECT CompanyName FROM enterpriseinfo where CompanyType='设备厂商'");
    QStringList ManufacturerName;
    while (query.next())
   {
       ManufacturerName<<query.value(0).toString();
   }
    ui->cboxManufacturer->clear();
    ui->cboxManufacturer->addItems(ManufacturerName);
    ui->cboxManufacturer->setCurrentIndex(0);

}
