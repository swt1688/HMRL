#include "remotecontrol.h"
#include "ui_remotecontrol.h"

#include <QDebug>

RemoteControl::RemoteControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteControl)
{
    ui->setupUi(this);

    Init();

}

RemoteControl::~RemoteControl()
{
    delete ui;
}
void RemoteControl::TitleInit()
{
    //widget= new QWidget(this);
    //widget->setProperty("form", "title2");


}
void RemoteControl::Init()
{
    QFont btnFont;
    btnFont.setFamily("楷体");
    btnFont.setPixelSize(22);
    //TitleInit();
    stackedWidget = new QStackedWidget(this);
    //stackedWidget->setMaximumHeight(45);
    QWidget *Page=new QWidget();
    Page->setObjectName(QStringLiteral("page"));

    QHBoxLayout *tHBoxLayout =new QHBoxLayout(Page);

    labSearchType = new QLabel(tr("查询类型"),Page);
    labStartTime = new QLabel(tr("开始时间"),Page);
    labEndTime = new QLabel(tr("结束时间"),Page);

    m_defStartDateTime = new DefDateTimeEdit(Page);
    m_defStartDateTime->setDate(QDate::currentDate().addDays(-1));//获取前一天时间
    m_defStartDateTime->setFont(btnFont);
    m_defStartDateTime->setMinimumHeight(42);

    m_defEndDateTime = new DefDateTimeEdit(Page);
    m_defEndDateTime->setDateTime(QDateTime::currentDateTime());//获取当前时间
    m_defEndDateTime->setFont(btnFont);
    m_defEndDateTime->setMinimumHeight(42);

    cboxSearchType = new QComboBox(Page);
    cboxSearchType->setMinimumHeight(42);
    cboxSearchType->setMinimumWidth(150);
    cboxSearchType->setFont(btnFont);

    tableView = new QTableView;

    btnSearch = new QPushButton (tr("查询"),Page);
    btnSearch->setMinimumHeight(45);
    btnSearch->setMinimumWidth(140);
    btnSearch->setFont(btnFont);

    searchTypeList<<tr("入库查询")<<tr("出库查询");//<<tr("入库查询");
    cboxSearchType->addItems(searchTypeList);

    tableView->setAlternatingRowColors(true);//隔行变色,以下是视觉方面的效果，不加也没影响,把表格的背景调成黄蓝相间
    //tableView->resizeColumnsToContents();
    /********setSectionResizeMode(ResizeMode mode)函数，ResizeMode接口枚举参数*******************
     * 1、QHeaderView::Interactive 用户可以重新调整表头的大小，也可以使用resizeSection()重新调整表头的大小。
     * 2、QHeaderView::Fixed 用户不可以重新调整表头的大小，只可以使用resizeSection()重新调整表头的大小。
     * 3、QHeaderView::Stretch 表头将会调整单元格到可得的空间。用户或者程序员通过代码都不能改变它的大小。
     * 4、QHeaderView::ResizeToContents 表头将自动根据整个行或者列的内容去调整表头单元格到最佳的大小。用户或者程序员通过代码都不能改变它的大小。
     * ******************************************************************************/
    /*****************设置第一列为固定60宽度，其他自适应 *********************************/
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setMinimumSectionSize(100);
    //tableView->horizontalHeader()->setSectionsMovable(true);
    tableView->horizontalHeader()->setObjectName("hHeader");

    int i=0;
    tHBoxLayout->insertWidget(i++,labSearchType);
    tHBoxLayout->insertWidget(i++,cboxSearchType);
    tHBoxLayout->insertWidget(i++,labStartTime);
    tHBoxLayout->insertWidget(i++,m_defStartDateTime);
    tHBoxLayout->insertWidget(i++,labEndTime);
    tHBoxLayout->insertWidget(i++,m_defEndDateTime);
    tHBoxLayout->insertWidget(i++,btnSearch);



    QSpacerItem *horizontalSpacer_1;
    horizontalSpacer_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    tHBoxLayout->insertSpacerItem(i++,horizontalSpacer_1);
    tHBoxLayout->setSpacing(15);
    stackedWidget->addWidget(Page);

    //Page2页面内容
    QWidget *page2 = new QWidget();
    page2->setObjectName(QStringLiteral("page"));

    labSearchReport = new QLabel(tr("查询类型"),page2);
    labStartTimeReport = new QLabel(tr("报表时段"),page2);
    cboxSearchReport = new QComboBox(page2);
    cboxSearchReport->setMinimumHeight(42);
    cboxSearchReport->setMinimumWidth(150);
    cboxSearchReport->setFont(btnFont);
    cboxSearchReport->clear();
    cboxSearchReport->addItems(searchTypeList);

    btnSearchReport = new QPushButton(tr("生成报表"),page2);
    btnSearchReport->setMinimumHeight(45);
    btnSearchReport->setMinimumWidth(140);
    btnSearchReport->setFont(btnFont);
    connect(btnSearchReport,&QPushButton::clicked,this,&RemoteControl::CreateReport);

    dayEdit = new QDateEdit(page2);//日数据
    dayEdit->setMinimumHeight(42);
    dayEdit->setMinimumWidth(140);
    dayEdit->setDisplayFormat("yyyy-MM-dd");
    dayEdit->setCalendarPopup(true);
    dayEdit->setFont(btnFont);
    dayEdit->setDate(QDate::currentDate().addDays(-1));

    QHBoxLayout *tHBoxLayout2 =new QHBoxLayout(page2);
    i=0;
    tHBoxLayout2->insertWidget(i++,labSearchReport);
    tHBoxLayout2->insertWidget(i++,cboxSearchReport);
    tHBoxLayout2->insertWidget(i++,labStartTimeReport);
    tHBoxLayout2->insertWidget(i++,dayEdit);
    tHBoxLayout2->insertWidget(i++,btnSearchReport);
    tHBoxLayout2->insertSpacerItem(i++,horizontalSpacer_1);
    tHBoxLayout2->setSpacing(15);
    stackedWidget->addWidget(page2);



    QVBoxLayout *tVBoxLayout = new QVBoxLayout();
    tVBoxLayout->insertWidget(0,stackedWidget);
    tVBoxLayout->insertWidget(1,tableView);
    tVBoxLayout->setContentsMargins(2,2,2,2);
    tVBoxLayout->setStretch(1, 9);

    //定义ListView
    listView1 = new NavListView (this);
    listView1->setIcoColorBg(false);
    listView1->setColorLine(QColor(32, 53, 74));
    listView1->setColorBg(QColor(86, 91, 222), QColor(24, 189, 155), QColor(24, 189, 155, 150));
    listView1->setColorText(QColor(254, 255, 255), QColor(252, 252, 252), QColor(0, 0, 0));

    listView1->setMaximumWidth(180);
    listView1->setInfoVisible(false);
    listView1->setIcoStyle(NavListView::IcoStyle_Triangle);
    listView1->readData(":/QSS/config.xml");
    connect(listView1,&NavListView::pressed,this,&RemoteControl::listView_pressed);

    QHBoxLayout *tHBoxLayout3 =new QHBoxLayout();
    tHBoxLayout3->insertWidget(0,listView1);
    tHBoxLayout3->insertLayout(1,tVBoxLayout);

    modelShow = new QSqlQueryModel;

    this->setLayout(tHBoxLayout3);
    connect(btnSearch,&QPushButton::clicked,this,&RemoteControl::searchRecord);

    CurrentListSelect == "出入库";

}

