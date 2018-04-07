#include "mytcpserver.h"

#include <QDateTime>
#include <QProcess>
#include <QHostInfo>
#include <QDataStream>
#include <QNetworkInterface>
#include <QTextCodec>

#include "protocol/protocolclass.h"
#include "DBProcess/dbconnectionpool.h"


//QStringList myTcpServer::clientInfo;
myTcpServer::myTcpServer(QObject *parent) :
    QTcpServer(parent)
{
    //clientCount=0;
    connect(this,&myTcpServer::ServerData,this,&myTcpServer::SendDataArray);
}
myTcpServer::~myTcpServer()
{
    qDebug()<<"myTcpServer类终于退出了";
    this->CloseAll();
}
bool myTcpServer::start(QHostAddress address, int port)
{
    return listen(address, port);
}

void myTcpServer::incomingConnection(int handle)
{

//    SocketThread *thread = new SocketThread(socketDescriptor,this);
//    Processor *cpu=new Processor(thread->socket);
//    connect(thread->socket,SIGNAL(readyRead()),cpu,SLOT(work()));
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    cpu->moveToThread (thread);
//    thread->start();

    myTcpClient *client=new myTcpClient(this,handle);
    client->setSocketDescriptor(handle);

    QString ip =  client->peerAddress().toString();
    int port = client->peerPort();
    //clientInfo.append(QString("%1:%2").arg(ip).arg(port));//客户端信息IP地址与端口号

    ClientIDs.append(handle);//将新的连接的ID添加到客户端ID列表
    ClientsList.insert(QString("%1:%2").arg(ip).arg(port),client);
    //clientCount++;
    CurrentClient=client;//存储当前连接

    connect(client,&myTcpClient::ClientReadData,this,&myTcpServer::ClientReadData);
    //connect(client,&myTcpClient::ClientDisConnect,this,&myTcpServer::DisConnect);
    //connect(client,&myTcpClient::disconnected,[=](){delete client;});
    connect(client,&myTcpClient::ClientConnectionError,[=](){/*switch(client->error())*/qDebug()<<tr("IP%1 Port%2--ErrorInfo%3").arg(ip).arg(port).arg(client->errorString());});

    connect(client,&myTcpClient::ClientDisConnect,[=](){ClientsList.value(QString("%1:%2").arg(ip).arg(port))->deleteLater();ClientsList.remove(QString("%1:%2").arg(ip).arg(port));emit ClientDisConnect(handle,ip,port);});

    emit ClientConnect(handle, ip, port);

}


void myTcpServer::DisConnect(int clientID,QString IP,int Port)
{
    ClientsList.value(QString("%1:%2").arg(IP).arg(Port))->deleteLater();
    ClientsList.remove(QString("%1:%2").arg(IP).arg(Port));
    emit ClientDisConnect(clientID,IP,Port);
    //this->deleteLater();

}
void myTcpServer::SendData(QString ip, int port, QByteArray data)
{
    ClientsList.value(QString("%1:%2").arg(ip).arg(port))->write(data);

}
void myTcpServer::SendDataArray(QString ClientInfo, QByteArray data)
{

    qDebug()<<"当前所有的客户端"<<ClientsList.count()<<ClientsList;
    qDebug()<<"当前线程ID"<<this->thread();
    if(ClientsList.contains(ClientInfo))
    {
        ClientsList.value(ClientInfo)->write(data);
        emit CurrentThread( QStringLiteral("@0x%1").arg(quintptr(QThread::currentThreadId())),ClientsList.count(),ClientInfo,data);
    }
    else
    {
        emit CurrentThread( QStringLiteral("@0x%1").arg(quintptr(QThread::currentThreadId())),ClientsList.count(),ClientInfo,tr("client is null").toLatin1());
    }
}
//指定客户端连接发消息
void myTcpServer::SendData(int clientID, QByteArray data)
{
    for (int i=0;i<ClientIDs.count();i++)
    {
        if (ClientIDs.at(i)==clientID)
        {
            ClientsList.value(ClientsList.keys().at(i))->write(data);
            break;
        }
    }
}
//对所有连接发送数据
void myTcpServer::SendData(QByteArray data)
{
    for (int i = 0; i < ClientsList.count(); i++) {
        ClientsList.value(ClientsList.keys().at(i))->write(data);
    }
}
//对当前连接发送数据
void myTcpServer::SendDataCurrent(QByteArray data)
{
    //如果没有一个存在的连接，则不处理
    if (ClientsList.count()<1){return;}
    CurrentClient->write(data);
}

