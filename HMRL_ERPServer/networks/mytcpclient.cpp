#include "mytcpclient.h"

#include <QHostAddress>
#include "qhostaddress.h"


#include "networks/mytcpclient.h"
#include "helperclass.h"

myTcpClient::myTcpClient(QObject *parent,int clientID) :
    QTcpSocket(parent)
{
    this->clientID=clientID;
    connect(this,SIGNAL(readyRead()),this,SLOT(ReadData()));//挂接读取数据信号
    connect(this,SIGNAL(disconnected()),this,SLOT(DisConnect()));//关闭连接时，发送断开连接信号
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpClientReadError()));

    //如果关闭连接自动删除，则下次不能再次监听，奇怪的问题
    //connect(this,SIGNAL(disconnected()),this,SLOT(deleteLater()));//关闭连接时，对象自动删除
}
myTcpClient::~myTcpClient()
{
    qDebug()<<"TcpClient下线了";
    deleteLater();
}
void myTcpClient::ReadData()
{
    if (bytesAvailable() <= 0) {
        return;
    }

    //HelperClass::Sleep(100);
    //读取完整一条数据并发送信号
    QByteArray data=readAll();
    qDebug()<<"读客户端数据"<<data;
    emit ClientReadData(clientID,peerAddress().toString(),peerPort(),data);
}

void myTcpClient::DisConnect()
{
    //断开连接时，发送断开信号
    emit ClientDisConnect(clientID,peerAddress().toString(),peerPort());
}
void myTcpClient::tcpClientReadError()
{
    QString data;
    disconnectFromHost();

    data=tr("Current IP:%1,Port:%2-->Connect fault : %3").arg(peerAddress().toString()).arg(peerPort()).arg(errorString());

    emit ClientConnectionError(clientID,peerAddress().toString(),peerPort(),data);
    //AppendTcpInfo(1, QString("连接服务器失败,原因 : %1").arg(tcpClient->errorString()));
}
