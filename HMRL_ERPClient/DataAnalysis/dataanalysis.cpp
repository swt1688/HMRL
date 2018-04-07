#include "dataanalysis.h"
#include "ui_dataanalysis.h"

#include <QDateTime>
#include <QDebug>

#include "logoin.h"

DataAnalysis::DataAnalysis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataAnalysis)
{
    ui->setupUi(this);

    modelShow = new QSqlQueryModel(this);
    connect(ui->cboxProductName,&QComboBox::currentTextChanged,[=](){
            QStringList ProductLevelList;
            QString str=tr("SELECT distinct Quality FROM `hmrl_erp`.porductpricetable where ProductName='%1';").arg(ui->cboxProductName->currentText());
            if(query.exec(str)){while(query.next()) {ProductLevelList<<query.value(0).toString();}}
             ui->cboxSpecificationLevel->clear(); ui->cboxSpecificationLevel->addItems(ProductLevelList);
             CalculatePrice(ui->txtPrice->text());
            });
    connect(ui->cboxSpecificationLevel,&QComboBox::currentTextChanged,this,&DataAnalysis::CalculatePrice);

    connect(ui->txtWeight,&QLineEdit::textChanged,this,&DataAnalysis::CalculatePrice);
    connect(ui->txtPrice,&QLineEdit::textChanged,this,&DataAnalysis::CalculatePrice);
    connect(ui->cboxColdStoreName,&QComboBox::currentTextChanged,[=](){
        QStringList ColdSNList;
        QString str=tr("SELECT  ColdStoreSN FROM `hmrl_erp`.coldstoretable where ColdStoreName='%1';").arg(ui->cboxColdStoreName->currentText());
        if(query.exec(str)){while(query.next()){ColdSNList<<query.value(0).toString();}}
        ui->cboxColdStoreSN->clear();ui->cboxColdStoreSN->addItems(ColdSNList);
    });

    ui->tableView->setAlternatingRowColors(true);//隔行变色,以下是视觉方面的效果，不加也没影响,把表格的背景调成黄蓝相间
    /********setSectionResizeMode(ResizeMode mode)函数，ResizeMode接口枚举参数*******************
     * 1、QHeaderView::Interactive 用户可以重新调整表头的大小，也可以使用resizeSection()重新调整表头的大小。
     * 2、QHeaderView::Fixed 用户不可以重新调整表头的大小，只可以使用resizeSection()重新调整表头的大小。
     * 3、QHeaderView::Stretch 表头将会调整单元格到可得的空间。用户或者程序员通过代码都不能改变它的大小。
     * 4、QHeaderView::ResizeToContents 表头将自动根据整个行或者列的内容去调整表头单元格到最佳的大小。用户或者程序员通过代码都不能改变它的大小。
     * ******************************************************************************/
    /*****************设置第一列为固定60宽度，其他自适应 *************************/

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(100);
    //tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setObjectName("hHeader");

    InitInStroage();



}

