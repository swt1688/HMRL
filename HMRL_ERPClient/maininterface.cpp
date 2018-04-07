#include "maininterface.h"
#include "ui_maininterface.h"



#include "logoin.h"
#include "remindinterface.h"
#include "QFrame/picturemodify.h"


#include "aboutus.h"


#include "iconhelper.h"
#include <QtCore>

#include <time.h>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QTextEdit>
#include <QSize>
#include <QTabWidget>
#include <QLabel>


QHash<QString,int>mainInterface::UploadIP;
QStringList mainInterface::OnLineFlag;


mainInterface::mainInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainInterface)
{
    ui->setupUi(this);
    this->setAttribute (Qt::WA_DeleteOnClose);//当close时会析构该对象

    OnLineFlag.clear();
    OnLineFlag<<tr("离线")<<tr("在线");


    initStyle();


    statusBarInit();

    QTimer *timer=new QTimer(this);
    QObject::connect(timer,&QTimer::timeout,[=](){labTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss dddd"));});
    timer->start(1000);



}

void mainInterface::initStyle()
{



    ui->cboxUserLogo->setView(new QListView());//使下拉框的Item高度生效
    //ui->groupBox_2->setTitle(tr("当前企业：")+BaseInfo::CompanyName);

    //ui->labTitle->setText(tr("当前企业:%1(--联网状态:%2)").arg(BaseInfo::CompanyName).arg(OnLineFlag.at(BaseInfo::OnLineStatus)));

    //ui->widgetTitle->installEventFilter(this);
    ui->widgetTitle->setProperty("form", "title");
    ui->widgetTop->setProperty("nav", "top");
    ui->labICON->setText("智能综合管理系统");
    ui->labICON->setFont(QFont("Microsoft Yahei", 20));
    this->setWindowTitle(ui->labICON->text());

    QSize icoSize(45, 45);
    int icoWidth = 85;

    //设置顶部导航按钮
    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }

    dataInfo=new DataInfo(this);//
    dataAnalysis=new DataAnalysis(this);
    remoteControl=new RemoteControl(this);
    logInfo = new LoginInfo(this);
    systemMaintain = new SystemMaintain(this);



    quint8 i=0;
    ui->stackedWidget->insertWidget(i++,dataInfo);
    ui->stackedWidget->insertWidget(i++,dataAnalysis);
    ui->stackedWidget->insertWidget(i++,logInfo);
    ui->stackedWidget->insertWidget(i++,remoteControl);
    ui->stackedWidget->insertWidget(i++,systemMaintain);

    ui->stackedWidget->setCurrentIndex(0);

    ui->cboxUserLogo->addItem(QIcon(":/image/icon/userlogo.png"),tr("你好,")+Logoin::CurrentUserName);
    ui->cboxUserLogo->addItem(QIcon(":/image/icon/shuo.png"),tr("修改密码"));
    ui->cboxUserLogo->addItem(QIcon(":/image/icon/talk.png"),tr("提醒信息"));
    ui->cboxUserLogo->addItem(QIcon(":/image/icon/m_icon.png"),tr("修改图像"));
    ui->cboxUserLogo->addItem(QIcon(":/image/icon/switch_user.png"),tr("退出"));
    ui->cboxUserLogo->addItem(tr("关于..."));
}
void mainInterface::buttonClick()
{
    QToolButton *b = (QToolButton *)sender();
    QString name = b->text();

    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        if (btn == b) {
            btn->setChecked(true);
        } else {
            btn->setChecked(false);
        }
    }

    if (name == "产品概况") {
        ui->stackedWidget->setCurrentIndex(0);
        dataInfo->datapross();
    } else if (name == "商品入库") {
        ui->stackedWidget->setCurrentIndex(1);
        //dataAnalysis->InitInStroage();
    } else if (name == "商品出库") {
        ui->stackedWidget->setCurrentIndex(2);
        //logInfo->InitInStroage();
    } else if (name == "库存查询") {
        ui->stackedWidget->setCurrentIndex(3);
    } else if (name == "系统维护") {
        ui->stackedWidget->setCurrentIndex(4);
    }
}


