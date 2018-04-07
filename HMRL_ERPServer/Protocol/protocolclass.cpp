#include "protocolclass.h"
#include "verifychecksum.h"


#include <QDebug>
#include <QDateTime>
unsigned char ProtocolClass::HoldRegister[256]={0};
unsigned char ProtocolClass::CoilStatus[64]={0};
QHash<QString,QStringList>ProtocolClass::MonitorPiontInfo;//监控点位信息
QHash<QString,QString>ProtocolClass::FactorCodeToName;
ProtocolClass::ProtocolClass(QObject *parent):QObject(parent)
{

}

QString ProtocolClass::HBGBProtocolServer(QByteArray GuoBiao_Buffer)
{
    QString QnRtn_Flag,ExeRtn_Flag,DataHead,DataStr;//MN_Code,
    QString MN_Code,returnValue;//MN_Code,,QN_Code,startCode
    QStringList FactorList;
    short StringPosition_01,StringPosition_02;
    unsigned short CN_Code,ST_Code;//DataLen2,

    //startCode=GuoBiao_Buffer.mid(0,2);

    //if (startCode=="##")
    if ((GuoBiao_Buffer.at(0)==0x23) and (GuoBiao_Buffer.at(1)==0x23))
    {
        //DataLen2 = GuoBiao_Buffer.mid(2,4).toShort();//数据长度
//        StringPosition_01 = GuoBiao_Buffer.indexOf("QN=");   //查找“QN=”
//        if(StringPosition_01 != -1)
//        {
//            StringPosition_02 = GuoBiao_Buffer.indexOf(";",StringPosition_01);
//            QN_Code=GuoBiao_Buffer.mid(StringPosition_01,StringPosition_02-StringPosition_01);
//        }
//        StringPosition_01=GuoBiao_Buffer.indexOf("PW=");   //查找“PW=”
//        if(StringPosition_01!=-1)
//        {
//            StringPosition_02 = GuoBiao_Buffer.indexOf(";",StringPosition_01);
//            PW_Code=GuoBiao_Buffer.mid(StringPosition_01+3,StringPosition_02-StringPosition_01-3);
//        }
        //StringPosition_01=GuoBiao_Buffer.indexOf("ST=");   //查找“ST=”
        //if(StringPosition_01!=-1)
        //{
            //StringPosition_02 = GuoBiao_Buffer.indexOf(";",StringPosition_01);
            ST_Code=GuoBiao_Buffer.mid(30,2).toUShort();
        //}
        StringPosition_01=GuoBiao_Buffer.indexOf("CN=");   //查找“CN=”
        if(StringPosition_01!=-1)
        {
            StringPosition_02 = GuoBiao_Buffer.indexOf(";",StringPosition_01);
            CN_Code=GuoBiao_Buffer.mid(StringPosition_01+3,4).toUShort();
        }

        StringPosition_01=GuoBiao_Buffer.indexOf("MN=");   //查找“MN=”
        if(StringPosition_01!=-1)
        {
            StringPosition_02 = GuoBiao_Buffer.indexOf(";",StringPosition_01);
            MN_Code=GuoBiao_Buffer.mid(StringPosition_01+3,StringPosition_02-StringPosition_01-3);
        }
        FactorList=MonitorPiontInfo.value(MN_Code);
        if(ST_Code==91)
        {
            switch(CN_Code)
            {
                case 9011:
                {
                    StringPosition_01 = GuoBiao_Buffer.indexOf("QnRtn=");   //查找“QnRtn=” 上位机接收请求应答，根据请求应答标志QnRtn的值决定是否等待执行结果
                    if(StringPosition_01!=-1)
                    {
                        StringPosition_02 = GuoBiao_Buffer.indexOf("&",StringPosition_01);
                        QnRtn_Flag=GuoBiao_Buffer.mid(StringPosition_01+6,StringPosition_02-StringPosition_01-6);

                    }
                    returnValue = tr("QnRtn=%1").arg(QnRtn_Flag);
                    break;
                }
                case 9012:
                {
                    StringPosition_01 = GuoBiao_Buffer.indexOf("ExeRtn=");   //查找“ExeRtn=” 上位机执行结果，根据执行结果标志ExeRtn的值检查请求是否完成，
                    if(StringPosition_01!=-1)
                    {
                        StringPosition_02 = GuoBiao_Buffer.indexOf("&",StringPosition_01);
                        ExeRtn_Flag=GuoBiao_Buffer.mid(StringPosition_01+7,StringPosition_02-StringPosition_01-7);

                    }
                    returnValue = tr("ExeRtn_Flag=%1").arg(ExeRtn_Flag);
                    break;
                }
                case 9013:
                case 9021:
                {
                    returnValue = "";
                    break;
                }
            }
            return "";
        }
        else
        {
            QString DataTime_Code;
            StringPosition_01=GuoBiao_Buffer.indexOf("DataTime=");
            StringPosition_02 = GuoBiao_Buffer.indexOf(";",StringPosition_01);
            DataTime_Code=GuoBiao_Buffer.mid(StringPosition_01+9,StringPosition_02-StringPosition_01-9);

            StringPosition_01 = GuoBiao_Buffer.indexOf("&",StringPosition_02);
            QString GuoBiaoStr;
            GuoBiaoStr=GuoBiao_Buffer.mid(StringPosition_02+1,StringPosition_01-StringPosition_02-1);
            GuoBiaoStr.replace(QRegExp(";"), ",");
            GuoBiaoStr.append(",");
            qDebug()<<GuoBiaoStr;
            switch (CN_Code)
            {
            case 2011:         //开始察看实时数据
            {
                QStringList DataViewHead,DataValue;
                QString tmpValue;
                for(int i=0;i<FactorList.count();i++)
                {

                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-Rtd=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {

                        StringPosition_02 = GuoBiaoStr.indexOf(",",StringPosition_01);
                        tmpValue=GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+5,StringPosition_02-(StringPosition_01+FactorList.at(i).length()+5));
                        DataStr+=tmpValue+",";
                        DataHead+=FactorList.at(i)+"Rtd,";
                        //写入所有数据一览表
                        DataViewHead<<FactorCodeToName.value(FactorList.at(i));
                        DataValue<<tmpValue;

                    }

                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-Flag=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {
                        //StringPosition_02
                        DataStr+="'"+GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+6,1)+"',";
                        DataHead+=FactorList.at(i)+"Flag,";

                    }
                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-SampleTime=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {
                        //DataStr+=GuoBiao_Buffer.mid(StringPosition_01+FactorList.at(i).length()+5,14);
                    }
                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-EFlag=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {
                        //DataStr+=GuoBiao_Buffer.mid(StringPosition_01+FactorList.at(i).length()+5,3);
                    }
                 }
                //QSqlQuery query;
                //QString InsertData;
                returnValue="replace into `"+MN_Code+"`.rtddatabase(DataTime,"+DataHead.remove(DataHead.length()-1,1)+")values("+DataTime_Code+","+DataStr.remove(DataStr.length()-1,1)+");replace into `onlinedatabase`.dataoverview(MN_Code,DataTime,"+DataViewHead.join(",")+")values('"+MN_Code+"',"+DataTime_Code+","+DataValue.join(",")+");";
                //query.exec(InsertData);
                //qDebug()<<InsertData;
                return returnValue;
                break;
            }
            case 2031:         //取污染物日历史数据
            case 2051:          //取污染物分钟数据
            case 2061:         //取污染物小时数据
            {

                for(int i=0;i<FactorList.count();i++)
                {
                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-Avg=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {
                        StringPosition_02 = GuoBiaoStr.indexOf(",",StringPosition_01);
                        DataStr+=GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+5,StringPosition_02-(StringPosition_01+FactorList.at(i).length()+5))+",";
                        DataHead+=FactorList.at(i)+"Avg,";
                    }

                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-Min=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {
                        StringPosition_02 = GuoBiaoStr.indexOf(",",StringPosition_01);
                        DataStr+=GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+5,StringPosition_02-(StringPosition_01+FactorList.at(i).length()+5))+",";
                        DataHead+=FactorList.at(i)+"Min,";
                    }
                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-Max=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {

                        StringPosition_02 = GuoBiaoStr.indexOf(",",StringPosition_01);
                        DataStr+=GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+5,StringPosition_02-(StringPosition_01+FactorList.at(i).length()+5))+",";
                        DataHead+=FactorList.at(i)+"Max,";
                    }
                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-Cou=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {

                        StringPosition_02 = GuoBiaoStr.indexOf(",",StringPosition_01);
                        DataStr+=GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+5,StringPosition_02-(StringPosition_01+FactorList.at(i).length()+5))+",";
                        DataHead+=FactorList.at(i)+"Cou,";
                    }
                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-Flag=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {
                        //StringPosition_02
                        DataStr+="'"+GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+6,1)+"',";
                        DataHead+=FactorList.at(i)+"Flag,";
                    }
                 }
                //QSqlQuery query;
                //QString InsertData;
                if(CN_Code==2051)
                {
                    returnValue="replace into `"+MN_Code+"`.Mindatabase(DataTime,"+DataHead.remove(DataHead.length()-1,1)+")values("+DataTime_Code+","+DataStr.remove(DataStr.length()-1,1)+");";
                }
                else if(CN_Code==2061)
                {
                    returnValue="replace into `"+MN_Code+"`.hourdatabase(DataTime,"+DataHead.remove(DataHead.length()-1,1)+")values("+DataTime_Code+","+DataStr.remove(DataStr.length()-1,1)+");";
                }
                else if(CN_Code==2031)
                {
                    returnValue="replace into `"+MN_Code+"`.daydatabase(DataTime,"+DataHead.remove(DataHead.length()-1,1)+")values("+DataTime_Code+","+DataStr.remove(DataStr.length()-1,1)+");";
                }

                //query.exec(InsertData);
                //qDebug()<<InsertData;
                return returnValue;
                break;
            }
            case 1011:        //提取现场机时间,数据区中如有因子编码，则提取对应设备时间，如不含则提取数采仪时间
            {
                QString PolIDStr,SystemTimeStr;
                StringPosition_01=GuoBiao_Buffer.indexOf("PolId=");
                if(StringPosition_01!=-1)
                {
                    //StringPosition_02 = GuoBiao_Buffer.indexOf(";",StringPosition_01);
                    PolIDStr=GuoBiao_Buffer.mid(StringPosition_01+6,6);
                }
                else
                {
                    PolIDStr="dtu000";
                }
                StringPosition_01=GuoBiao_Buffer.indexOf("SystemTime=");
                //StringPosition_02 = GuoBiao_Buffer.indexOf("&",StringPosition_01);
                SystemTimeStr=GuoBiao_Buffer.mid(StringPosition_01+6,14);
                //QSqlQuery query;
                //QString InsertData;
                returnValue="INSERT INTO `"+MN_Code+"`.polinfodatabase(DataTime,PolID,InfoId,Pol_Info) values("+DataTime_Code.mid(0,14)+",'"+PolIDStr+"','"+CN_Code+"','"+SystemTimeStr+"');";

                //query.exec(InsertData);
                //qDebug()<<InsertData;
                return returnValue;
                break;
            }

            case 2041:            //取设备运行时间日历史数据
            {
                for(int i=0;i<FactorList.count();i++)
                {
                    StringPosition_01=GuoBiaoStr.indexOf(FactorList.at(i)+"-RT=");   //查找FactorList.at(i)
                    if(StringPosition_01!=-1)
                    {
                        StringPosition_02 = GuoBiaoStr.indexOf(",",StringPosition_01);
                        DataStr+=GuoBiaoStr.mid(StringPosition_01+FactorList.at(i).length()+4,StringPosition_02-(StringPosition_01+FactorList.at(i).length()+4))+",";
                        DataHead+=FactorList.at(i)+"RT,";
                    }
                 }
                //QSqlQuery query;
                //QString InsertData;
                returnValue="replace into `"+MN_Code+"`.RTdatabase(DataTime,"+DataHead.remove(DataHead.length()-1,1)+")values("+DataTime_Code+","+DataStr.remove(DataStr.length()-1,1)+");";
                //query.exec(InsertData);
                //qDebug()<<InsertData;
                return returnValue;
                break;
            }

            case 2081:            //上传现场设备开关机时间
            {

                StringPosition_01=GuoBiaoStr.indexOf("RestartTime=");   //查找FactorList.at(i)
                if(StringPosition_01!=-1)
                {
                    StringPosition_02 = GuoBiaoStr.indexOf(",",StringPosition_01);
                    DataStr=GuoBiaoStr.mid(StringPosition_01+12,StringPosition_02-(StringPosition_01+12));
                    //DataHead+="RestartTime";
                 }


                //QSqlQuery query;
                //QString InsertData;
                returnValue="replace into `"+MN_Code+"`.RTUStartdatabase(CN_Code,DataTime)values('2081',"+DataStr+");";

                //query.exec(InsertData);
                //qDebug()<<InsertData;
                return returnValue;
                break;
            }


            case 3020://提取现场机参数及相关信息
            {
                QString PolID,InfoID,InfoValue;
                StringPosition_01=GuoBiaoStr.indexOf("PolId=");   //查找FactorList.at(i)
                if(StringPosition_01!=-1)
                {
                    PolID=GuoBiaoStr.mid(StringPosition_01+6,6);
                 }
                StringPosition_01=GuoBiaoStr.indexOf("-Info=");   //查找FactorList.at(i)
                if(StringPosition_01!=-1)
                {
                    InfoID=GuoBiaoStr.mid(StringPosition_01-6,6);
                    InfoValue=GuoBiaoStr.mid(StringPosition_01+6,GuoBiaoStr.length()-StringPosition_01-7);
                 }
                InfoValue.remove(QRegExp("[\(\)]"));

                //QSqlQuery query;
                //QString InsertData;
                returnValue="INSERT INTO `"+MN_Code+"`.polinfodatabase(DataTime,PolID,InfoId,Pol_Info) values("+DataTime_Code.mid(0,14)+",'"+PolID+"','"+InfoID+"','"+InfoValue+"');";

                //query.exec(InsertData);
                //qDebug()<<InsertData;
                return returnValue;
                break;
            }


            //case 3022:               //提取现场设备参数
            //case 3023:                //提取数采仪系统日志
            //case 3024:               //上传现场仪器状态
            case 4050:            //读取因子实时数据--类型---开始时间----结束时间
            {
                QString PolID,PolID_Rtd,PolID_DT,PolID_StartTime,PolID_EndTime;
                StringPosition_01=GuoBiaoStr.indexOf("-Rtd=");   //查找FactorList.at(i)
                if(StringPosition_01!=-1)
                {
                    PolID=GuoBiaoStr.mid(StringPosition_01-6,6);
                    PolID_Rtd=GuoBiaoStr.mid(StringPosition_01+5,6);
                 }
                StringPosition_01=GuoBiaoStr.indexOf("-DT=");   //查找FactorList.at(i)
                if(StringPosition_01!=-1)
                {
                    PolID_DT=GuoBiaoStr.mid(StringPosition_01+4,1);
                 }
                StringPosition_01=GuoBiaoStr.indexOf("-SampleTime=");   //查找FactorList.at(i)
                if(StringPosition_01!=-1)
                {
                    StringPosition_02=GuoBiaoStr.indexOf(",",StringPosition_01);   //查找FactorList.at(i)
                    PolID_StartTime=GuoBiaoStr.mid(StringPosition_01+12,StringPosition_02-StringPosition_01-12);
                 }
                StringPosition_01=GuoBiaoStr.indexOf("-EndTime=");   //查找FactorList.at(i)
                if(StringPosition_01!=-1)
                {
                    StringPosition_02=GuoBiaoStr.indexOf(",",StringPosition_01);   //查找FactorList.at(i)
                    PolID_EndTime=GuoBiaoStr.mid(StringPosition_01+9,StringPosition_02-StringPosition_01-9);
                 }

                //QSqlQuery query;
                //QString InsertData;
                returnValue="replace INTO `"+MN_Code+"`.MeasureInfo(DataTime,PolID,PolID_Rtd,PolID_DT,PolID_StartTime,PolID_EndTime) values("+DataTime_Code.mid(0,14)+",'"+PolID+"','"+PolID_Rtd+"','"+PolID_DT+"','"+PolID_StartTime+"','"+PolID_EndTime+"');";

                //query.exec(InsertData);
                qDebug()<<tr("4050指令输出%1").arg(returnValue);
                return returnValue;
                break;
            }
    //        case 1061:break;         //提取实时数据间隔
    //        case 1063:break;         //提取分钟数据上报间隔

            case 2021:break;         //开始察看设施运行状态数据
    //        case 3015:break;            //控制采样器进行超标留样

    //        case 3017:break;             //提取采样时间周期
    //        case 3018:break;            //提取出样时间
    //        case 3019:break;            //提取设备唯一标识
    //        case 3021:break;              //设置现场机参数
    //        case 9011:break;               //请求应答
    //        case 9012:break;               //执行结果
    //        case 9013:break;               //回应通知命令
    //        case 9014:break;               //数据应答命令
              deafult:return "";

            }
        }

    }

    return QString(tr("无需解析(%1-%2-%3-%4-%5)").arg(QString(GuoBiao_Buffer.at(0))).arg(GuoBiao_Buffer.at(1)).arg(QString(GuoBiao_Buffer.at(2))).arg(QString(GuoBiao_Buffer.at(3))).arg(QString(GuoBiao_Buffer)));
    //return "";
}





