#include "systemmaintain.h"
#include "ui_systemmaintain.h"

#include "logoin.h"
#include <QDateTime>
#include <QDebug>

SystemMaintain::SystemMaintain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemMaintain)
{
    ui->setupUi(this);




    modelShow = new QSqlQueryModel(this);
    LoadUserData();

    QSize icoSize(45, 45);
    int icoWidth = 85;

    //设置顶部导航按钮
    ui->widget->setProperty("form", "title2");
    QList<QToolButton *> tbtns = ui->widget->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }

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

}

SystemMaintain::~SystemMaintain()
{
    delete ui;
}

void SystemMaintain::LoadUserData()
{
    modelShow->setQuery("SELECT UserSN as '用户编号',UserName as '用户名称',userLevel as '职位',sex as '性别',MobilePhone as '联系电话' FROM `hmrl_erp`.userinfotable;");
    ui->tableView->setModel(modelShow);
}

void SystemMaintain::LoadProductData()
{
    modelShow->setQuery("SELECT ProductName as '产品名称',DataTime as '创建时间' FROM `hmrl_erp`.porductinfotable;");
    ui->tableView->setModel(modelShow);
}
void SystemMaintain::LoadColdData()
{
    modelShow->setQuery("SELECT ColdStoreName as '冷库名称',ColdStoreSN as '冷库编号',DataTime as '创建时间' FROM `hmrl_erp`.coldstoretable;");
    ui->tableView->setModel(modelShow);
}
void SystemMaintain::buttonClick()
{
    QToolButton *b = (QToolButton *)sender();
    QString name = b->text();

    QList<QToolButton *> tbtns = ui->widget->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        if (btn == b) {
            btn->setChecked(true);
        } else {
            btn->setChecked(false);
        }
    }

    if (name == "用户设置") {
        ui->stackedWidget->setCurrentIndex(0);
        LoadUserData();
    } else if (name == "冷库设置") {
        ui->stackedWidget->setCurrentIndex(1);
        LoadColdData();
    } else if (name == "产品设置") {
        ui->stackedWidget->setCurrentIndex(2);
        LoadProductData();
    } else if (name == "今日价格") {
        ui->stackedWidget->setCurrentIndex(3);
        LoadProductPriceData();
    } else if (name == "客户信息") {
        ui->stackedWidget->setCurrentIndex(4);
        LoadCustomerData();
    }else if (name == "库存预警") {
        ui->stackedWidget->setCurrentIndex(5);
    }
}


void SystemMaintain::on_btnAdd_clicked()
{
    if((ui->txtUserSN->text().isEmpty())||(ui->txtUserName->text().isEmpty())||(ui->txtPassword->text().isEmpty()))
    {
        return ;
    }
    QString pw=ui->txtPassword->text().toUtf8().toBase64();

    QString str=tr("INSERT INTO `hmrl_erp`.userinfotable ( `UserSN`, `UserName`, `Password`,`MobilePhone`,`userLevel`,`sex`,`CreateDate`) "
                   "VALUES ('%1', '%2', '%3', '%4', '%5','%6','%7');")
                    .arg(ui->txtUserSN->text()).arg(ui->txtUserName->text()).arg(pw)
                    .arg(ui->txtMobilePhone->text()).arg(ui->cboxAccePermi->currentText()).arg(ui->cboxSex->currentText())
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.exec(str);

    qDebug()<<str;

    LoadUserData();
}

void SystemMaintain::on_btnRefresh_clicked()
{
    LoadUserData();
}

void SystemMaintain::on_btnColdAdd_clicked()
{
    if((ui->txtColdStoreName->text().isEmpty())||(ui->txtColdStoreSN->text().isEmpty()))
    {
        return ;
    }

    QString str=tr("INSERT INTO `hmrl_erp`.coldstoretable ( `ColdStoreName`, `ColdStoreSN`,`DataTime`) "
                   "VALUES ('%1', '%2', '%3');")
                    .arg(ui->txtColdStoreName->text()).arg(ui->txtColdStoreSN->text())
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.exec(str);
    LoadColdData();
    qDebug()<<str;
}


void SystemMaintain::on_btnColdRefresh_clicked()
{
    LoadColdData();
}

void SystemMaintain::on_btnAddProduct_clicked()
{
    if(ui->txtProductName->text().isEmpty())
    {
        return ;
    }

    QString str=tr("INSERT INTO `hmrl_erp`.porductinfotable ( `ProductName`, `DataTime`) "
                   "VALUES ('%1', '%2');")
                    .arg(ui->txtProductName->text())
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.exec(str);
    LoadProductData();
    qDebug()<<str;
}


void SystemMaintain::on_btnUpdataPrice_clicked()
{
    if(ui->txtUnitPrice->text().isEmpty())
    {
        return ;
    }

    QString str=tr("INSERT INTO `hmrl_erp`.porductpricetable ( `ProductName`, `Quality`,`UnitPrice`, `UpdataPerson`,`UpdataTime`) "
                   "VALUES ('%1', '%2','%3', '%4', '%5');")
            .arg(ui->cboxProductName->currentText()).arg(ui->cboxQuality->currentText())
            .arg(ui->txtUnitPrice->text()).arg(Logoin::CurrentUserName)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.exec(str);
    LoadProductPriceData();
    qDebug()<<str;
}
void SystemMaintain::LoadProductPriceData()
{
    QString str="SELECT ProductName FROM `hmrl_erp`.porductinfotable;";
    QStringList strlist;
    query.exec(str);
    while(query.next())
    {
        strlist<<query.value(0).toString();
    }
    ui->cboxProductName->addItems(strlist);
    modelShow->setQuery("SELECT ProductName as '产品名称' ,Quality as '规格等级',UnitPrice as '单价',UpdataPerson as '更新人员',UpdataTime as '更新时间' FROM `hmrl_erp`.porductpricetable;");
    ui->tableView->setModel(modelShow);
}

void SystemMaintain::on_btnAddCustomer_clicked()
{
    if(ui->txtCustomerName->text().isEmpty())
    {
        return ;
    }
    QString str=tr("INSERT INTO `hmrl_erp`.customertable ( `CustomerName`, `CreditCode`, `CustomerAddress`,`BankName`,`BankAccount`,`ContactName`,`ContactPhone`,`UpdataPerson`,`UpdataTime`) "
                   "VALUES ('%1', '%2', '%3', '%4', '%5','%6','%7','%8','%9');")
                    .arg(ui->txtCustomerName->text()).arg(ui->txtCreditCode->text()).arg(ui->txtCustomerAddress->text())
                    .arg(ui->txtBankName->text()).arg(ui->txtBankAccount->text()).arg(ui->txtContactName->text())
                    .arg(ui->txtContactPhone->text()).arg(Logoin::CurrentUserName)
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.exec(str);

    qDebug()<<str;

    LoadCustomerData();
}
void SystemMaintain::LoadCustomerData()
{
    modelShow->setQuery("SELECT CustomerName as 客户名称,CreditCode as 信用代码,CustomerAddress as 客户地址,BankName as 开户银行,BankAccount as 银行账号,ContactName as 联系人,ContactPhone as 联系电话,UpdataPerson as 记录人员,UpdataTime as 记录时间 FROM `hmrl_erp`.customertable;");
    ui->tableView->setModel(modelShow);
}
