#ifndef LOGOIN_H
#define LOGOIN_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSettings>
#include <QDebug>


namespace Ui {
class Logoin;
}

class Logoin : public QDialog
{
    Q_OBJECT

public:
    explicit Logoin(QWidget *parent = 0);
    ~Logoin();

    static QString AppPath;                 //应用程序路径
    static QString CurrentImage;            //设置当前主界面对应图片

    static QString CurrentUserName;         //当前用户名
    static QString CurrentUserPwd;          //当前用户密码


    static QString LastLoginter;            //最后一次登录用户名，用来自动登录时加载的用户信息
    static QString LoginTime;              //登录时间
    static QString LogoutTime;             //退出登录时间

    static bool RemeberUser;         //当前用户类型(管理员、普通用户)
    static bool AutoLogo;           //自动登录标志


protected:
    bool eventFilter(QObject *obj, QEvent *event);//键盘事件的响应
private slots:

    void on_btnLogoin_clicked();
    void on_btnQuit_clicked();
    void LoginSystem(QString UserName,QString UserPwd);


private:
    Ui::Logoin *ui;

    int total;
    void InitLogin();

    void WriteConfig();          //写入配置文件,在更改配置文件
    void ReadConfig();           //读取配置文件,在main函数最开始加载程序载入


};


#endif // LOGOIN_H