void RemoteControl::listView_pressed()
{
    QModelIndex index = listView1->currentIndex();
    CurrentListSelect = index.data().toString();

    if (CurrentListSelect == "出入库") {
        searchTypeList.clear();
        searchTypeList<<tr("入库查询")<<tr("出库查询");//<<tr("入库查询");
        cboxSearchType->clear();
        cboxSearchType->addItems(searchTypeList);
        stackedWidget->setCurrentIndex(0);

        //ui->stackedWidget->setCurrentIndex(0);
    } else if (CurrentListSelect == "价格波动") {
        searchTypeList.clear();
        searchTypeList<<tr("全部");
        QString str;
        str=tr("SELECT distinct ProductName FROM `hmrl_erp`.PorductPriceTable;");
        if(query.exec(str))
        {
            while(query.next())
            {
                searchTypeList<<query.value(0).toString();
            }
        }
        cboxSearchType->clear();
        cboxSearchType->addItems(searchTypeList);
        stackedWidget->setCurrentIndex(0);

    } else if(CurrentListSelect == "日报表")
    {
        dayEdit->setDisplayFormat("yyyy-MM-dd");
        dayEdit->setDate(QDate::currentDate().addDays(-1));
        stackedWidget->setCurrentIndex(1);
    }else if(CurrentListSelect == "月报表")
    {
        dayEdit->setDisplayFormat("yyyy-MM");
        dayEdit->setDate(QDate::currentDate());
        stackedWidget->setCurrentIndex(1);
    }else if(CurrentListSelect == "年报表")
    {
        dayEdit->setDisplayFormat("yyyy");
        dayEdit->setDate(QDate::currentDate());
        stackedWidget->setCurrentIndex(1);
    }

}


