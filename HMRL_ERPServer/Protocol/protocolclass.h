#ifndef PROTOCOLCLASS_H
#define PROTOCOLCLASS_H

#include "protocol/verifychecksum.h"
#include <QByteArray>
//#include <QWidget>
#include <QStringList>
#include <QObject>
/*******************************************************
 * 2017.02.06
 * 在qt添加类的时候如果没有继承QOBJECT，使用Q_OBJECT宏，在后续的编程中当希望使用信号与槽的时候，添加了Q_OBJECT宏，
 * 在编译的时候不小心会出现如下错误,undefined reference to 'vtable for ***(类）'.
 * 解决方法1、删除debug目录中的makefile文件，在从新编译。
 * 解决方法2、修改pro文件，以达到修给makefile文件的目的（添加删除，空行空格都可以，只要修改pro文件就行.重新编译。
**************************************************************************/

class ProtocolClass : public QObject
{
    Q_OBJECT

public:
    ProtocolClass(QObject *parent = 0);

    static QString HBGBProtocolServer(QByteArray GuoBiao_Buffer);
    static QByteArray HBGBDataCollection(QString CN_Code,QString ST_Code,QString MN_Password,QString MN_Searil,QString CP_Code);
    static QStringList FlowMeterProtocol(QByteArray buffer);
    static QStringList AnalyzerProtocol_STEP(QByteArray buffer);


    static QHash<QString,QStringList>MonitorPiontInfo;//监控点位信息
    static QHash<QString,QString>FactorCodeToName;

    static unsigned char HoldRegister[256];
    static unsigned char CoilStatus[64];
signals:
    void ReadFinish();
};

union charToFloat
{
    unsigned char sChar[4];
    short int sInt[2];
    float sFloat;
};


#endif // PROTOCOLCLASS_H
