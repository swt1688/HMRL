#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QCloseEvent>


#include "qabstractitemmodel.h"



#include "DataInfo/datainfo.h"
#include "DataAnalysis/dataanalysis.h"
#include "RemoteControl/remotecontrol.h"
#include "SystemMaintain/systemmaintain.h"
#include "loginfo/logininfo.h"



namespace Ui {
class mainInterface;
}

class mainInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainInterface(QWidget *parent = 0);
    ~mainInterface();

    QLabel *labTime;
    static QHash<QString,int>UploadIP;
    static QStringList OnLineFlag;



protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void buttonClick();
    /***************************TCP Server 服务端槽函数********************************/


    void on_cboxUserLogo_activated(const QString &arg1);


private:
    Ui::mainInterface *ui;

    DataInfo *dataInfo;
    DataAnalysis *dataAnalysis;
    RemoteControl *remoteControl;
    LoginInfo *logInfo;
    SystemMaintain *systemMaintain;



    void statusBarInit();

    void initStyle();



};

#endif // MAININTERFACE_H
