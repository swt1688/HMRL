#include "serialporttest.h"
#include "ui_serialporttest.h"

#include "helperclass.h"
#include "logoin.h"

#ifdef Q_OS_WIN//如果是windows系统
   QStringList SerialPortTest::comList={"COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15"};//"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"<<"COM7"<<"COM8"<<"COM9"<<"COM10"<<"COM11"<<"COM12"<<"COM13"<<"COM14"<<"COM15";
#else//如果是unix或者其他系统
   QStringList SerialPortTest::comList={"ttyUSB0","ttyUSB1","ttyUSB2","ttyUSB3","ttyUSB4","ttyUSB5","ttyS0","ttyS1","ttyS2","ttyS3","ttyS4","ttyS5","ttyS6","ttyS7","ttyS8","ttyS9"};//"ttyUSB0"<<"ttyUSB1"<<"ttyUSB2"<<"ttyUSB3"<<"ttyUSB4"<<"ttyUSB5"<<"ttyS0"<<"ttyS1"<<"ttyS2"<<"ttyS3"<<"ttyS4"<<"ttyS5"<<"ttyS6"<<"ttyS7"<<"ttyS8"<<"ttyS9";
#endif
   QStringList SerialPortTest::baudList={"1200","2400","4800","9600","19200","38400","57600","115200"};//<<"1200"<<"2400"<<"4800"<<"9600"<<"19200"<<"38400"<<"57600"<<"115200";

#ifdef Q_OS_WIN//如果是windows系统
   QStringList SerialPortTest::parityList={"无校验","奇校验","偶校验","标志"};//<<"无校验"<<"奇校验"<<"偶校验";
    //parityList<<"标志";
#else
   QStringList SerialPortTest::parityList={"无校验","奇校验","偶校验","空格"};//<<"无校验"<<"奇校验"<<"偶校验";
   //parityList<<"空格";
#endif
   QStringList SerialPortTest::dataBitsList={"5","6","7","8"};//<<"5"<<"6"<<"7"<<"8";

#ifdef Q_OS_WIN//如果是windows系统
   QStringList SerialPortTest::stopBitsList={"1","1.5","2"};//<<"1";stopBitsList<<"1.5";
#else
   QStringList SerialPortTest::stopBitsList={"1","2"};//<<"1";stopBitsList<<"1.5"; stopBitsList<<"2";
#endif

SerialPortTest::SerialPortTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPortTest)
{
    ui->setupUi(this);

    HelperClass::FormInCenter(this);
    InitSerialCom();

    //qDebug()<<tr("界面设定成功");
    on_btnOpenSerail_clicked();
}

SerialPortTest::~SerialPortTest()
{
    delete ui;
}
//监听回车键
bool SerialPortTest::eventFilter(QObject *obj, QEvent *event)
{
    if (obj==ui->txtSendData)
    {
        if (event->type()==QEvent::KeyPress)
        {
            QKeyEvent *keyEvent=static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_Return || keyEvent->key()==Qt::Key_Enter)
            {
                Write_ComData();
                return true;
            }
        }
    }

    return QObject::eventFilter(obj,event);
}
void SerialPortTest::Write_ComData()
{
    //QSerialPort serial;
    QString str=ui->txtSendData->currentText();
    if (str==""){ui->txtSendData->setFocus();return;}//发送数据为空
    //if (!serial->open(QIODevice::ReadWrite))
    if (!serial->isOpen())
    {
        ui->ReceiveText->setText(tr("串口未打开"));
        return;
    }//串口没有打开

    QByteArray outData=str.toLatin1();
    int size=outData.size();

    if (IsHexSend)//转化为16进制发送
    {
        outData=HelperClass::HexStrToByteArray(str);
        size=outData.size();
        serial->write(outData);
    }
    else
    {
        size=outData.size();
        serial->write(outData);
    }

    ui->ReceiveText->append(QString("发送:%1 时间:%2")
                           .arg(str)
                           .arg(QTime::currentTime().toString("HH:mm:ss")));

    SendCount=SendCount+size;
    ui->labSendByte->setText(QString("发送:%1 字节").arg(SendCount));

    if (IsAutoClear)
    {
        ui->txtSendData->setCurrentIndex(-1);
        ui->txtSendData->setFocus();
    }
}

