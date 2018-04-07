#ifndef SPOLLSERVER_H
#define SPOLLSERVER_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QGridLayout>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>


#include "networks/mytcpserver.h"



//namespace Ui {
//class SPollServer;
//}




class SPollServer : public QWidget
{
    Q_OBJECT

public:
    //explicit SPollServer(QWidget *parent = 0);
    SPollServer();
    ~SPollServer();

    //static QVector<quint16> ListenPortlist;
    //static QVector<QString> UploadIP;
signals:
    void us_Close();

private slots:
    void OpenProcess();
    void TCPServerSendDataToClient(quint16 ClientPort,QString ClientAddress,QByteArray strByteArray);//TCP服务端发送信息到客户端
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage(char *msg);
    void RecordData(QString MN_Code,quint8 type, QString msg);
    void AppendTcpInfo(quint8 type, QString msg);

    void closeEvent(QCloseEvent *event);
    void loginDialog();
    void creataActions();

    //TCP_IP通讯部分
    void TCP_ServiceInit();
    void StartListenServer(quint16 ListenPort);
    void UDP_ServiceInit();
    void ClientConnectServer();

    void ConnectionToServer(int clientID,QString IP,int Port);
    void BootTime(QString ClientName);
    void TcpClientSendDataToServer(QString ClientName,const QByteArray data);
    void tcpClientReadData(int clientID,QString IP,int Port,QByteArray data);
    void tcpClientReadError(int clientID,QString TcpServerIP,int TcpServerPort,QString data);

//    void UdpServerSendData(QString ip, int port, QString data);
//    void UdpServerReadData();
//    void UdpServerReadError();

private:
    //Ui::SPollServer *ui;
    QSystemTrayIcon *systemTray;
    QGridLayout *TopLayout;
    QHBoxLayout *LeftLayout;
    QTextBrowser *textBrowser;
    QLineEdit *txtFilter;
    QPushButton *btnTextBrowserClean;

    QProcess *m_pProcess;

    QMap<quint16,ServerThread *>ListenThreads;//服务端多线程
    QMap<QString,myTcpClient *> tcpClients;
    UdpServer *udpServer;

    QSqlQuery query;

    ProcessorProtocol *processor;//多线程数据库处理
    //QHash<QString,int>UploadIP;//连接客户端信息

    QAction *action_show;
    QAction *action_quit;
    QAction *action_about;
    QAction *action_setting;
    QMenu *menu;
};

#endif // SPOLLSERVER_H