void myTcpServer::CloseAll()
{
    foreach (myTcpClient *client, ClientsList.values())
    {

        client->deleteLater();
        client->close();
    }


    this->close();
}

/**********************多线程多端口监听***************
 *
 *
*************************************************/

//QSemaphore ProducerBytes(8192);//256是这个list的缓冲区大小，当然可以随意设定；256的意思是初始空闲的缓冲区大小是256
//QSemaphore UsedBytes(0);//初始被使用的缓冲区大小是0

QWaitCondition ProducerBytes;
QWaitCondition UsedBytes;
QMutex mutex;//使用QMutex互斥量保证对线程操作的原子性
QByteArrayList list;
//QVector<QByteArray>list;
QString WriteToDatabase;


ServerThread::ServerThread(QHostAddress Default_Address,quint16 Default_Port,QObject *parent):Host_address(QHostAddress::Any),Listen_Port(6006),QThread(parent)
{
    Host_address=Default_Address;
    Listen_Port=Default_Port;
    stopped=false;
}
//ServerThread::ServerThread()
//{

//}
ServerThread::~ServerThread()
{
    static int i=0;

    disconnect(server);

    requestInterruption();
    //emit NetClientData(MN_Code,1,tr("关闭端口[Port:%1]->@0x%2->@0x%3").arg(Listen_Port).arg(quintptr(QThread::currentThreadId()),0,16).arg(quintptr(QThread::currentThread()),0,16));
    qDebug()<<tr("%1-->关闭端口>%2").arg(i).arg(Listen_Port);
    wait(200);
    quit();
    //terminate();//强制中止
    exit();
    wait(200);
    i++;


}

void ServerThread::run()
{

    server = new myTcpServer;
    if(!server->start(Host_address, Listen_Port))
    {
        qDebug()<<tr("监听端口失败[Port:%1]").arg(Listen_Port);
        quit();//监听失败，退出线程
    }
    qDebug()<<tr("TCP监听端口成功[Port:%1]").arg(Listen_Port)<<QThread::currentThreadId()<<QThread::currentThread();
    emit NetClientData(MN_Code,1,tr("TCP监听端口成功[Port:%1]->@0x%2->@0x%3").arg(Listen_Port).arg(quintptr(QThread::currentThreadId()),0,16).arg(quintptr(QThread::currentThread()),0,16));
    connect(server ,&myTcpServer::ClientConnect,this,&ServerThread::ClientConnect);
    connect(server ,&myTcpServer::ClientDisConnect,this,&ServerThread::ClientDisConnect);
    //connect(server,&myTcpServer::acceptError,[=](){/*switch(client->error())*/qDebug()<<tr("ErrorInfo%1").arg(server->errorString());});
    connect(server ,&myTcpServer::ClientReadData,this,&ServerThread::ClientReadData,Qt::DirectConnection);
    connect(server ,&myTcpServer::CurrentThread,this,&ServerThread::CurrentThread);

    connect(this ,&ServerThread::WriteData,server,&myTcpServer::ServerData);

    exec();
    //}

}


void ServerThread::startListen(QHostAddress address,quint16 port)
{
    Host_address = address;
    Listen_Port = port;
}

void ServerThread::stopListen()
{
    stopped=true;
}

void ServerThread::sendDataTcpServer(QString ClientInfo, QByteArray data)
{
    emit WriteData(ClientInfo, data);
}

