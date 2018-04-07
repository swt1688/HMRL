#ifndef TCPTEST_H
#define TCPTEST_H

#include <QDialog>
#include <QWidget>
#include "networks/mytcpserver.h"

namespace Ui {
class TCPtest;
}

class TCPtest : public QDialog
{
    Q_OBJECT

public:
    explicit TCPtest(QWidget *parent = 0);
    ~TCPtest();

private slots:

    //客户端槽函数
    void ReadData();
    void ReadError(QAbstractSocket::SocketError);

    //服务端槽函数
    void ClientReadData(int clientID,QString IP,int Port,QByteArray data);
    void ClientConnect(int clientID,QString IP,int Port);
    void ClientDisConnect(int clientID,QString IP,int Port);

    void on_btnSendServer_clicked();

    void on_btnSendClient_clicked();

    void on_btnListen_clicked();

    void on_btnClearServer_clicked();

    void on_btnConnect_clicked();

    void on_btnClearClient_clicked();

    void on_cboxMax_currentIndexChanged(int index);

private:
    Ui::TCPtest *ui;
    void Init();

    QTcpSocket *tcpClient;
    myTcpServer *tcpServer;
};


#endif // TCPTEST_H
