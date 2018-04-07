#ifndef SERIALPORTTEST_H
#define SERIALPORTTEST_H

#include <QDialog>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QtserialPort/QSerialPort>
#include <QtserialPort/QSerialPortInfo>
#include <QFile>
#include <QTextStream>

#include "SystemMaintain/connectionsetup.h"

namespace Ui {
class SerialPortTest;
}

class SerialPortTest : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortTest(QWidget *parent = 0);
    ~SerialPortTest();

    void SerachSerial();//查找串口

    static QStringList comList;//串口号
    static QStringList baudList;//波特率
    static QStringList parityList;//校验位
    static QStringList dataBitsList;//数据位
    static QStringList stopBitsList;//停止位

    void Read_ComData();
    void Write_ComData();//写串口数据

protected:
    bool eventFilter(QObject *obj,QEvent *event);//监听事件

private slots:

    void on_btnManData_clicked();
    void on_btnSaveData_clicked();
    void on_btnClearAll_clicked();
    void on_btnSendButton_clicked();
    void on_btnStopShow_clicked();
    void on_btnSendClear_clicked();
    void on_btnOpenSerail_clicked();

    void on_ckHexSend_stateChanged(int arg1);
    void on_ckHexReceive_stateChanged(int arg1);
    void on_ckAutoSend_stateChanged(int arg1);
    void on_ckAutoSave_stateChanged(int arg1);
    void on_cklsAutoClear_stateChanged(int arg1);
    void on_cklsDebug_stateChanged(int arg1);



    void SaveComData();//保存串口读取到的数据

    void ReadComConfig();//读取配置文件数据
    void ReadSendData();

    void on_btnReturn_clicked();

    void on_spboxSendTime_valueChanged(int arg1);
    void on_spboxSaveTime_valueChanged(int arg1);

private:
    Ui::SerialPortTest *ui;

    void InitSerialCom();
    void ChangeEnable(bool b);//改变状态

    QSerialPort *serial;

    QTimer *myTimer;

    QTimer *myReadTimer;//定时读取串口数据
    QTimer *mySendTimer;//定时发送串口数据
    QTimer *mySaveTimer;//定时保存串口数据

    int SendCount;//发送数据计数
    int ReceiveCount;//接收数据计数
    bool IsShow;//是否显示数据
    bool IsDebug;//是否启用调试,接收到数据后模拟发送数据
    bool IsAutoClear;//是否自动清空
    bool IsHexSend;//是否16进制数据发送
    bool IsHexReceive;//是否16进制数据接收

    QStringList SendDataList;//转发数据链表
};



#include <QObject>
#include <QSemaphore>
#include <QMutex>
#include <QRegExp>
class QThread;
class QSerialPort;
class MySerialPort : public QObject
{
    Q_OBJECT
public:
    explicit MySerialPort(const QString strComName = "");
    ~MySerialPort();
    bool isOpen() const;
    bool setCOM(const QString strCOM = "", const int iBautRate = 9600, const int iDataBits = 8, const char chParity = 'N', const char chStopBits = 1);
    qint64 write(const QByteArray &byteArray);
    qint64 write(const char *data, qint64 maxSize = -1);
    void close();
    void clear();
    //bool EchoCommand(const QString & strCmd, const QRegExp& rx = QRegExp(""), QString * pstrReply = 0);
    bool EchoCommand(const QString &strCmd, const QString &strExpectReply = "", QString *pstrReply = 0);
    //==================================================
protected:
    QString m_strComName;
    //========================
    QThread *m_pThread;
    QSerialPort *m_pCom;
    //=========================
    //write
    QMutex m_lockWrite;
    QSemaphore m_semWrite;
    QMutex m_lockWriteLen;
    qint64 m_iLen;
    //openCom
    QMutex m_lockSetCOM;
    QSemaphore m_semSetCOM;
    bool m_bOpen;
    //close
    QMutex m_lockClose;
    QSemaphore m_semClose;
    //clear
    QMutex m_lockClear;
    QSemaphore m_semClear;
    //=========================
    QMutex m_lockInBuffer;
    QString m_strInBuffer;
    QString m_strReply;

    QMutex m_lockExpect;
    QString m_strExpectReply;
    QRegExp m_rxExpectReply;

    QMutex m_lockEcho;
    QSemaphore m_semEcho;
    bool m_bEchoRegExp;
private:
    bool m_bEchoFlag;
    mutable QMutex m_lockEchoFlag;
    void setEchoFlag(bool b)
    {
        QMutexLocker lk(&m_lockEchoFlag);
        m_bEchoFlag = b;
    }
    bool getEchoFlag() const
    {
        QMutexLocker lk(&m_lockEchoFlag);
        return m_bEchoFlag;
    }

signals:
    void sigDataReady(QByteArray);
    void sigWrite(const char *, qint64);
    void sigSetCOM(const QString, const int, const int, const char, const char);
    void sigClose();
    void sigClear();
private slots:
    void slotDataReady();
    //-----------------
    void slotWrite(const char *pch, qint64);
    void slotSetCOM(const QString strCOM, const int iBautRate, const int iDataBits, const char chParity, const char chStopBits);
    void slotClose();
    void slotClear();
};

#endif // SERIALPORTTEST_H
