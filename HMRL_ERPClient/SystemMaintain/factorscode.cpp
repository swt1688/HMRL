#include "factorscode.h"
#include "ui_factorscode.h"
#include <QSqlQueryModel>
#include <QModelIndex>
#include <QSqlRecord>
#include <QTableView>
#include <QDebug>


FactorsCode::FactorsCode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FactorsCode)
{
    ui->setupUi(this);
    this->showFactorsInfo();
}

FactorsCode::~FactorsCode()
{
    delete ui;
}
void FactorsCode::showFactorsInfo()
{
    QSqlQueryModel *modelShow = new QSqlQueryModel;
    modelShow->setQuery("SELECT * FROM factorsInfo");

    modelShow->setHeaderData(0,Qt::Horizontal,QObject::tr("ID"));
    modelShow->setHeaderData(1,Qt::Horizontal,QObject::tr("ST码"));
    modelShow->setHeaderData(2,Qt::Horizontal,QObject::tr("因子名称"));
    modelShow->setHeaderData(3,Qt::Horizontal,QObject::tr("单位"));
    modelShow->setHeaderData(4,Qt::Horizontal,QObject::tr("GB212-2017"));
    modelShow->setHeaderData(5,Qt::Horizontal,QObject::tr("GB212-2005"));
    modelShow->setHeaderData(6,Qt::Horizontal,QObject::tr("湖北代码"));
    modelShow->setHeaderData(7,Qt::Horizontal,QObject::tr("自定义"));

    ui->tabFactorsInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tabFactorsInfo->setModel(modelShow);

    ui->tabFactorsInfo->setAlternatingRowColors(true); //隔行变色

    int row_count = modelShow->rowCount();
    qDebug()<<QObject::tr("当前数据条数")<<row_count;

    ui->tabFactorsInfo->resizeColumnsToContents ();//根据内容自动调整所有列的列宽  ui->tabUserInfo->resizeColumnToContents (0);

}

void FactorsCode::on_btnFactorsAdd_clicked()
{
    QString factorsID = QString::number((mysql->SearchMaxValue("ID","factorsInfo") + 1), 10);
    ui->ledFactorsID->setText(factorsID);
//    QString CustomerDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
//    ui->ledCreateDate->setText(CustomerDate);
}

void FactorsCode::on_btnFactorsSave_clicked()
{
    QString factorsID = ui->ledFactorsID->text();//因子编码
    QString factorsName = ui->ledFactorsName->text();//因子名称
    QString factorsGB = ui->ledFactorsGB->text();//因子代码1
    QString factorsHB = ui->ledFactorsHB->text();//因子代码2
    QString factorsREF = ui->ledFactorsREF->text();//因子代码3

    if(factorsName.isEmpty())
    {
        HelperClass::ShowMessageBoxInfo(tr("必须填入因子名称!"),this);
    }
    else
    {
        QSqlQuery query;
        bool ok = query.prepare("INSERT INTO factorsinfo (ID, factors_name,factors_GBCODE,factors_HBCODE,factors_REFCODE) VALUES (:ID,:factors_name,:factors_GBCODE,:factors_HBCODE,:factors_REFCODE)");
        query.bindValue(":ID",factorsID);
        query.bindValue(":factors_name",factorsName);
        query.bindValue(":factors_GBCODE",factorsGB);
        query.bindValue(":factors_HBCODE",factorsHB);
        query.bindValue(":factorsREFCODE",factorsREF);

        query.setForwardOnly(true);
        query.exec();
        showFactorsInfo();
//        if(ok)
//        {
//            ui->showResult->setText(tr("用户注册成功"));
//            //HelperClass::MyLoginBlog("logblog","注册","新用户注册",customerName);
//            //HelperClass::ShowMessageBoxInfo(tr("注册成功!"));
//        }
//        else
//        {
//            ui->showResult->setText(tr("注册失败"));
//        }
    }
}

void FactorsCode::on_tabFactorsInfo_clicked(const QModelIndex &index)
{
    QSqlQueryModel *modelShow = new QSqlQueryModel;
    modelShow->setQuery("SELECT * FROM onlinedatabase.factorsInfo");

    QSqlRecord recordData = modelShow->record(index.row());

    ui->ledFactorsID->setText(recordData.value(0).toString());
    ui->ledFactorsName->setText(recordData.value(1).toString());
    ui->ledFactorsGB->setText(recordData.value(2).toString());
    ui->ledFactorsHB->setText(recordData.value(3).toString());
    ui->ledFactorsREF->setText(recordData.value(4).toString());
}

void FactorsCode::on_btnReturn_clicked()
{
    this->close();
}

void FactorsCode::on_btnUpdataFactors_clicked()
{

    QString factorsID = ui->ledFactorsID->text();//用户ID
    QString factorsName = ui->ledFactorsName->text();//用户名称
    QString factorsGB = ui->ledFactorsGB->text();//国标代码
    QString factorsHB = ui->ledFactorsHB->text();//湖北代码
    QString factorsREF = ui->ledFactorsREF->text();//用户定义代码

    if(factorsName.isEmpty())
    {
        HelperClass::ShowMessageBoxInfo(tr("必须填入因子名称!"),this);
    }
    else
    {
        QSqlQuery query;

        QString sql ="UPDATE factorsinfo SET  factors_name='"+factorsName+"',factors_GBCODE='"+factorsGB+"', factors_HBCODE ='"+factorsHB+"', factors_REFCODE='"+factorsREF+"' where ID = "+factorsID+"";

        qDebug() <<sql;
        query.exec(sql);

        showFactorsInfo();
//        if(ok)
//        {
//            ui->showResult->setText(tr("用户注册成功"));
//            //HelperClass::MyLoginBlog("logblog","注册","新用户注册",customerName);
//            //HelperClass::ShowMessageBoxInfo(tr("注册成功!"));
//        }
//        else
//        {
//            ui->showResult->setText(tr("注册失败"));
//        }
    }

}


void FactorsCode::on_btnDeleteFactors_clicked()
{

}

void FactorsCode::on_btnAddProtocol_clicked()
{
    //QString ProtocolName = ui->ledAddProtocol->text();//用户ID
    QString ProtocolName="ALTER TABLE factorsinfo ADD "+ui->ledAddProtocol->text()+" text(20)";
    QSqlQuery query;
    query.exec(ProtocolName);
    showFactorsInfo();
}