void ServerThread::ClientReadData(int clientID,QString IP,int Port,QByteArray data)
{
    if (!data.isEmpty())
    {

        short StringPosition_01,StringPosition_02;

        StringPosition_01=data.indexOf("MN=");   //查找“MN=”
        if(StringPosition_01!=-1)
        {
            StringPosition_02 = data.indexOf(";",StringPosition_01);
            MN_Code=data.mid(StringPosition_01+3,StringPosition_02-StringPosition_01-3);
        }
        QString IP_Port;
        IP_Port=tr("%1:%2").arg(IP).arg(Port);
        if(ProtocolClass::MonitorPiontInfo.keys().contains(MN_Code.trimmed()))//检测MN_Code是否存在
        {

            txtDisplay=tr("收到数据->MN:%1->ClientID:%2 IP=%3:%4\r\n%5").arg(MN_Code).arg(clientID).arg(IP).arg(Port).arg(QString(data));

            if((!OnlineMonitor.keys().contains(MN_Code))and(OnlineMonitor.value(MN_Code)!=IP_Port))//检测当前MN号对应的IP是否存在
            {
                QString CN_Code;
                StringPosition_01=data.indexOf("CN=");   //查找“MN=”
                if(StringPosition_01!=-1)
                {
                    StringPosition_02 = data.indexOf(";",StringPosition_01);
                    CN_Code=data.mid(StringPosition_01+3,StringPosition_02-StringPosition_01-3);
                }
                QString currentTimes=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                strs="UPDATE `onlinedatabase`.monitorpointinfo SET  OnlineTime='"+currentTimes+"',LogIPAddress='"+IP+"',LogPort='"+QString::number(Port)+"', LogStatus ='1' where MN_Code='"+MN_Code+"';"
                     "INSERT INTO `"+MN_Code+"`.rtustartdatabase  (CN_Code,IP_Address,OnlineTime) value('"+CN_Code+"','"+tr("%1").arg(IP)+"','"+currentTimes+"');";

                //ProducerBytes.acquire();//先申请一个位置，相当于空闲的缓冲区-1
                mutex.lock();
                if(!WriteToDatabase.isEmpty())
                    ProducerBytes.wait(&mutex);
                WriteToDatabase+=strs;
                UsedBytes.wakeAll();
                mutex.unlock();
                //UsedBytes.release();//给使用的大小+1（因为这个是后加的，所以在提取的时候总是在它加完之后）

                OnlineMonitor[MN_Code]=IP_Port;
                //emit NetClientData(MN_Code,1,tr("当前所在端口:%1,IP::%2,MN::%3").arg(Listen_Port).arg(OnlineMonitor.value(MN_Code)).arg(OnlineMonitor.key(IP_Port)));
                emit NetClientStatus(clientID,IP,Port,MN_Code,true);
            }
            //ProducerBytes.acquire();//先申请一个位置，相当于空闲的缓冲区变为79
            mutex.lock();
            if(!list.count()==100)
                ProducerBytes.wait(&mutex);
            list.append(data);
            UsedBytes.wakeAll();
            mutex.unlock();

            //txtDisplay=ProtocolClass::HBGBProtocolServer(data);
            //UsedBytes.release();//给使用的大小+1（因为这个是后加的，所以在提取的时候总是在它加完之后）
            //ProtocolClass::HBGBProtocolServer(data,MN_Code,ProtocolClass::MonitorPiontInfo.value(MN_Code));
        }
        else
        {
            //emit NetClientData(mainInterface::MonitorPiontInfo.keys().join("***"),1,tr("监听端口:%1,%2").arg(Listen_Port).arg(mainInterface::MonitorPiontInfo.value(MN_Code).join("---")));
            strs="INSERT INTO `onlinedatabase`.TempReceiver (DataTime,MN_CODE,ReceiverData,flag) VALUES ('"+QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")+"','"+MN_Code+"','"+QString(data)+"','1')";
            //ProducerBytes.acquire();//先申请一个位置，相当于空闲的缓冲区-1
            mutex.lock();
            if(!WriteToDatabase.isEmpty())
                ProducerBytes.wait(&mutex);
            WriteToDatabase+=strs;
            UsedBytes.wakeAll();
            mutex.unlock();
            //UsedBytes.release();//给使用的大小+1（因为这个是后加的，所以在提取的时候总是在它加完之后）
            txtDisplay = tr("Error:当前站点未注册，请确认是否加入该站点-->MN_CODE:%1,PW_CODE:%2,数据(%3)").arg(MN_Code).arg(PW_Code).arg(strs);
        }
        emit NetClientData(MN_Code,1,txtDisplay);
    }
}

