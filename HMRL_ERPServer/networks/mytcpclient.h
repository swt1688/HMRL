#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H


#include <QTcpSocket>

class myTcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit myTcpClient(QObject *parent = 0,int clientID=0);
    ~myTcpClient();

private:
    int clientID;

signals:
    void ClientReadData(int clientID,QString IP,int Port,QByteArray data);
    void ClientDisConnect(int clientID,QString IP,int Port);
    void ClientConnectionError(int clientID,QString IP,int Port,QString data);

private slots:
    void ReadData();
    void DisConnect();
    void tcpClientReadError();

public slots:

};

#endif // MYTCPCLIENT_H
