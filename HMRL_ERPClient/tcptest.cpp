#include "tcptest.h"
#include "ui_tcptest.h"

#include "helperclass.h"
#include "connectionsetup.h"

#include <QDebug>
#include <QHostInfo>
#include <QNetworkInterface>
//#include <QNetworkAccessManager>

TCPtest::TCPtest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TCPtest)
{
    ui->setupUi(this);
    this->Init();
}

TCPtest::~TCPtest()
{
    delete ui;
}

void TCPtest::Init()
{
    //ui->txtIP->setText(ConnectionSetup::IP_Address_01);
    //ui->txtPortClient->setText(ConnectionSetup::PortNumber_01);
    //ui->txtPortServer->setText(ConnectionSetup::PortNumber_01);

    ui->btnSendClient->setEnabled(false);
    ui->txtDataClient->setReadOnly(true);

    tcpClient=new QTcpSocket(this);
    tcpClient->abort();//取消原有连接
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(ReadData()));
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ReadError(QAbstractSocket::SocketError)));
    //设置服务端
    ui->btnSendServer->setEnabled(false);
    ui->txtDataServer->setReadOnly(true);//设置服务端窗口为只读

    tcpServer=new myTcpServer(this);

    connect(tcpServer,SIGNAL(ClientConnect(int,QString,int)),this,SLOT(ClientConnect(int,QString,int)));
    connect(tcpServer,SIGNAL(ClientDisConnect(int,QString,int)),this,SLOT(ClientDisConnect(int,QString,int)));
    connect(tcpServer,SIGNAL(ClientReadData(int,QString,int,QByteArray)),this,SLOT(ClientReadData(int,QString,int,QByteArray)));

    for (int i=10;i<=1000;i=i+10)
    {
        ui->cboxMax->addItem(QString::number(i));//最大连接数
    }

    QString localHostName = QHostInfo::localHostName();
    ui->labHostName->setText(tr("主机名:%1").arg(localHostName));
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            ui->labIPAddress->setText(tr("IPV4:%1").arg(address.toString()));
    }
//    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
//    foreach(QNetworkInterface interface,list)
//    {
//        qDebug() <<"Device:"<<interface.name();
//        qDebug() << "HardwareAddress:"<<interface.hardwareAddress();
//        QList<QNetworkAddressEntry> entryList = interface.addressEntries();
//        //获取IP地址条目列表，每个条目中包含一个IP地址，一个子网掩码和一个广播地址
//        foreach(QNetworkAddressEntry entry,entryList)
//        {
//            qDebug()<<"IP Address:"<<entry.ip().toString();
//            qDebug()<<"Netmask: "  <<entry.netmask().toString();
//            qDebug()<<"Broadcast:" <<entry.broadcast().toString();
//        }
//    }



}

void TCPtest::ReadData()
{
    QByteArray buffer=tcpClient->readAll();
    if (!buffer.isEmpty())
    {
        if (ui->ckHexReceiveClient->isChecked())
        {
            ui->txtDataClient->append(tr("接收数据:%1 时间:%2")
                                      .arg(HelperClass::ByteArrayToHexStr(buffer)).arg(QTime::currentTime().toString("hh:mm:ss")));
        }
        else
        {
            ui->txtDataClient->append(tr("接收数据:%1 时间:%2")
                                      .arg(QString(buffer)).arg(QTime::currentTime().toString("hh:mm:ss")));
        }
    }
}

void TCPtest::ReadError(QAbstractSocket::SocketError)
{
    tcpClient->disconnectFromHost();
    ui->btnConnect->setText("连接");
    ui->txtDataClient->append(tr("连接服务器失败,原因:%1").arg(tcpClient->errorString()));
}