//初始化
void SerialPortTest::InitSerialCom()
{
    ReceiveCount=0;
    SendCount=0;
    IsShow=true;
    IsAutoClear=false;
    IsHexSend=true;
    IsHexReceive=true;
    IsDebug=false;




    ui->cboxPortName->addItems(SerialPortTest::comList);//串口列表
    ui->cboxPortName->setCurrentText(ConnectionSetup::PortName);

    ui->cboxBaudRate->addItems(SerialPortTest::baudList);//波特率
    ui->cboxBaudRate->setCurrentText(ConnectionSetup::BaudRate);

    ui->cboxParity->addItems(SerialPortTest::parityList);//校验位
    ui->cboxParity->setCurrentIndex(ConnectionSetup::Parity);

    ui->cboxDataBit->addItems(SerialPortTest::dataBitsList);//数据位
    ui->cboxDataBit->setCurrentIndex(ConnectionSetup::DataBit);

    ui->cboxStopBit->addItems(SerialPortTest::stopBitsList);//停止位
    ui->cboxStopBit->setCurrentIndex(ConnectionSetup::StopBit);

    SerachSerial();
       //显示日期时间
//        myTimer=new QTimer(this);
//        myTimer->start(1000);
//        QObject::connect(myTimer,&QTimer::timeout,this,&mainInterface::timerUpDate);
        //connect(myTimer,SIGNAL(timeout()),this,SLOT(SetTime()));
    //ui->cboxPortName->addItem(serial.portName());
            //定时读串口
            myReadTimer=new QTimer(this);
            myReadTimer->setInterval(300);
            QObject::connect(myReadTimer,&QTimer::timeout,this,&SerialPortTest::Read_ComData);
            //connect(myReadTimer,SIGNAL(timeout()),this,SLOT(ReadMyCom()));

            //发送数据
            mySendTimer=new QTimer(this);
            mySendTimer->setInterval(ui->spboxSendTime->value());
            QObject::connect(mySendTimer,&QTimer::timeout,this,&SerialPortTest::Write_ComData);

            //保存数据
            mySaveTimer=new QTimer(this);
            mySaveTimer->setInterval(ui->spboxSaveTime->value());
            QObject::connect(mySaveTimer,&QTimer::timeout,this,&SerialPortTest::SaveComData);

//    for (int i=1;i<=60;i++)
//    {
//        ui->cboxSendTime->addItem(QString::number(i)+"s");
//        ui->cboxSaveTime->addItem(QString::number(i)+"s");
//    }

    ui->spboxSendTime->setValue(1000);
    ui->spboxSaveTime->setValue(1000);


    this->ChangeEnable(false);
    this->ReadComConfig();//读取发送数据加载到下拉框
    this->ReadSendData();//读取数据转发文件

    ui->txtSendData->installEventFilter(this);//安装监听器监听发送数据框回车响应
}

//查找串口，并关闭串口
void SerialPortTest::SerachSerial()
{
    //读取数据(采用定时器读取数据，不采用事件，方便移植到linux)
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        qDebug()<<"Name : "<<info.portName();
        qDebug()<<"description : "<<info.description();
        qDebug()<<"Manufacturer : "<<info.manufacturer();
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {

            serial.close();
            //ui->cboxPortName->addItem(serial.portName());
            //读取数据
        }
    }
}
void SerialPortTest::Read_ComData()
{
    QByteArray buffer;
    if(serial->bytesAvailable()<=0){return;}
    HelperClass::Sleep(100);
    buffer = serial->readAll();
    if(buffer!=NULL)
    {
        if (IsShow)
        {
            if (IsHexReceive)
            {
//                QString str = ui->ReceiveText->toPlainText();
//                str+=tr(buf);
//                ui->ReceiveText->clear();
//                ui->ReceiveText->append(str);

                QString tempDataHex=HelperClass::ByteArrayToHexStr(buffer);
                ui->ReceiveText->append(QString("接收:%1 时间:%2")
                                       .arg(tempDataHex)
                                       .arg(QTime::currentTime().toString("HH:mm:ss")));

                if (IsDebug)//2013-8-6增加接收数据后转发数据，模拟设备
                {
                    foreach(QString tempData,SendDataList)
                    {
                        QStringList temp=tempData.split(';');
                        if (tempDataHex==temp[0])
                        {
                            //这里没有跳出循环，有可能一条数据会对应多条数据需要转发
                            serial->write(HelperClass::HexStrToByteArray(temp[1]));
                        }
                    }
                }
            }
            else
            {
                QString tempDataNormal=QString(buffer);
                ui->ReceiveText->append(QString("接收:%1 时间:%2")
                                       .arg(tempDataNormal)
                                       .arg(QTime::currentTime().toString("HH:mm:ss")));

                if (IsDebug)//2013-8-6增加接收数据后转发数据，模拟设备
                {
                    foreach(QString tempData,SendDataList)
                    {
                        QStringList temp=tempData.split(';');
                        if (tempDataNormal==temp[0])
                        {
                            //这里没有跳出循环，有可能一条数据会对应多条数据需要转发
                            serial->write(temp[1].toLatin1());
                        }
                    }
                }
            }

            ReceiveCount=ReceiveCount+buffer.size();
            ui->labReceiveByte->setText(QString("接收:%1 字节").arg(ReceiveCount));
        }
    }
    buffer.clear();
}

