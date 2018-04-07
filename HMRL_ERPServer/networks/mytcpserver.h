#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QUdpSocket>
#include <QThread>
#include <QHostAddress>

#include <QDebug>
#include <QSqlQuery>

#include "networks/mytcpclient.h"

#include <QMutex>
#include <QSemaphore>
#include <QWaitCondition>
//#include <QStringList>

extern QByteArrayList list;
extern QSemaphore freeBytes;
extern QSemaphore usedBytes;
extern QString WriteToDatabase;

// 枚举变量标志信息的类型，分别为消息，新用户加入，用户退出，文件名，拒绝接受文件

enum MessageType{Message, NewParticipant, ParticipantLeft,ControlCommand,UpdataEnterprise,DelEnterprise,ListenPortUpdata, FileName, Refuse};

class myTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit myTcpServer(QObject *parent = 0);
    ~myTcpServer();
    bool start(QHostAddress address, int port);

private:
    //QList<myTcpClient *> ClientList;
    QList<int> ClientIDs;
    myTcpClient *CurrentClient;
    QHash <QString,myTcpClient *>ClientsList;



protected:
    void incomingConnection(int handle);

signals:
    void ClientReadData(int clientID,QString IP,int Port,QByteArray data);
    void ClientConnect(int clientID,QString IP,int Port);
    void ClientDisConnect(int clientID,QString IP,int Port);
    void ServerData(QString ClientInfo, QByteArray data);
    void CurrentThread(QString ThreadID,int clientCount,QString ClientInfo,QByteArray strByteArray);

private slots:
    void DisConnect(int clientID,QString IP,int Port);

public slots:
    void SendDataArray(QString ClientInfo, QByteArray data);
    void SendData(int clientID, QByteArray data);
    void SendData(QString ip, int port, QByteArray data);
    void SendData(QByteArray data);//对所有客户端发送数据
    void SendDataCurrent(QByteArray data);//对当前客户端发送数据

    void CloseAll();
};

/****************************多线程TCP服务类******************************/

class ServerThread: public QThread
{
    Q_OBJECT
public:
    //explicit ServerThread(QObject *parent=0);
    explicit ServerThread(QHostAddress Default_Address=QHostAddress::Any,quint16 Default_Port=6666,QObject *parent=0);
    //virtual ~ServerThread();
    //explicit ServerThread();
    ~ServerThread();
    virtual void run()Q_DECL_OVERRIDE;

    void startListen(QHostAddress address,quint16 port);
    void sendDataTcpServer(QString ClientInfo, QByteArray data);
    void stopListen();

    //QHash<QString,QStringList>MonitorPiontInfo;

private slots:
    void ClientReadData(int clientID,QString IP,int Port,QByteArray data);
    void ClientDisConnect(int clientID,QString IP,int Port);
    void ClientConnect(int clientID,QString IP,int Port);

signals:
    void WriteData(QString ClientInfo, QByteArray data);
    void CurrentThread(QString ThreadID,int clientCount,QString ClientInfo,QByteArray strByteArray);

    void NetClientStatus(int clientID,QString IP,int Port,QString MN_Serial,bool status);//网络部分
    void NetClientData(QString MN_Code,quint8 type, QString msg);


private:
    QHostAddress Host_address;
    quint16 Listen_Port;
    QHash<QString,QString>OnlineMonitor;//QHash<MN_Code,IP_Port>   键(MN_Code)<---->值(IP_Port)

    myTcpServer *server;
    volatile bool stopped;
    QString PW_Code,MN_Code;
    QString strs,txtDisplay;

};


/****************************多线程数据的处理协议类******************************/
class ProcessorProtocol : public QThread
{
    Q_OBJECT
public:
    ProcessorProtocol();
    ~ProcessorProtocol();
    void run();

signals:
    void ReadData(QString MN_Code,quint8 type, QString msg);

private slots:

private:

    QSqlQuery query;
    volatile bool stopped;
    QByteArray data;
    QString str;
};

/***************************UPD数据处理协议************************************/
class UdpServer : public QThread
{
    Q_OBJECT
public:
    explicit UdpServer(quint16 Listen=5200,QObject *parent=0);
    ~UdpServer();
    void run();
private slots:
    void UdpServerReadData();
    void UdpServerWrite(QString data,QString ip, int port);
    void UdpServerReadError();
    void UdpSendMessage(MessageType type);
    QString getUserName();
    QString getIP();  //获取ip地址
    void DataProcess(QByteArray data);
signals:
    void UdpServerInfo(quint8 type, QString msg);//将UDP收到的数据发到主线程中
    void UdpServerSendData(QString data,QString ip, int port);//发送数据到客户端
    void UdpEnterpriseUpdata(QString EnterpriseName);//更新载入内存的企业
    void UdpListenPortUpdata(quint16 ListenPort);//增加监听端口
    void TCPServerSendData(quint16 ClientPort,QString ClientAddress,QByteArray strByteArray);//TCP服务端发送信息到客户端
private:
    QUdpSocket *udpServer;
    quint16 ListenPort;           //监听端口


};
#endif // MYTCPSERVER_H