void TCPtest::ClientReadData(int clientID,QString IP,int Port,QByteArray data)
{
    if (!data.isEmpty())
    {
        if (ui->ckHexReceiveServer->isChecked())
        {
            ui->txtDataServer->append(tr("接收数据:%1 来自:[clientID:%2 IP:%3 Port:%4] 时间:%5")
                                      .arg(QString(HelperClass::ByteArrayToHexStr(data)))
                                      .arg(clientID).arg(IP).arg(Port).arg(QTime::currentTime().toString("hh:mm:ss")));
        }
        else
        {
            ui->txtDataServer->append(tr("接收数据:%1 来自:[clientID:%2 IP:%3 Port:%4] 时间:%5")
                                      .arg(QString(data)).arg(clientID).arg(IP).arg(Port).arg(QTime::currentTime().toString("hh:mm:ss")));
        }
    }
}

void TCPtest::ClientConnect(int clientID,QString IP,int Port)
{
    ui->txtDataServer->append(tr("客户端:[clientID:%1 IP:%2 Port:%3]上线 时间:%4")
                              .arg(clientID).arg(IP).arg(Port).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->cboxClient->addItem(tr("%1:%2:%3").arg(clientID).arg(IP).arg(Port));
}

void TCPtest::ClientDisConnect(int clientID,QString IP,int Port)
{
    ui->txtDataServer->append(tr("客户端:[clientID:%1 IP:%2 Port:%3]下线 时间:%4")
                              .arg(clientID).arg(IP).arg(Port).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->cboxClient->removeItem(ui->cboxClient->findText(tr("%1:%2:%3").arg(clientID).arg(IP).arg(Port)));
}

void TCPtest::on_btnSendServer_clicked()
{
    QString txt=ui->txtSendServer->text();
    if (txt==""){return;}

    QString str=ui->cboxClient->currentText();
    int clientID=str.split(":")[0].toInt();
    QByteArray tempData;

    if (ui->ckHexSendServer->isChecked())
    {
        tempData=HelperClass::HexStrToByteArray(txt);
        tcpServer->SendData(clientID,tempData);
    }
    else
    {
        tempData=txt.toLatin1();
        tcpServer->SendData(clientID,tempData);
    }
}

void TCPtest::on_btnSendClient_clicked()
{
    QString data=ui->txtSendClient->text();

    if (data!="")
    {
        if (ui->ckHexSendClient->isChecked())
        {
            tcpClient->write(HelperClass::HexStrToByteArray(data));
        }
        else
        {
            tcpClient->write(data.toLatin1());
        }
    }
}

void TCPtest::on_btnListen_clicked()
{
    if (ui->btnListen->text()=="监听")
    {
        bool ok=tcpServer->listen(QHostAddress::Any,ui->txtPortServer->text().toInt());
        if (ok)
        {
            ui->btnListen->setText("停止");
            ui->txtDataServer->append("监听成功");
            ui->btnSendServer->setEnabled(true);
        }
    }
    else
    {
        tcpServer->CloseAll();//断开所有已连接的客户端
        tcpServer->close();//停止监听
        ui->btnListen->setText("监听");
        ui->txtDataServer->append("停止监听成功");
        ui->btnSendServer->setEnabled(false);
    }
}

void TCPtest::on_btnClearServer_clicked()
{
    ui->txtDataServer->clear();
}

void TCPtest::on_btnConnect_clicked()
{
    if (ui->btnConnect->text()=="连接")
    {
        tcpClient->connectToHost(ui->txtIP->text(),ui->txtPortClient->text().toInt());
        if (tcpClient->waitForConnected(1000))
        {
            ui->btnConnect->setText("断开");
            ui->txtDataClient->append("连接服务器成功");
            ui->btnSendClient->setEnabled(true);
        }
    }
    else
    {
        tcpClient->disconnectFromHost();
        if (tcpClient->state() == QAbstractSocket::UnconnectedState || tcpClient->waitForDisconnected(1000))
        {
            ui->btnConnect->setText("连接");
            ui->txtDataClient->append("断开连接成功");
            ui->btnSendClient->setEnabled(true);
        }
    }
}

void TCPtest::on_btnClearClient_clicked()
{
    ui->txtDataClient->clear();
}

void TCPtest::on_cboxMax_currentIndexChanged(int index)
{
    tcpServer->setMaxPendingConnections(index+1);
}