void SerialPortTest::ChangeEnable(bool b)
{
    ui->cboxBaudRate->setEnabled(!b);
    ui->cboxDataBit->setEnabled(!b);
    ui->cboxParity->setEnabled(!b);
    ui->cboxPortName->setEnabled(!b);
    ui->cboxStopBit->setEnabled(!b);

    //这样的话保证每次关闭串口后,自动发送和自动保存定时器不会空转
    ui->ckAutoSend->setChecked(false);
    ui->ckAutoSave->setChecked(false);

}

void SerialPortTest::on_btnManData_clicked()
{
    QString fileName=Logoin::AppPath+"/localization/SerialData.txt";
    QFile file(fileName);

    if (!file.exists()){ HelperClass::ShowMessageBoxError("数据文件不存在！"); return;}

    if (ui->btnManData->text()=="管理数据")
    {
        ui->ReceiveText->setReadOnly(false);
        ui->gReceive->setTitle(tr("管理数据"));
        ui->ReceiveText->clear();

        file.open(QFile::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        ui->ReceiveText->setText(in.readAll());
        file.close();

        ui->btnManData->setText(tr("保存数据"));
    }
    else
    {
        ui->ReceiveText->setReadOnly(true);
        ui->gReceive->setTitle(tr("接收数据"));

        file.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out<<ui->ReceiveText->toPlainText();
        file.close();

        ui->ReceiveText->clear();
        ui->btnManData->setText(tr("管理数据"));

        this->ReadComConfig();
    }
}

void SerialPortTest::on_btnSaveData_clicked()
{
    this->SaveComData();
}


void SerialPortTest::on_btnClearAll_clicked()
{
    ui->ReceiveText->clear();
}
void SerialPortTest::on_btnSendButton_clicked()
{
    this->Write_ComData();
}

void SerialPortTest::on_btnStopShow_clicked()
{
    if (ui->btnStopShow->text()=="停止显示")
    {
        IsShow=false;
        ui->btnStopShow->setText("开始显示");
    }
    else
    {
        IsShow=true;
        ui->btnStopShow->setText("停止显示");
    }
}



void SerialPortTest::on_btnSendClear_clicked()
{
    SendCount=0;
    ui->labSendByte->setText("发送:0 字节");
    ReceiveCount=0;
    ui->labReceiveByte->setText("接收:0 字节");
}

void SerialPortTest::on_ckHexSend_stateChanged(int arg1)
{
        IsHexSend=(arg1==0?false:true);
}

void SerialPortTest::on_ckHexReceive_stateChanged(int arg1)
{
       IsHexReceive=(arg1==0?false:true);
}

void SerialPortTest::on_ckAutoSend_stateChanged(int arg1)
{
    bool IsAutoSend=(arg1==0?false:true);
    if (IsAutoSend)
    {
        this->mySendTimer->start();
    }
    else
    {
        this->mySendTimer->stop();
    }
    ui->spboxSendTime->setEnabled(IsAutoSend);
}

void SerialPortTest::on_ckAutoSave_stateChanged(int arg1)
{
    bool IsAutoSave=(arg1==0?false:true);
    if (IsAutoSave)
    {
        this->mySaveTimer->start();
    }
    else
    {
        this->mySaveTimer->stop();
    }
    ui->spboxSaveTime->setEnabled(IsAutoSave);
}

void SerialPortTest::on_cklsAutoClear_stateChanged(int arg1)
{
     IsAutoClear=(arg1==0?false:true);
}

void SerialPortTest::on_cklsDebug_stateChanged(int arg1)
{
    IsDebug=(arg1==0?false:true);
}

void SerialPortTest::on_btnOpenSerail_clicked()
{

    if(ui->btnOpenSerail->text()==tr("打开串口"))
    {
        serial = new QSerialPort(ui->cboxPortName->currentText());
        serial->close();

        serial->setPortName(ui->cboxPortName->currentText());//设置串口名
        //serial->open(QIODevice::ReadWrite);//打开串口
        if(serial->open(QIODevice::ReadWrite))
        {
            serial->setBaudRate(ui->cboxBaudRate->currentText().toInt(),QSerialPort::AllDirections);//设置波特率

            switch(ui->cboxDataBit->currentIndex())
            {
                case 0:serial->setDataBits(QSerialPort::Data5);break;
                case 1:serial->setDataBits(QSerialPort::Data6);break;
                case 2:serial->setDataBits(QSerialPort::Data7);break;
                case 3:serial->setDataBits(QSerialPort::Data8);break;
                default:break;
            }
            switch(ui->cboxParity->currentIndex())
            {
                case 0: serial->setParity(QSerialPort::NoParity);break;
                case 1: serial->setParity(QSerialPort::OddParity);break;
                case 2: serial->setParity(QSerialPort::EvenParity);break;
                case 3: serial->setParity(QSerialPort::MarkParity);break;
                default:break;
            }
            switch(ui->cboxStopBit->currentIndex())
            {
                case 0:serial->setStopBits(QSerialPort::OneStop);break;
                case 1:serial->setStopBits(QSerialPort::OneAndHalfStop);break;
                case 2:serial->setStopBits(QSerialPort::TwoStop);break;
                default:break;
            }
            serial->setFlowControl(QSerialPort::NoFlowControl);
            QObject::connect(serial,&QSerialPort::readyRead,this,&SerialPortTest::Read_ComData);
            this->myReadTimer->start();
            //QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::Read_ComData);
        }

        this->ChangeEnable(false);
        ui->btnOpenSerail->setText(tr("关闭串口"));
        ui->labSerialStatus->setText(tr("串口状态：打开"));
        ui->btnSendButton->setEnabled(true);

    }
    else
    {
        serial->clear();
        serial->close();
        serial->deleteLater();
        //恢复设置使能
        this->ChangeEnable(true);
        ui->btnOpenSerail->setText(tr("打开串口"));
        ui->labSerialStatus->setText(tr("串口状态：关闭"));
        ui->btnSendButton->setEnabled(false);
        this->myReadTimer->stop();
    }
}
void SerialPortTest::on_btnReturn_clicked()
{
    this->close();
    //mainInterface *maininterface=new mainInterface(this);
   // maininterface->show();
}

void SerialPortTest::on_spboxSendTime_valueChanged(int arg1)
{
     mySendTimer->setInterval(arg1);
}

void SerialPortTest::on_spboxSaveTime_valueChanged(int arg1)
{
    mySaveTimer->setInterval(arg1);
}

void SerialPortTest::SaveComData()
{
    QString tempData=ui->ReceiveText->toPlainText();
    if (tempData==""){return;}//如果没有内容则不保存

    QDateTime now=QDateTime::currentDateTime();
    QString name=now.toString("yyyyMMddHHmmss");
    QString fileName=Logoin::AppPath+"/localization/"+name+".txt";

    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out<<tempData;
    file.close();
}
void SerialPortTest::ReadComConfig()
{
    QString fileName=Logoin::AppPath+"/localization/SerialData.txt";
    QFile file(fileName);
    if (!file.exists()){return;}

    ui->txtSendData->clear();
    file.open(QFile::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString line;

    do{ line=in.readLine();if (line!="") {ui->txtSendData->addItem(line);}}
    while(!line.isNull());

    file.close();
}
void SerialPortTest::ReadSendData()
{
    QString fileName=Logoin::AppPath+"/localization/SerialData.txt";
    QFile file(fileName);
    if (!file.exists()){return;}//如果文件不存在则返回

    file.open(QFile::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString line;

    do { line=in.readLine();if (line!="") { SendDataList.append(line); }}
    while(!line.isNull());

    file.close();
}


#include <QThread>
#include <QSerialPort>
#ifdef QT_DEBUG
#include <QDebug>
#include <QDateTime>
#endif

MySerialPort::MySerialPort(const QString strComName)
    : m_strComName(strComName),
      m_pThread(new QThread()),
      m_pCom(new QSerialPort()),
      m_iLen(-1), m_bOpen(false),
      m_bEchoRegExp(false), m_bEchoFlag(false)
{
    m_pCom->moveToThread(m_pThread);
    this->moveToThread(m_pThread);
    m_pThread->start();
    connect(m_pCom, &QSerialPort::readyRead, this, &MySerialPort::slotDataReady, Qt::DirectConnection);

    connect(this, &MySerialPort::sigSetCOM, this, &MySerialPort::slotSetCOM);
    connect(this, &MySerialPort::sigClose, this, &MySerialPort::slotClose);
    connect(this, &MySerialPort::sigClear, this, &MySerialPort::slotClear);
    connect(this, &MySerialPort::sigWrite, this, &MySerialPort::slotWrite);
}
MySerialPort::~MySerialPort()
{
#ifdef QT_DEBUG
    qDebug() << "~MySerialPort:" << QThread::currentThreadId() << "\n";
#endif
    close();
    m_pThread->quit();
    m_pThread->wait();
    delete m_pCom;
    m_pCom = 0;
    delete m_pThread;
    m_pThread = 0;
}
bool MySerialPort::isOpen() const
{
    return m_bOpen;
}
bool MySerialPort::setCOM(const QString strCOM /*=""*/,
                          const int iBautRate /*=9600*/,
                          const int iDataBits /*=8*/,
                          const char chParity /*='N'*/,
                          const char chStopBits /*=1*/)
{
    m_lockSetCOM.lock();
    const int nAvlb = m_semSetCOM.available();
    if(nAvlb > 0)
    {
        m_semSetCOM.tryAcquire(nAvlb);
    }
    emit sigSetCOM(strCOM, iBautRate, iDataBits, chParity, chStopBits);
    const bool bWait = m_semSetCOM.tryAcquire(1, 5000);// bool bWait = m_waitSetCOM.wait(&m_lockSetCOM, 5000);
    m_lockSetCOM.unlock();
    return bWait ? m_bOpen : false;
}
qint64 MySerialPort::write(const QByteArray &byteArray)
{
    return write(byteArray.data(), byteArray.size());
}

qint64 MySerialPort::write(const char *data, qint64 maxSize/* = -1*/)
{
    if(!m_bOpen) return -1;
    m_lockWrite.lock();
    const int nAvlb = m_semWrite.available();
    if(nAvlb > 0)
    {
        m_semWrite.tryAcquire(nAvlb);
    }
    emit sigWrite(data, maxSize);
    const bool bWait = m_semWrite.tryAcquire(1, 5000);
    m_lockWrite.unlock();
    QMutexLocker lk(&m_lockWriteLen);
    qint64 iRet = bWait ? m_iLen : 0;
    return iRet;
}
void MySerialPort::close()
{
    m_lockClose.lock();
    const int nAvlb = m_semClose.available();
    if(nAvlb > 0)
    {
        m_semClose.tryAcquire(nAvlb);
    }
    emit sigClose();
    m_semClose.tryAcquire(1, 5000);
    m_lockClose.unlock();
}
void MySerialPort::clear()
{
    m_lockClear.lock();
    const int nAvlb = m_semClear.available();
    if(nAvlb > 0)
    {
        m_semClear.tryAcquire(nAvlb);
    }
    emit sigClear();
    m_semClear.tryAcquire(1, 5000);// m_waitClear.wait(&m_lockClear, 5000);
    m_lockClear.unlock();
}

bool MySerialPort::EchoCommand(const QString &strCmd, const QString &strExpectReply /* ="" */, QString *pstrReply /* =0 */)
{
    QMutexLocker lk(&m_lockEcho);
    m_bEchoRegExp = false;
    m_strExpectReply = strExpectReply;
    clear();
    setEchoFlag(true);
    const int nAvlb = m_semEcho.available();
    if(nAvlb > 0)
    {
        m_semEcho.tryAcquire(nAvlb);
    }
    const QByteArray ba = (strCmd + "\r\n").toLatin1();
    const int iLen = ba.length();
    if(write(ba) < iLen)
    {
        setEchoFlag(false);
        return false;
    }
    bool bRet = m_semEcho.tryAcquire(1, 10000);// bool bRet = m_waitEcho.wait(&m_lockEcho, 10000);
    //qDebug() << QDateTime::currentDateTime().toString("HH:mm:ss.zzz ") << "write " << strCmd << " OK\n";
    if("" == strExpectReply && 0 == pstrReply)
    {
        setEchoFlag(false);
        return true;
    }
    if(pstrReply)
    {
        pstrReply->clear();
    }
#ifdef QT_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("HH:mm:ss.zzz") << "EchoCommand wait " << bRet << " " << m_strReply << " |\n";
#endif
    setEchoFlag(false);
    if(false == bRet)
    {
        if(m_strReply.indexOf(strExpectReply) >= 0 && m_strReply.endsWith('\n'))
        {
            bRet = true;
        }
    }
    else if(m_strReply.indexOf("ES") >= 0 || m_strReply.indexOf("EL") >= 0)
    {
        bRet = false;
    }
    if(pstrReply)
    {
        *pstrReply = m_strReply;
    }

    return bRet;
}
//====================================================


void MySerialPort::slotWrite(const char *pch, qint64 maxSize)
{
    //qDebug() << QDateTime::currentDateTime().toString("HH:mm:ss.zzz") << " write begin\n";
    m_lockWriteLen.lock();
    m_iLen = -1;
    if(m_pCom->isOpen())
    {
        m_iLen = (maxSize < 0) ? m_pCom->write(pch) : m_pCom->write(pch, maxSize);
    }
    m_lockWriteLen.unlock();
    m_semWrite.release();
    //qDebug() << QDateTime::currentDateTime().toString("HH:mm:ss.zzz") << " write OK\n";
}

void MySerialPort::slotSetCOM(const QString strCOM, const int iBautRate, const int iDataBits, const char chParity, const char chStopBits)
{
    m_pCom->close();
    m_bOpen = false;
    if(strCOM.length() > 0)
    {
        m_strComName = strCOM;
    }
    if(m_strComName.length() < 1)
    {
        m_semSetCOM.release();// m_waitSetCOM.wakeAll();
        return;
    }
    m_pCom->setPortName(m_strComName);

    if(false == m_pCom->isOpen())
    {
        if(false == m_pCom->open(QIODevice::ReadWrite))
        {
            m_semSetCOM.release();// m_waitSetCOM.wakeAll();
        }
    }
    QSerialPort::BaudRate eBaudRate = QSerialPort::Baud9600;
    switch(iBautRate)
    {
    case 115200:
        eBaudRate = QSerialPort::Baud115200;
        break;
    case 9600:
        eBaudRate = QSerialPort::Baud9600;
        break;
    case 2400:
        eBaudRate = QSerialPort::Baud2400;
        break;
    case 1200:
        eBaudRate = QSerialPort::Baud1200;
        break;
    case 4800:
        eBaudRate = QSerialPort::Baud4800;
        break;
    case 19200:
        eBaudRate = QSerialPort::Baud19200;
        break;
    case 38400:
        eBaudRate = QSerialPort::Baud38400;
        break;
    case 57600:
        eBaudRate = QSerialPort::Baud57600;
        break;
    default:
        //eBaudRate = QSerialPort::UnknownBaud;
        break;
    }
    m_pCom->setBaudRate(eBaudRate);

    QSerialPort::DataBits eDataBits = QSerialPort::Data8;
    switch(iDataBits)
    {
    case 8:
        eDataBits = QSerialPort::Data8;
        break;
    case 7:
        eDataBits = QSerialPort::Data7;
        break;
    case 6:
        eDataBits = QSerialPort::Data6;
        break;
    case 5:
        eDataBits = QSerialPort::Data5;
        break;
    default:
        //eDataBits = QSerialPort::UnknownDataBits;
        break;
    }
    m_pCom->setDataBits(eDataBits);

    QSerialPort::Parity eParity = QSerialPort::NoParity;
    switch(chParity)
    {
    case 'N':
        eParity = QSerialPort::NoParity;
        break;
    case 'E':
        eParity = QSerialPort::EvenParity;
        break;
    case 'O':
        eParity = QSerialPort::OddParity;
        break;
    case 'S':
        eParity = QSerialPort::SpaceParity;
        break;
    case 'M':
        eParity = QSerialPort::MarkParity;
        break;
    default:
        //eParity = QSerialPort::UnknownParity;
        break;
    }
    m_pCom->setParity(eParity);

    QSerialPort::StopBits eStopBits = QSerialPort::OneStop;
    switch(chStopBits)
    {
    case 1:
        eStopBits = QSerialPort::OneStop;
        break;
    case 2:
        eStopBits = QSerialPort::TwoStop;
        break;
    case 3:
        eStopBits = QSerialPort::OneAndHalfStop;
        break;
    default:
        //eStopBits = QSerialPort::UnknownStopBits;
        break;
    }
    m_pCom->setStopBits(eStopBits);

    /*
        enum FlowControl {
            NoFlowControl,
            HardwareControl,
            SoftwareControl,
            UnknownFlowControl = -1
        }; */

    m_pCom->clear();
    m_bOpen = true;
    m_semSetCOM.release();
}
void MySerialPort::slotClose()
{
    //qDebug() << "slotClose:" << QThread::currentThreadId() <<"\n";
    m_pCom->close();
    m_bOpen = false;
    m_semClose.release();
}
void MySerialPort::slotClear()
{
    m_pCom->clear();
    m_lockInBuffer.lock();
    m_strInBuffer.clear();
    m_strReply.clear();
    m_lockInBuffer.unlock();
    m_semClear.release();
}
void MySerialPort::slotDataReady()
{
    QSerialPort *const pCom = m_pCom;
    if(getEchoFlag())
    {
        QMutexLocker lk(&m_lockInBuffer);
        m_strInBuffer.push_back(pCom->readAll());
        m_strReply = m_strInBuffer;
        if(m_bEchoRegExp)
        {
            if(m_strInBuffer.indexOf(m_rxExpectReply) >= 0)
            {
                setEchoFlag(false);
                m_semEcho.release();
            }
        }
        else
        {
            int idx = m_strInBuffer.indexOf(m_strExpectReply);
            if(idx >= 0 && m_strInBuffer.indexOf('\n', idx + 1) >= 0)
            {
                setEchoFlag(false);
#ifdef QT_DEBUG
                qDebug() << QDateTime::currentDateTime().toString("HH:mm:ss.zzz ") << m_strReply << " wakeAll\n";
#endif
                m_semEcho.release();
                return;
            }
        }
    }
    else
    {
        emit sigDataReady(pCom->readAll());
    }
}

#if 0
bool MySerialPort::EchoCommand(const QString &strCmd, const QRegExp &rx /* ="" */, QString *pstrReply /* =0 */)
{
    m_strReply = "";
    m_bEchoRegExp = true;
    m_rxExpectReply = rx;
    m_lockInBuffer.lock();
    m_strInBuffer.clear();
    m_lockInBuffer.unlock();
    m_bEchoFlag = true;
    if(write((strCmd + "\r\n").toLatin1()) < strCmd.length() + 2)
    {
        return false;
    }

    QMutexLocker lk(&m_lockEcho); //.lock();
    bool bRet = (m_waitEcho.wait(&m_lockEcho, 5000));
    m_bEchoFlag = false;
    m_bEchoRegExp = false;
    if(pstrReply)
    {
        *pstrReply = bRet ? m_strReply : "";
    }

    return bRet;
}
#endif