QByteArray ProtocolClass::HBGBDataCollection(QString CN_Code,QString ST_Code,QString MN_Password,QString MN_Searil,QString CP_Code)
{
    quint16 strCount;
    QByteArray strByteArray;
    QString str;
    str="QN="+QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz")+";ST="+ST_Code+";"+CN_Code+"PW="+MN_Password+";MN="+MN_Searil+";Flag=3;"+CP_Code;
    //str="QN=20161117001323782;ST=32;CN=2011;PW=123;MN=40142001001108;Flag=3;CP=&&BeginTime=20161115000000,EndTime=20161117235959&&";
    //str="QN=20170928125703829;ST=32;CN=2031;PW=123456;MN=40142000135246;Flag=3;CP=&&BeginTime=20170924000000,EndTime=20170928000000&&";
    strCount=str.count();

    str="##"+QString::number(strCount,10).rightJustified(4,'0')+str+QString::number(VerifyChecksum::CRC16(str.toLatin1()),16).rightJustified(4,'0').toUpper();
    strByteArray.clear();
    strByteArray.append(str.toLatin1());
    strByteArray.append(0x0d);//'\r'   回车
    strByteArray.append(0x0a);//'\n'   换行

    return strByteArray;
}
QStringList ProtocolClass::FlowMeterProtocol(QByteArray buffer)
{
    QStringList value;
    value.clear();
    charToFloat ucf;
    if(VerifyChecksum::crc16ForModbus(buffer)==0)
    {
        for(int i=0;i<7;i++)
        {

            ucf.sChar[1]=buffer.at(i*4+3);
            ucf.sChar[0]=buffer.at(i*4+4);
            ucf.sChar[3]=buffer.at(i*4+5);
            ucf.sChar[2]=buffer.at(i*4+6);
            value<<QString().setNum(ucf.sFloat,'f',4);

        }
    }
    else
    {
        for(int i=0;i<7;i++)
        {
            value<<"数据校验错误";
        }

    }
    return value;
}

