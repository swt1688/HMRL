#include "spollserver.h"
#include <QDebug>

#include <QSqlQuery>
#include <QTime>



#include "protocol/protocolclass.h"
#include "helperclass.h"

//QVector<quint16> SPollServer::ListenPortlist;
//QVector<QString> SPollServer::UploadIP;

//#include "ui_spollserver.h"

//SPollServer::SPollServer(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::SPollServer)
SPollServer::SPollServer()
{
    //ui->setupUi(this);

    this->setAttribute (Qt::WA_DeleteOnClose);//当close时会析构该对象
    setMinimumSize(1024,600);
    setWindowTitle(tr("污染源在线综合管理系统--服务端"));

    TopLayout = new QGridLayout(this);
    LeftLayout = new QHBoxLayout(this);
    textBrowser=new QTextBrowser(this);
    textBrowser->setStyleSheet(QString::fromUtf8("background-color: rgb(22, 0, 33);\n"
                    "font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
    txtFilter= new QLineEdit(this);
    btnTextBrowserClean=new QPushButton(tr("清除"),this);
    connect(btnTextBrowserClean,&QPushButton::clicked,[=](){textBrowser->clear();});
    LeftLayout->addWidget(txtFilter);
    LeftLayout->addWidget(btnTextBrowserClean);
    //TopLayout->addWidget(txtFilter,0,0);
    //TopLayout->addWidget(btnTextBrowserClean,0,1);
    TopLayout->addLayout(LeftLayout,0,0);
    TopLayout->addWidget(textBrowser,1,0);
    TopLayout->setContentsMargins(2,2,2,2);//设置左上右下空白

    loginDialog();

    TCP_ServiceInit();
    UDP_ServiceInit();
    qDebug()<<"此步能否正常执行1";

}

SPollServer::~SPollServer()
{
    //delete ui;

    //测试多线程的结束

    for(auto thread:ListenThreads)
    {
        qDebug()<<tr("执行到@0x%1").arg(quintptr(QThread::currentThreadId()));

        thread->stopListen();
        thread->deleteLater();

        //thread->wait(1000);
    }
    //udpServer->deleteLater();

    qDebug()<<"SPollServer类要退出了哦";
    //this->deleteLater();
    //this->close();
    //emit us_Close();
}


void SPollServer::creataActions()
{
}

void SPollServer::loginDialog()
{
    systemTray = new QSystemTrayIcon(this);
    //放在托盘提示信息、托盘图标
    systemTray->setToolTip(QString("环保在线"));
    systemTray->setIcon(QIcon(":/ICON/tree.ico"));

    //创建托盘项
    action_show = new QAction(QIcon(":/ICON/mms.png"),tr("显示"),this);
    action_quit = new QAction(QIcon(":/ICON/delete-v2.png"),tr("退出"),this);
    action_about = new QAction(QIcon(":/ICON/help.png"),tr("关于"),this);
    action_setting = new QAction(QIcon(":/ICON/properties.png"),tr("设置"),this);
    connect(action_show, &QAction::triggered, this, &SPollServer::show);
    connect(action_quit, &QAction::triggered, this, &SPollServer::us_Close);
    connect(action_setting,&QAction::triggered,this,&SPollServer::OpenProcess);

    menu = new QMenu(this);
    menu->addAction(action_show);
    menu->addAction(action_setting);
    menu->addAction(action_about);
    menu->addSeparator();
    menu->addAction(action_quit);
    systemTray->setContextMenu(menu);

    connect(systemTray, &QSystemTrayIcon::activated, this, &SPollServer::iconActivated);

    systemTray->show();
        //托盘显示提示信息
    systemTray->showMessage(QString("环保数据在线"), QString("您身边的环保专家"));

}
void SPollServer::OpenProcess()
{
    // 判断进程是否存在
    qDebug()<<"打开了吗";
    QProcess *tasklist=new QProcess(this);
    tasklist->start(QApplication::applicationDirPath() + "/sourceOfPollution.exe" );


}
void SPollServer::showMessage(char *msg)
{
    QString titlec=tr(msg);
    QString textc=tr("测试内容单击、双击、中键、按钮");
    systemTray->showMessage(titlec, textc, QSystemTrayIcon::Information, 5000);
}
void SPollServer::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //触发后台图标执行相应事件
       switch (reason)
       {
       case QSystemTrayIcon::Trigger:
           systemTray->show();
           showMessage("鼠标单击！");
           break;
       case QSystemTrayIcon::DoubleClick:
           showMessage("鼠标双击！");
           break;
       case QSystemTrayIcon::MiddleClick:
           showMessage("鼠标中键！");
           break;
       default:
           break;
       }
}
void SPollServer::closeEvent(QCloseEvent *event)
{

    if(systemTray->isVisible())
    {
        hide();
        systemTray->showMessage("提示", "程序过入后台运行!");

        event->ignore();

    }
}




/********************************************************************
 * TCP/IP通讯部分，
 * ******************************************************************/
void SPollServer::TCP_ServiceInit()
{
    processor=new ProcessorProtocol;//开始数据的载入
    QObject::connect(processor,&ProcessorProtocol::ReadData,this,&SPollServer::RecordData);
    processor->start();

    //服务器版本才需进行监听，打开端口，开始监听
//     QSqlQuery query;
    QString strs="SELECT * FROM `onlinedatabase`.ListenPortTable where selectStatus ='1';";
    query.exec(strs);

    while (query.next())
   //for(int i=0;i<SPollServer::ListenPortlist.count();i++)
    {
        StartListenServer(query.value(1).toUInt());
    }
    //服务端发送数据到客户端
    //connect(dataInfo,&DataInfo::TCPServerSendData,this,&mainInterface::TCPServerSendDataToClient);

    ClientConnectServer();//TCP客户端的初始化
}

void SPollServer::StartListenServer(quint16 ListenPort)
{
    ServerThread* server_thread = new ServerThread(QHostAddress::Any,ListenPort,this);
    //server_thread = new ServerThread();
    ListenThreads[ListenPort]= server_thread;
    //ListenThreads.insert(query.value(1).toInt(),server_thread);
    connect(ListenThreads[ListenPort],&ServerThread::NetClientData,this,&SPollServer::RecordData);
    //ListenThreads.value(query.value(1).toInt())->startListen(QHostAddress::Any,query.value(1).toInt());

    ListenThreads.value(ListenPort)->start(QThread::HighestPriority);//线程开始运行
    //connect(ListenThreads[ListenPort],&ServerThread::NetClientStatus,this,&SPollServer::OnlineInfo);
}

void SPollServer::RecordData(QString MN_Code,quint8 type, QString msg)
{
    if(txtFilter->text().isEmpty())
    {
        AppendTcpInfo(type, msg);
    }
    else
    {
        if(MN_Code==txtFilter->text())
        {
            AppendTcpInfo(type, msg);
        }
    }
}

void SPollServer::AppendTcpInfo(quint8 type, QString msg)
{
    //static int msgMaxCount=100;//最大信息条数
    static int CurrentCountTcp=0;//当前数据条数

    if (CurrentCountTcp > 100)
    {
        //ui->textBrowser->clearHistory();
        //ui->textBrowser->setPlainText("");//这是清空textBrowser 里面所有数据
        textBrowser->clear();
        CurrentCountTcp = 0;
    }

    if (type == 0) {
        textBrowser->setTextColor(QColor("dodgerblue"));//str = ">> 发送 :";
    }
    else if (type == 1) {
        textBrowser->setTextColor(QColor("red"));//str = "<< 接收 :";
    }
    else if(type == 2)
    {
        textBrowser->setTextColor(QColor("white"));//str = "<< 下线 :";
    }
    else if(type == 3)
    {
        textBrowser->setTextColor(QColor("yellow"));//str = "<< 下线 :";
    }

    textBrowser->append(QString("%0 时间[%1]-->%2").arg(CurrentCountTcp).arg(QTime::currentTime().toString("HH:mm:ss zzz")).arg(msg));
    CurrentCountTcp++;
}

//void SPollServer::OnlineInfo(int clientID,QString IP,int Port,QString MN_Serial,bool status)
//{
//    QString IP_Port;
//    IP_Port=tr("%1:%2").arg(IP).arg(Port);
//   if(status==true)
//    {
//        onlineList[MN_Serial]=IP_Port;
//        QString str=tr("在线企业数:%1,IP::%2,MN::%3-->上线").arg(onlineList.count()).arg(onlineList.value(MN_Serial)).arg(onlineList.key(IP_Port));
//        AppendTcpInfo(0,str);
//    }
//    else
//    {
//        QString str=tr("在线企业数:%1,IP::%2,MN::%3-->下线").arg(onlineList.count()-1).arg(onlineList.value(MN_Serial)).arg(onlineList.key(IP_Port));
//        AppendTcpInfo(2,str);
//        onlineList.remove(MN_Serial);
//    }
//    if(BaseInfo::MN_Searil==MN_Serial)
//    {
//        BaseInfo::clientID=clientID;
//        BaseInfo::IPAddress=IP;
//        BaseInfo::PortNum=Port;
//        BaseInfo::OnLineStatus=status;
//        ui->labTitle->setText(tr("当前企业:%1(--联网状态:%2)").arg(BaseInfo::CompanyName).arg(OnLineFlag.at(BaseInfo::OnLineStatus)));
//        //ui->textBrowser->setText(tr("当前企业:%1(--联网状态:%2)").arg(BaseInfo::CompanyName).arg(OnLineFlag.at(BaseInfo::OnLineStatus)));
//    }

//}
/************************连接到其它服务器************************************/
void SPollServer::ClientConnectServer()
{
    //QSqlQuery query;
    QString strs="SELECT * FROM `onlinedatabase`.ipaddress where selectStatus ='1';";
    query.exec(strs);
    while (query.next())
    //for(int i=0;i<SPollServer::UploadIP.count();i++)
   {
        myTcpClient *tcpClient = new myTcpClient(this);

        //tcpClient->abort();
        //str<<tr("%1:%2").arg(query.value(1).toString()).arg(query.value(2).toInt());
        //UploadIP.insert(query.value(1).toString(),query.value(2).toInt());
        tcpClients.insert(tr("%1:%2").arg(query.value(1).toString()).arg(query.value(2).toInt()),tcpClient);
        //if(query.value(0).toBool()==true)
        //{
            ConnectionToServer(0,query.value(1).toString(),query.value(2).toInt());
            //AutoTcpConnect(query.value(1).toString(),query.value(2).toInt(),query.value(0).toBool());
        //}
        connect(tcpClient, &myTcpClient::ClientReadData, this, &SPollServer::tcpClientReadData);
        connect(tcpClient, &myTcpClient::ClientDisConnect, this, &SPollServer::ConnectionToServer);
        connect(tcpClient, &myTcpClient::ClientConnectionError, this, &SPollServer::tcpClientReadError);
   }
    //connect(dataInfo,&DataInfo::TCPClientSendData,this,&mainInterface::TcpClientSendDataToServer);
}
void SPollServer::TCPServerSendDataToClient(quint16 ClientPort,QString ClientAddress,QByteArray strByteArray)
{
    //显示发送数据
    if(ListenThreads.contains(ClientPort))
    {
        ListenThreads.value(ClientPort)->sendDataTcpServer(ClientAddress,strByteArray);
        AppendTcpInfo(0, tr("发送数据->%1-->%2").arg(ClientAddress).arg(QString(strByteArray)));
    }
    else
    {
        AppendTcpInfo(3, tr("发送错误,端口%0不存在或有误-->%1-->%2").arg(ClientPort).arg(ClientAddress).arg(QString(strByteArray)));//上位机设置的端口与下位机的端口不能对应
    }
}
//客户端连到服务器
void SPollServer::ConnectionToServer(int clientID,QString IP,int Port)
{
    //int Port;
    QString IP_Port;
    IP_Port=tr("%1:%2").arg(query.value(1).toString()).arg(query.value(2).toInt());
    //QStringList strs=IP_Port.split(":");
    //IP=strs.at(0);
    //Port=strs.at(1);

//    if (ConnectionAction == true) {
        if (tcpClients[IP_Port]->state() == QAbstractSocket::UnconnectedState )
        {
            //qDebug()<<str;
            tcpClients[IP_Port]->connectToHost(IP, Port);

            if (tcpClients[IP_Port]->waitForConnected(1000))
            {
                QString queryStrs="UPDATE `onlinedatabase`.IPAddress SET ConnectionStatus ='1', where IPAddress='"+IP+"' and Port='"+Port+"';";
                //QSqlQuery query;
                query.exec(queryStrs);
                AppendTcpInfo(0, tr("连接服务器成功-->IP:%1,Port:%2").arg(IP).arg(Port));
                BootTime(IP_Port);
            }
            else
            {
                AppendTcpInfo(1, tr("连接服务器失败-->IP:%1,Port:%2,%3").arg(IP).arg(Port).arg(tcpClients[IP_Port]->errorString()));
            }
        }

//    } else {
//        tcpClients[str]->disconnectFromHost();

//        if (tcpClients[str]->state() == QAbstractSocket::UnconnectedState || tcpClients[str]->waitForDisconnected(1000))
//        {
//            AppendTcpInfo(0, tr("断开连接成功-->IP:%1,Port:%2").arg(TcpServerIP).arg(TcpServerPort));
//        }
//    }
}

//连接后立即上传的数据
void SPollServer::BootTime(QString ClientName)
{
    QString CN_Code,ST_Code,MN_Password,MN_Searil,CP_Code,CurrentCommand;
    ST_Code="91";
    QByteArray strByteArray;
    //QRegExp MoveChar("[- :]");//正则表达式

    QString str=QDateTime::currentDateTime().toString("yyyyMMddHHmmss");//.remove(MoveChar);//去掉字符串中间的- :
    CN_Code="CN=2081;";//上传现场开关机时间
    CP_Code="CP=&&DataTime="+str+";RestartTime="+QDateTime::currentDateTime().toString("yyyyMMddHHmmss")+"&&";
    CurrentCommand="上传现场开关机时间";

    strByteArray=ProtocolClass::HBGBDataCollection(CN_Code,ST_Code,MN_Password,MN_Searil,CP_Code);
    TcpClientSendDataToServer(ClientName,strByteArray);

    //AppendTcpInfo(0, tr("%1->%2").arg(CurrentCommand).arg(QString(strByteArray)));//.arg(QString(QThread::currentThreadId())).
}

//客户端发送数据
void SPollServer::TcpClientSendDataToServer(QString ClientName,const QByteArray data)
{
    if(tcpClients[ClientName]->state() == QAbstractSocket::UnconnectedState)
    {
        AppendTcpInfo(0, tr("当前连接已断开%1").arg(tcpClients[ClientName]->errorString()));
    }
    if (!tcpClients[ClientName]->isWritable()) {
        return;
    }

    if (data.isEmpty()) {
        return;
    }
    tcpClients[ClientName]->write(data);
    AppendTcpInfo(0, tr("%1").arg(ClientName)+data);
}

void SPollServer::tcpClientReadData(int clientID,QString IP,int Port,QByteArray data)
{
    QString buffer;
    buffer=data;
    AppendTcpInfo(1, tr("接收到服务端数据[%1:%2]-->%3").arg(IP).arg(Port).arg(buffer));
}

void SPollServer::tcpClientReadError(int clientID,QString TcpServerIP,int TcpServerPort,QString data)
{
    QString strs="UPDATE `onlinedatabase`.IPAddress SET ConnectionStatus ='0', where IPAddress='"+TcpServerIP+"' and Port='"+TcpServerPort+"';";
    //QSqlQuery query;
    query.exec(strs);
    //ui->textBrowser->setText(tr("错误的信息[%1:%2]-->%3").arg(TcpServerIP).arg(TcpServerPort).arg(data));
    AppendTcpInfo(0,tr("错误的信息[%1:%2]-->%3").arg(TcpServerIP).arg(TcpServerPort).arg(data));
}
/****************************开始监听UDP端口********************************************/
void SPollServer::UDP_ServiceInit()
{
    udpServer=new UdpServer(52114,this);
    connect(udpServer,&UdpServer::UdpServerInfo,this,AppendTcpInfo);//显示UDP接收到的数据
    connect(udpServer,&UdpServer::TCPServerSendData,this,&SPollServer::TCPServerSendDataToClient);//发送反控数据到TCP客户端
    connect(udpServer,&UdpServer::UdpListenPortUpdata,this,&SPollServer::StartListenServer);//监听TCP端口
    udpServer->start();
}