void ServerThread::ClientDisConnect(int clientID,QString IP,int Port)
{
    QString IP_Port;
    IP_Port=tr("%1:%2").arg(IP).arg(Port);
    if(!OnlineMonitor.values().contains(IP_Port))
    {
        emit NetClientData(OnlineMonitor.key(IP_Port),1, tr("未注册的客户端[clientID:%1 IP:%2 Port:%3] 下线").arg(clientID).arg(IP).arg(Port));
        return;
    }
    else
    {
        //QString str=MonitorPiontInfo.value(IP_Port.at(0)).at(0);
        emit NetClientStatus(clientID,IP,Port,OnlineMonitor.key(IP_Port),false);
        //emit NetClientData(OnlineMonitor.key(IP_Port),1, QString("客户端[clientID:%1 IP:%2 Port:%3 MN码:%4] 下线").arg(clientID).arg(IP).arg(Port).arg(OnlineMonitor.key(IP_Port)));
        //emit NetClientData(tr("监听端口:%1,%2").arg(Listen_Port).arg(MonitorPiontInfo.value(IP_Port.at(0)).join("---")),1,QString("未注册的客户端[clientID:%1 IP:%2 Port:%3] 下线").arg(clientID).arg(IP).arg(Port));
        QString strs="UPDATE `onlinedatabase`.monitorpointinfo SET LogStatus ='0',LastLoginTime='"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"' where MN_Code='"+OnlineMonitor.key(IP_Port)+"';"
        "UPDATE `"+OnlineMonitor.key(IP_Port)+"`.rtustartdatabase SET  OfflineTime = '"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"' where IP_Address = '"+tr("%1").arg(IP)+"' order by ID desc limit 1;";

        //ProducerBytes.acquire();//先申请一个位置，相当于空闲的缓冲区-1
        mutex.lock();
        if(!WriteToDatabase.isEmpty())
            ProducerBytes.wait(&mutex);
        WriteToDatabase+=strs;
        UsedBytes.wakeAll();
        mutex.unlock();
        //UsedBytes.release();//给使用的大小+1（因为这个是后加的，所以在提取的时候总是在它加完之后）

        OnlineMonitor.remove(OnlineMonitor.key(IP_Port));
    }
}
void ServerThread::ClientConnect(int clientID,QString IP,int Port)
{
    emit NetClientData(0,1, tr("监控端口%0-->客户端[clientID:%1 IP:%2 Port:%3]上线").arg(Listen_Port).arg(clientID).arg(IP).arg(Port));
    //emit NetClientData(0,1, QString("客户端[clientID:%1 IP:%2 Port:%3] 当前的客户端%4上线").arg(clientID).arg(IP).arg(Port).arg(OnlineMonitor.keys().join("--")));
}

