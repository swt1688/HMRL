#include "datainfo.h"
#include "ui_datainfo.h"

#include "initdatabase.h"
#include <QDateTime>





DataInfo::DataInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataInfo)
{
    ui->setupUi(this);

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

    modelShow = new QSqlQueryModel;

    QTimer *timer=new QTimer(this);
    QObject::connect(timer,&QTimer::timeout,this,&DataInfo::datapross);//作为客户端需处理的数据
    timer->start(60000);

    datapross();

}

DataInfo::~DataInfo()
{
    delete ui;
}



void DataInfo::datapross()
{
    QString strs;
    //QString startTime,endTime;
    //startTime=QDateTime::currentDateTime().date().toString("yyyy-MM-dd 00:00:00");
    //endTime=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->label->setText(tr("当前库存概况"));//（%1--%2）").arg(startTime).arg(endTime));
    //strs=tr("SELECT DataTime,ProductName,ProductSN,SpecificationLevel,Unit,Weight,MetagePerson,ColdStoreName,ColdStoreSN,UnitPrice,TotalAmount,StroageType,RecordPerson FROM `hmrl_erp`.storagerecordtable where DataTime between '%1' and '%2';")
            //.arg(startTime).arg(endTime);
    strs=tr("SELECT ProductName,SpecificationLevel,Unit,Weight,UnitPrice,TotalAmount FROM `hmrl_erp`.currentstoragetable;");

    //qDebug()<<"测试"<<strs;
    modelShow->setQuery(strs);
    if(modelShow->rowCount()>0)
    {
        int i=0;
        qDebug()<<tr("dataInfo->datapross当前读取到的数据条数%1").arg(modelShow->rowCount());
        modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品名称"));
        //modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品编号"));
        modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("规格等级"));
        modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单位"));
        modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("重量"));

        modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("当前价格"));
        modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("总价"));

        ui->tableView->setModel(modelShow);
    }
    else
    {
            //checkNetWorkOnline();
    }
}

void DataInfo::checkNetWorkOnline()
{
    QHostInfo::lookupHost("139.159.225.248",this,SLOT(onLookupHost(QHostInfo)));
}

void DataInfo::onLookupHost(QHostInfo host)
{
    if (host.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed:" << host.errorString();
        //网络未连接，发送信号通知
        //ui->labInfo->setText(tr("当前网络连接已断开"));
        emit sigLookUpHostResult(false);
    }
    else{
        OpenDatabase();
        //ui->labInfo->setText(tr("当前网络已连接,正在读取数据"));
        emit sigLookUpHostResult(true);
    }
}