QStringList ProtocolClass::AnalyzerProtocol_STEP(QByteArray buffer)
{

    QStringList value;
    value.clear();
    //charToFloat ucf;
    quint16 RegisterAddress,RegisterCount;
    //qDebug()<<"buffer"<<buffer.count();
    if(VerifyChecksum::crc16ForModbus(buffer)==0)
    {
        RegisterAddress=(quint16(buffer.at(2)<<8)|buffer.at(3))*2;

        switch(buffer.at(1))
        {
            case 0x03:
            {
                RegisterCount=quint16(buffer.at(4)<<8)|buffer.at(5);
                for(int i=0;i<RegisterCount;i++)
                {
                    HoldRegister[RegisterAddress+i]=buffer.at(i+6);
                    //HoldRegister[RegisterAddress+i]=buffer.at(i+6);
                }
                for(int i=0;i<256;i++)
                {
                    value<<tr("%1=%2").arg(i).arg(HoldRegister[i]);//<<QString::number(HoldRegister[i]);
                }

                break;
            }
            case 0x06:
            {
                RegisterCount=buffer.at(4);
                for(int i=0;i<RegisterCount;i++)
                {

                    HoldRegister[RegisterAddress+i]=buffer.at(i+5);
                }
                for(int i=0;i<256;i++)
                {
                    value<<tr("%1=%2").arg(i).arg(HoldRegister[i]);//value<<QString::number(HoldRegister[i]);
                }

                break;
            }

            case 0x05:
            {

                CoilStatus[RegisterAddress]=buffer.at(4);
                for(int i=0;i<64;i++)
                {
                    //value<<QString::setNum(CoilStatus[i]);
                }

                break;
            }
        }
        qDebug()<<value;
        //value<<"接收到正确的数据";
        //emit ReadFinish();
    }
    else
    {

        value<<"数据校验错误";


    }
    return value;
}