void mainInterface::statusBarInit()
{
    QLabel *LabUserName = new QLabel;
    LabUserName->setText(QString(tr("登录用户:%1")).arg(Logoin::CurrentUserName));
    LabUserName->setAlignment(Qt::AlignHCenter);
    LabUserName->setMinimumSize(150,20); //设置标签最小尺寸
    LabUserName->setFrameShape(QFrame::StyledPanel); //设置标签形状
    LabUserName->setFrameShadow(QFrame::Raised); //设置标签阴影
    ui->statusbar->addWidget(LabUserName);

    QLabel *labLogTime = new QLabel;
    labLogTime->setText(QString(tr("本次登录时间:%1--上次关闭时间%2")).arg(Logoin::LoginTime).arg(Logoin::LogoutTime));
    labLogTime->setAlignment(Qt::AlignHCenter);
    labLogTime->setMinimumSize(450,20); //设置标签最小尺寸
    labLogTime->setFrameShape(QFrame::StyledPanel); //设置标签形状
    labLogTime->setFrameShadow(QFrame::Raised); //设置标签阴影
    ui->statusbar->addWidget(labLogTime);

    //QString Company_Name=BaseInfo::CompanyName;
    QLabel *labCompany = new QLabel;
    labCompany->setAlignment(Qt::AlignHCenter);
    labCompany->setMinimumSize(400,20); //设置标签最小尺寸
    labCompany->setFrameShape(QFrame::StyledPanel); //设置标签形状
    labCompany->setFrameShadow(QFrame::Raised); //设置标签阴影
    labCompany->setText(tr("%1(MN:%2)").arg("湖北省麻城市惠民肉联有限公司").arg("001"));
    ui->statusbar->addWidget(labCompany);

    labTime = new QLabel(this);
    labTime->setAlignment(Qt::AlignHCenter);
    labTime->setMinimumSize(200,20); //设置标签最小尺寸
    labTime->setFrameShape(QFrame::StyledPanel); //设置标签形状
    labTime->setFrameShadow(QFrame::Raised); //设置标签阴影
    ui->statusbar->addPermanentWidget(labTime);
}

mainInterface::~mainInterface()
{
    delete ui;
}



//关闭软件
void mainInterface::closeEvent(QCloseEvent *event)
{
    QString currentTime=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString fileName = Logoin::AppPath+"/localization/config.conf";
    QSettings *set = new QSettings(fileName,QSettings::IniFormat);
    set->beginGroup("logInfo");
    set->setValue("LogoutTime",currentTime);
    set->endGroup();

    qDebug()<<"文件写入完毕"<<endl;
    //正常退出写入日志数据库
    //修改最后一条记录  UPDATE userinfo set userid='55' WHERE 1 ORDER BY userid DESC LIMIT 1
    //QString sql="UPDATE userlogin SET LogoutTime ='"+currentTime+"' WHERE LoginTime='"+Logoin::LoginTime+"';";
    //QSqlQueryModel *modelShow = new QSqlQueryModel;
    //modelShow->setQuery(sql);//
    //测试多线程的结束

    this->close();
}

void mainInterface::on_cboxUserLogo_activated(const QString &arg1)
{
    ui->cboxUserLogo->setCurrentIndex(0);

    if(arg1==tr("提醒信息"))
    {
        RemindInterface *remindInterface = new RemindInterface(this);
        remindInterface->show();
    }
    if(arg1==tr("修改图像"))
    {
        PictureModify *pictureModify=new PictureModify(this);
        pictureModify->show();
    }
    if(arg1==tr("关于..."))
    {
        // QR二维码
        AboutUs *test_case_011=new AboutUs(this);
        test_case_011->show();
    }
}