void RemoteControl::searchRecord()
{
    if (CurrentListSelect == "出入库") {
        searchStorageInfo();
    } else if (CurrentListSelect == "价格波动") {
        searchPriceInfo();
    }



}
void RemoteControl::searchStorageInfo()
{
    QString strs;
    QStringList searchTypelist;
    QStringList timeType;
    searchTypelist<<tr("入库")<<tr("出库");
    timeType<<tr("入库时间")<<tr("出库时间");
    strs=tr("SELECT DataTime,ProductName,ProductSN,SpecificationLevel,Unit,Weight,MetagePerson,ColdStoreName,ColdStoreSN,UnitPrice,TotalAmount,StroageType,RecordPerson FROM `hmrl_erp`.storagerecordtable where StroageType = '%1' and DataTime between '%2' and '%3';")
            .arg(searchTypelist.at(cboxSearchType->currentIndex()))
            .arg(m_defStartDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(m_defEndDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss"));

    qDebug()<<"出入库记录"<<strs;
    modelShow->setQuery(strs);
    int i=0;
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("%1").arg(timeType.at(cboxSearchType->currentIndex())));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品编号"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("规格等级"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单位"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("重量"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("录入人员"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("冷库名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("冷库编号"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单价"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("总价"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("类型"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("录入人员"));
    tableView->setModel(modelShow);
}
void RemoteControl::searchPriceInfo()
{
    QString strs;
    if(cboxSearchType->currentText()=="全部")
    {
        strs=tr("SELECT ProductName,Quality,UnitPrice,UpdataPerson,UpdataTime FROM `hmrl_erp`.porductpricetable where UpdataTime between '%1' and '%2';")
                .arg(m_defStartDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(m_defEndDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
    }
    else
    {
        strs=tr("SELECT ProductName,Quality,UnitPrice,UpdataPerson,UpdataTime FROM `hmrl_erp`.porductpricetable where ProductName = '%1' and UpdataTime between '%2' and '%3';")
                .arg(cboxSearchType->currentText())
                .arg(m_defStartDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(m_defEndDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
    }
    qDebug()<<"查询价格"<<strs;

    modelShow->setQuery(strs);

    int i=0;

    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("规格等级"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单价"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("价格录入人员"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("日期"));

    tableView->setModel(modelShow);
}
void RemoteControl::CreateReport()
{

    QString strs;
    QStringList searchTypelist;
    QStringList timeType;
    searchTypelist<<tr("入库")<<tr("出库");
    timeType<<tr("入库时间")<<tr("出库时间");

    if(CurrentListSelect == "日报表")
    {
        strs=tr("SELECT date_format(DataTime, '%Y-%m-%d'),ProductName,SpecificationLevel,Unit,SUM(Weight) AS Weight,sum(TotalAmount) AS TotalAmount FROM `hmrl_erp`.storagerecordtable where DataTime between date_format('%1', '%Y-%m-%d') and date_format('%2', '%Y-%m-%d') and StroageType = '%3' GROUP BY ProductName,SpecificationLevel "
                "union "
                "SELECT date_format(DataTime, '%Y-%m-%d'),'合计','全部',Unit,SUM(Weight) AS Weight,sum(TotalAmount) AS TotalAmount FROM `hmrl_erp`.storagerecordtable where DataTime between date_format('%1', '%Y-%m-%d') and date_format('%2', '%Y-%m-%d') and StroageType = '%3';")
                .arg(dayEdit->dateTime().toString("yyyy-MM-dd")).arg(dayEdit->dateTime().addDays(+1).toString("yyyy-MM-dd"))
                .arg(searchTypelist.at(cboxSearchReport->currentIndex()));
    }else if(CurrentListSelect == "月报表")
    {
        strs=tr("SELECT date_format(DataTime, '%Y-%m'),ProductName,SpecificationLevel,Unit,SUM(Weight) AS Weight,sum(TotalAmount) AS TotalAmount FROM `hmrl_erp`.storagerecordtable where  DataTime between date_format('%1', '%Y-%m-%d') and date_format('%2', '%Y-%m-%d') and StroageType = '%3' GROUP BY ProductName,SpecificationLevel "
                "union "
                "SELECT date_format(DataTime, '%Y-%m'),'合计','全部',Unit,SUM(Weight) AS Weight,sum(TotalAmount) AS TotalAmount FROM `hmrl_erp`.storagerecordtable where  DataTime between date_format('%1', '%Y-%m-%d') and date_format('%2', '%Y-%m-%d') and StroageType = '%3';")
                .arg(dayEdit->dateTime().toString("yyyy-MM-00")).arg(dayEdit->dateTime().addMonths(+1).toString("yyyy-MM-00"))
                .arg(searchTypelist.at(cboxSearchReport->currentIndex()));
    }else if(CurrentListSelect == "年报表")
    {
        strs=tr("SELECT date_format(DataTime, '%Y'),ProductName,SpecificationLevel,Unit,SUM(Weight) AS Weight,sum(TotalAmount) AS TotalAmount FROM `hmrl_erp`.storagerecordtable where DataTime between date_format('%1', '%Y-%m-%d') and date_format('%2', '%Y-%m-%d') and StroageType = '%3' GROUP BY ProductName,SpecificationLevel "
                "union "
                "SELECT date_format(DataTime, '%Y'),'合计','全部',Unit,SUM(Weight) AS Weight,sum(TotalAmount) AS TotalAmount FROM `hmrl_erp`.storagerecordtable where DataTime between date_format('%1', '%Y-%m-%d') and date_format('%2', '%Y-%m-%d') and StroageType = '%3' ;")
                .arg(dayEdit->dateTime().toString("yyyy-00-00")).arg(dayEdit->dateTime().addYears(+1).toString("yyyy-00-00"))
                .arg(searchTypelist.at(cboxSearchReport->currentIndex()));
    }
    qDebug()<<"出入库记录"<<strs;
    modelShow->setQuery(strs);
    int i=0;
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("%1").arg(timeType.at(cboxSearchReport->currentIndex())));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("规格等级"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单位"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("重量"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("总价"));
    tableView->setModel(modelShow);
}