DataAnalysis::~DataAnalysis()
{
    delete ui;
}
void DataAnalysis::CalculatePrice(const QString &)
{
    QString str=tr("SELECT UnitPrice FROM `hmrl_erp`.PorductPriceTable where ProductName='%1' and Quality='%2' order by UpdataTime desc limit 1;").arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText());
    if(query.exec(str))
    {
        while (query.next())
        {
            ui->txtPrice->setText(query.value(0).toString());
        }
    }
    totalAmount=ui->txtWeight->text().toFloat()*ui->txtPrice->text().toFloat();
    ui->labTotalAmount->setText(tr("总价:%1元").arg(totalAmount, 14, 'f', 2));
    str=tr("SELECT Weight FROM `hmrl_erp`.currentstoragetable where ProductName='%1' and SpecificationLevel='%2' and Unit = '%3';").arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText()).arg(ui->cboxUnit->currentText());
    CurrentCount=0;
    CurrentSum=0;
    if(query.exec(str))
    {
        CurrentCount=query.numRowsAffected();
        qDebug()<<"当数据条数"<<CurrentCount;
        while (query.next())
        {
            //CurrentCount=1;
            CurrentSum=query.value(0).toFloat();
        }
    }


}
void DataAnalysis::InitInStroage()
{
    QStringList ProductList;
    QString str;
    str="SELECT distinct ProductName FROM `hmrl_erp`.porductpricetable;";
    if(query.exec(str))
    {
        while(query.next())
        {
            ProductList<<query.value(0).toString();
        }
    }
    qDebug()<<"产品列表"<<ProductList<<str;
    ui->cboxProductName->clear();
    ui->cboxProductName->addItems(ProductList);
    QStringList ColdStorageList;
    str="SELECT distinct ColdStoreName FROM `hmrl_erp`.coldstoretable;";
    if(query.exec(str))
    {
        while(query.next())
        {
            ColdStorageList<<query.value(0).toString();
        }
    }
    ui->cboxColdStoreName->clear();
    ui->cboxColdStoreName->addItems(ColdStorageList);
    QStringList MetagePersonList;
    str="SELECT UserName FROM `hmrl_erp`.userinfotable;";
    if(query.exec(str))
    {
        while(query.next())
        {
            MetagePersonList<<query.value(0).toString();
        }
    }
    ui->cboxMetagePerson->clear();
    ui->cboxMetagePerson->addItems(MetagePersonList);
    LoadData();

}
void DataAnalysis::LoadData()
{
    QString startTime,endTime;
    startTime=QDateTime::currentDateTime().date().toString("yyyy-MM-dd 00:00:00");
    endTime=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->label_12->setText(tr("今日入库情况:(%1--%2)").arg(startTime).arg(endTime));
    QString str;
    str=tr("SELECT DataTime,ProductName,ProductSN,SpecificationLevel,Unit,Weight,MetagePerson,ColdStoreName,ColdStoreSN,UnitPrice,TotalAmount,RecordPerson FROM `hmrl_erp`.storagerecordtable where StroageType='入库' and DataTime between '%1' and '%2';")
            .arg(startTime).arg(endTime);
    modelShow->setQuery(str);
    int i=0;
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("入库时间"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品编号"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("规格等级"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单位"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("重量"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("称重人员"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("冷库名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("冷库编号"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单价"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("总价"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("入库人员"));

    //隔行变色,以下是视觉方面的效果，不加也没影响
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents ();//根据内容自动调整所有列的列宽  ui->tabUserInfo->resizeColumnToContents (0);
    //ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableView->setModel(modelShow);

}
void DataAnalysis::on_pushButton_clicked()
{
    QString str=tr("INSERT INTO `hmrl_erp`.storagerecordtable ( `DataTime`, `ProductName`, `ProductSN`,`SpecificationLevel`,`Unit`,`Weight`,`MetagePerson`,`ColdStoreName`,`ColdStoreSN`,`UnitPrice`,`TotalAmount`,`StroageType`,`RecordPerson`) "
                   "VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10','%11','%12','%13');")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->cboxProductName->currentText())
                    .arg(ui->txtProductSN->text()).arg(ui->cboxSpecificationLevel->currentText()).arg(ui->cboxUnit->currentText())
                    .arg(ui->txtWeight->text()).arg(ui->cboxMetagePerson->currentText()).arg(ui->cboxColdStoreName->currentText()).arg(ui->cboxColdStoreSN->currentText())
            .arg(ui->txtPrice->text()).arg(totalAmount).arg(tr("入库")).arg(Logoin::CurrentUserName);
    query.exec(str);

    float sum = CurrentSum+ui->txtWeight->text().toFloat();
    double sumAmount=sum*ui->txtPrice->text().toFloat();
    if(CurrentCount == 0)
    {
        str = tr("INSERT INTO `hmrl_erp`.currentstoragetable(`ProductName`,`SpecificationLevel`,`Unit`,`Weight`,`UnitPrice`,`TotalAmount`) "
                 "VALUES ('%1','%2','%3','%4','%5','%6');")
                .arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText())
                .arg(ui->cboxUnit->currentText()).arg(sum).trimmed().arg(ui->txtPrice->text()).arg(sumAmount);

    }
    else
    {
        str=tr("update `hmrl_erp`.currentstoragetable set ProductName='%1',SpecificationLevel='%2',Unit='%3',Weight='%4',UnitPrice='%5',TotalAmount='%6' where ProductName = '%7' and SpecificationLevel = '%8' and Unit = '%9';")
                .arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText())
                .arg(ui->cboxUnit->currentText()).arg(sum).arg(ui->txtPrice->text()).arg(sumAmount)
                .arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText()).arg(ui->cboxUnit->currentText());

    }
   query.exec(str);
    qDebug()<<str;

    LoadData();


}

void DataAnalysis::on_btnUpdate_clicked()
{
    InitInStroage();
}
