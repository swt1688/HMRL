#include "logoin.h"
#include "ui_logoin.h"

#include "maininterface.h"
#include "helperclass.h"


//存储路径
QString Logoin::AppPath = "";

//存储当前登录用户信息,方便用户密码修改和用户退出操作
QString Logoin::CurrentUserName = "";
QString Logoin::CurrentUserPwd = "";
bool Logoin::RemeberUser;         //当前用户类型(管理员、普通用户)
bool Logoin::AutoLogo;           //自动登录标志
QString Logoin::LoginTime="";          //登录时间
QString Logoin::LogoutTime="";         //退出时间
QString Logoin::LastLoginter = "";

Logoin::Logoin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Logoin)
{
    ui->setupUi(this);
    this->setAttribute (Qt::WA_DeleteOnClose);
    this->setWindowTitle(tr("惠民肉联综合管理系统"));
    installEventFilter(this);//安装监听器监听密码框回车键//ui->txtPassword->

    ui->labelLoginDesText->setOpenExternalLinks(true);//label超级链接
    ui->labelLoginDesText->setText(
                QString("<style>a{text-decoration:none;color:#616161}</style><a href=\"http://www.hbmchmrl.com/\">更多信息请访问惠民肉联官方网站 www.hbmchmrl.com")
                );

    this->InitLogin();
}

Logoin::~Logoin()
{
    delete ui;
}
void Logoin::InitLogin()
{
    total = 0;

    ReadConfig();

    ui->chkRemeber->setChecked(RemeberUser);
    ui->chkAutoLogin->setChecked(AutoLogo);
    if(Logoin::RemeberUser==true)
    {

        ui->txtUserName->setText(Logoin::CurrentUserName);
        ui->txtPassword->setText(Logoin::CurrentUserPwd);
    }
    else
    {
        ui->txtUserName->setText(Logoin::CurrentUserName);
        ui->txtPassword->clear();
        CurrentUserPwd ="";
        ui->txtPassword->setFocus();
    }
    if(Logoin::AutoLogo==true)
    {

        LoginSystem(Logoin::CurrentUserName,Logoin::CurrentUserPwd);
    }
}

void Logoin::on_btnLogoin_clicked()
{
    QString UserName = ui->txtUserName->text().trimmed();
    QString UserPwd = ui->txtPassword->text().trimmed().toUtf8().toBase64();

    ui->txtPassword->setFocus();
    //管理员登录
    if(UserPwd.isEmpty()||UserName.isEmpty())
    {
        HelperClass::ShowMessageBoxInfo(tr("user name or password is not empty,please input"),this);
    }
    else
    {
        LoginSystem(UserName,UserPwd);
    }
}

void Logoin::LoginSystem(QString UserName,QString UserPwd)
{
    QSqlTableModel model;
    model.setTable("userinfotable");//查询数据表
    model.setFilter(QObject::tr("UserName = '%1' and Password ='%2'").arg(UserName).arg(UserPwd));
    model.select();
    qDebug()<<tr("UserName = '%1' and Password ='%2'").arg(UserName).arg(UserPwd)<<model.rowCount();

    if(model.rowCount() == 1)
    {
        Logoin::LastLoginter = ui->txtUserName->text();
        Logoin::CurrentUserName = Logoin::LastLoginter;
        Logoin::CurrentUserPwd = ui->txtPassword->text();
        //logo::CurrentUserType = tr("管理员");

        Logoin::RemeberUser = ui->chkRemeber->isChecked();
        Logoin::AutoLogo = ui->chkAutoLogin->isChecked();
        Logoin::LoginTime=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        Logoin::WriteConfig();           //写进配置文件

        qDebug() <<UserName<<""<<UserPwd;
        //accept();
        this->close();
        mainInterface *main_interface=new mainInterface();

        main_interface->show();



    }
    else
    {
        total++;
        HelperClass::ShowMessageBoxError(tr("password error，please input"));
        ui->txtPassword->clear();
        ui->txtPassword->setFocus();
        if(total == 3)
        {
            HelperClass::ShowMessageBoxError(tr("您已连续三次输错密码，若忘记密码请联系管理员！"));
            qApp->quit();
        }
    }
}

/*
 *函数功能：读取配置文件内容
 *输入参数：无
 *输出参数：无
 *返回值:  无
*/
void Logoin::ReadConfig()
{
    QString fileName = Logoin::AppPath+"/localization/config.conf";
    //如果配置文件不存在,则以初始值继续运行
    //没有这个判断的话,配置赋值都为空
    if(!HelperClass::FileIsExist(fileName))
    {
        Logoin::CurrentUserName = Logoin::CurrentUserName.toLatin1();
        return;
    }
    QSettings *set = new QSettings(fileName,QSettings::IniFormat);
    set->beginGroup("logInfo");
    Logoin::LogoutTime=set->value("LogoutTime").toString();
    Logoin::CurrentUserName=set->value("LastLoginer").toString();

    //set->beginGroup("AppConfig");
    Logoin::CurrentUserPwd=set->value("CurrentUserPwd").toString();
    Logoin::RemeberUser =set->value("RemeberFlag").toBool();
    Logoin::AutoLogo=set->value("AutoLogoFlag").toBool();

    set->endGroup();

}
/*
 *函数功能：写配置文件内容
 *输入参数：无
 *输出参数：无
 *返回值:  无
*/
void Logoin::WriteConfig()
{
    QString fileName = Logoin::AppPath+"/localization/config.conf";
    QSettings *set = new QSettings(fileName,QSettings::IniFormat);

    set->beginGroup("logInfo");
    //set->beginGroup("AppConfig");
    set->setValue("LoginTime",Logoin::LoginTime);
    set->setValue("LastLoginer",Logoin::CurrentUserName);
    set->setValue("CurrentUserPwd",Logoin::CurrentUserPwd);
    set->setValue("RemeberFlag",RemeberUser);
    set->setValue("AutoLogoFlag",AutoLogo);

    set->endGroup();
}

bool Logoin::eventFilter(QObject *obj, QEvent *event)
{
    //用户按下回车键,触发登录信号.
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key()==Qt::Key_Enter)
        {
            this->on_btnLogoin_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj,event);
}


void Logoin::on_btnQuit_clicked()
{
    //qApp->quit();
    //qApp->exit();
    this->close();
}