/****************************多线程数据的处理协议类******************************/
ProcessorProtocol::ProcessorProtocol()
{
    stopped=false;
    QSqlDatabase db = DBConnectionPool::openConnection();
    qDebug() << "In thread run():" << db.connectionName();
    QSqlQuery query(db);
    query.exec("SELECT MN_Code,factorCode FROM `onlinedatabase`.MonitorInfo where ApplyEnable='启用';");
    while(query.next())
    {
        ProtocolClass::MonitorPiontInfo[query.value(0).toString()].append(query.value(1).toString());
    }
    foreach(QString MN,ProtocolClass::MonitorPiontInfo.keys())
    {//将
        query.exec("UPDATE `"+MN+"`.rtustartdatabase SET  OfflineTime = '"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"',OfflineCause='服务器重启',ReasonSubmitPerson='system' order by ID desc limit 1;");
    }
    query.exec("SELECT distinct factorCode,factorName FROM `onlinedatabase`.monitorinfo;");
    while(query.next())
    {
        ProtocolClass::FactorCodeToName[query.value(0).toString()].append(query.value(1).toString());
    }
    qDebug()<<ProtocolClass::MonitorPiontInfo;

}
ProcessorProtocol::~ProcessorProtocol()
{
    stopped=true;
    requestInterruption();
    quit();
    //terminate();//强制中止
    exit();
    wait();
}
void ProcessorProtocol::run()
{
    while(!stopped)
    {
        //msleep(5);
        mutex.lock();
        while(!list.isEmpty())
        {
            //UsedBytes.acquire();
            data=list.takeFirst();
            str=ProtocolClass::HBGBProtocolServer(data);
            WriteToDatabase+=str;
            emit ReadData("执行了",1,str);

            //ProtocolClass::HBGBProtocolServer(data,MN_Code,mainInterface::MonitorPiontInfo.value(MN_Code));
            //ProducerBytes.release();
        }
//        if(!WriteToDatabase.isEmpty())
//        {
//            UsedBytes.acquire();
//            query.exec(WriteToDatabase);
//            WriteToDatabase.clear();
//            ProducerBytes.release();

//        }
        if(WriteToDatabase.isEmpty())
            UsedBytes.wait(&mutex);
        query.exec(WriteToDatabase);
        WriteToDatabase.clear();
        ProducerBytes.wakeAll();
        mutex.unlock();
    }
}

/**************************************UDP服务端程序**********************/
UdpServer::UdpServer(quint16 Listen,QObject *parent):ListenPort(5200),QThread(parent)
{
    ListenPort=Listen;
}
UdpServer::~UdpServer()
{
    quit();
    //terminate();//强制中止
    exit();
    qDebug()<<"Udp线程退出了哦";
    wait();
}

void UdpServer::run()
{

    udpServer = new QUdpSocket;
    udpServer->abort();

    /*********bind是个重载函数，连接本机的port端口
     * 采用ShareAddress模式(即允许其它的服务连接到相同的地址和端口，特别是用在多客户端监听同一个服务器端口
     * 时特别有效)，和ReuseAddressHint模式(重新连接服务器)  *****************************/
    //bool ok = udpServer->bind(ListenPort);
    if (!udpServer->bind(ListenPort,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    //if(!udpServer->bind(ListenPort))
    {
        emit UdpServerInfo(3, tr("UDP端口%1监听失败,请检查端口是否被占用").arg(ListenPort));
        quit();
    }
    emit UdpServerInfo(3, tr("UDP端口%1监听成功").arg(ListenPort));
    //此处加上Qt::DirectConnection,不加只能接收到第一条数据，
    connect(udpServer, &QUdpSocket::readyRead, this,&UdpServer::UdpServerReadData,Qt::DirectConnection);
    connect(this,&UdpServer::UdpServerSendData,this,UdpServerWrite);

    UdpSendMessage(NewParticipant); //打开软件时就向外发射本地信息，让其他在线用户得到通知

    exec();
}
void UdpServer::UdpServerReadData()
{
    QHostAddress senderServerIP;
    quint16 senderServerPort;
    QByteArray data;
    QString buffer;

    //hasPendingDatagrams返回true时表示至少有一个数据报在等待被读取
    while(udpServer->hasPendingDatagrams())
    {
        data.resize(udpServer->pendingDatagramSize());//pendingDatagramSize为返回第一个在等待读取报文的size，resize函数是把datagram的size归一化到参数size的大小一样
        //将读取到的不大于data.size()大小数据输入到data.data()中，data.data()返回的是一个字节数组中存储数据位置的指针
        udpServer->readDatagram(data.data(), data.size(), &senderServerIP, &senderServerPort);
        buffer=QTextCodec::codecForLocale()->toUnicode(data);//将传来的数据中出现的中文字符正确转换
        QString str = QString("来自:%1[%2] ").arg(senderServerIP.toString()).arg(senderServerPort);

        QStringList datalist;
        QString IP,UserName,currentTime;
        quint16 ClientPort;
        datalist=buffer.split('$');
        //s=datalist.at(0).toUInt();
        UserName=datalist.at(1);
        currentTime=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        switch(datalist.at(0).toUInt())
        {
            case ControlCommand://反控信息
            {
                /****|    0   | 1  |   2   |  3   |    4  |  5 |  6 |*****
                 ****｜指令类型｜用户｜监听端口｜IP地址｜反控指令|MN码|CN码|*****
                ***********************************************************/
                //QByteArray Com
                ClientPort=datalist.at(2).toUInt();
                IP=datalist.at(3);
                emit TCPServerSendData(ClientPort,IP,datalist.at(4).toLatin1());
                //将反控信息记录到数据库
                str=tr("INSERT INTO `onlinedatabase`.RemoteControlTable (DataTime,UserName,MN_Code,CN_Code,ControlCommand) VALUES ('%1','%2','%3','%4','%5');")
                        .arg(currentTime).arg(UserName).arg(datalist.at(5)).arg(datalist.at(6)).arg(datalist.at(4));
                //qDebug()<<str;
                WriteToDatabase+=str;
                break;
            }
            case NewParticipant://用户登录
            {

                str=tr("INSERT INTO `onlinedatabase`.UserLogin (LoginTime,LoginName,IP_Address,IP_Port,PC_Name,PC_User) VALUES ('%1','%2','%3','%4','%5','%6');UPDATE `onlinedatabase`.userinfo set OnlineStatus='1', IP_Address='%7',IP_Port='%8',LoginTime='%9' where UserName= '%10';")
                        .arg(currentTime).arg(UserName).arg(senderServerIP.toString()).arg(senderServerPort).arg(datalist.at(2)).arg(datalist.at(4))
                        .arg(senderServerIP.toString()).arg(senderServerPort).arg(currentTime).arg(UserName);
                //qDebug()<<str;
                WriteToDatabase+=str;
                break;
            }
            case ParticipantLeft://用户退出
            {
                str=tr("UPDATE `onlinedatabase`.userlogin SET LogoutTime ='%1' WHERE LoginName='%2' order by ID desc limit 1;UPDATE `onlinedatabase`.userinfo set OnlineStatus='0' where UserName= '%3';")
                        .arg(currentTime).arg(UserName).arg(UserName);
                //qDebug()<<str;
                WriteToDatabase+=str;
                break;
            }
            case UpdataEnterprise:
            {
                QSqlDatabase db = DBConnectionPool::openConnection();
                buffer+=tr("-->In thread run():").arg(db.connectionName());
                QStringList MN_list;
                QSqlQuery query(db);
                QString queryMN="SELECT MN_Code FROM `onlinedatabase`.enterpriselist where CompanyName='"+datalist.at(2)+"';";
                emit UdpServerInfo(3, tr("查询MN号-->(%1)").arg(queryMN));
                query.exec(queryMN);
                while (query.next())
                {
                    emit UdpServerInfo(3, tr("检测到的MN号%1").arg(query.value(0).toString()));
                    MN_list<<query.value(0).toString();
                    if(ProtocolClass::MonitorPiontInfo.contains(query.value(0).toString()))
                        ProtocolClass::MonitorPiontInfo.remove(query.value(0).toString());
                }
                query.exec("SELECT MN_Code,factorCode FROM `onlinedatabase`.monitorinfo where CompanyName='"+datalist.at(2)+"' and ApplyEnable='启用';");
                while (query.next())
                {
                   ProtocolClass::MonitorPiontInfo[query.value(0).toString()].append(query.value(1).toString());
                }
                // 连接使用完后需要释放回数据库连接池
                DBConnectionPool::closeConnection(db);
                //str.clear();
                for(int i=0;i<MN_list.count();i++)
                {
                    str+=tr("\r\n更新企业监测因子(企业MN号:%1-->监测因子:%2").arg(MN_list.at(i)).arg(ProtocolClass::MonitorPiontInfo.value(MN_list.at(i)).join(","));
                }
                break;

            }
            case DelEnterprise:
            {
                ProtocolClass::MonitorPiontInfo.remove(datalist.at(2));break;
            }
            case ListenPortUpdata:
            {
                emit UdpListenPortUpdata(datalist.at(2).toUInt());break;
            }

        }
    emit UdpServerInfo(3, tr("%1%2--%3").arg(str).arg(buffer));
    }
    emit UdpServerSendData("OK,你发的数据已收到",senderServerIP.toString(), senderServerPort);
}

void UdpServer::DataProcess(QByteArray data)
{
    QByteArrayList datalist;
    QString IP,UserName,str;
    quint16 ClientPort;
    datalist=data.split('$');
    //s=datalist.at(0).toUInt();
    UserName=QTextCodec::codecForLocale()->toUnicode(datalist.at(1));
    switch(datalist.at(0).toUInt())
    {
        case ControlCommand:
        {

            ClientPort=datalist.at(2).toUInt();
            IP=datalist.at(3);
            emit TCPServerSendData(ClientPort,IP,datalist.at(4));
            break;
        }
    case NewParticipant:
    {
        QString str;
        str=tr("INSERT INTO `onlinedatabase`.UserLogin (LoginTime,LoginName,IP_Address,IP_Port) VALUES (%1,%2,%3,%4);").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(UserName);
    }

    }
}

void UdpServer::UdpServerWrite(QString data,QString ip, int port)
{
    if (!data.isEmpty())
    {
        QByteArray buffer;
        buffer=data.toLocal8Bit();
        udpServer->writeDatagram(buffer, QHostAddress(ip), port);

        QString str = QString("目标:%1[%2] ").arg(ip).arg(port);
        emit UdpServerInfo(0, str + data);
    }
}


void UdpServer::UdpServerReadError()
{
    emit UdpServerInfo(1, QString("UDP发生错误,原因 : %1").arg(udpServer->errorString()));
}

/********使用UDP广播发送信息，MessageType是指头文件中的枚举数据类型,sendMessage即把本机的主机名，用户名+（消息内容后ip地址）广播出去******/
void UdpServer::UdpSendMessage(MessageType type)
{
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();//返回主机名，QHostInfo包含了一些关于主机的静态函数
    QString address = getIP();    //调用自己类中的getIP()函数
    //将type，getUserName()，localHostName按照先后顺序送到out数据流中，消息类型type在最前面
    //out << type << getUserName() << localHostName;

    switch(type)
    {
    case Message :

        out << address << "信息来了哦";//将ip地址和得到的消息内容输入out数据流

        break;

    case NewParticipant :
        out << address;    //为什么此时只是输出地址这一项呢？因为此时不需要传递聊天内容
        break;

    case ParticipantLeft :
        break;

    case FileName :
        break;

    case Refuse :
        break;
    }
    //一个udpSocket已经于一个端口bind在一起了，这里的data是out流中的data，最多可以传送8192个字节，但是建议不要超过
    //512个字节，因为这样虽然可以传送成功，但是这些数据需要在ip层分组，QHostAddress::Broadcast是指发送数据的目的地址
    //这里为本机所在地址的广播组内所有机器，即局域网广播发送
    QString str;
    str.append("我启动了，和我一起喊ABC123");
    str.append(getUserName());
    str.append(address);
    str.append(localHostName);

    //udpServer->writeDatagram(str.toLocal8Bit(),str.toLocal8Bit().length(),QHostAddress("127.0.0.1"), 52118);
    udpServer->writeDatagram(str.toLocal8Bit(),str.toLocal8Bit().length(),QHostAddress::Broadcast, 52118);//将data中的数据发送
}

// 获取用户名
QString UdpServer::getUserName()
{
    QStringList envVariables;
    //将后面5个string存到envVariables环境变量中
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    //系统中关于环境变量的信息存在environment中
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables) {
        //indexOf为返回第一个匹配list的索引,QRegExp类是用规则表达式进行模式匹配的类
        int index = environment.indexOf(QRegExp(string));
        if (index != -1) {
            //stringList中存的是environment.at(index)中出现'='号前的字符串
            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2) {
                return stringList.at(1);//at(0)为文字"USERNAME."，at(1)为用户名
                break;
            }
        }
    }
    return "unknown";
}

QString UdpServer::getIP()  //获取ip地址
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
       if(address.protocol() == QAbstractSocket::IPv4Protocol) //我们使用IPv4地址
            return address.toString();
    }
       return 0;
}
