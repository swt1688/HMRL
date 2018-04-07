#ifndef HELPERCLASS_H
#define HELPERCLASS_H

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QTime>
#include <QStyleFactory>
#include <QInputDialog>
#include <QSound>
#include <QProcess>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>

/*描述：辅助类，包含对话框，中文显示，文件处理等
 *作者：史文涛
 *时间：2016-10-02
*/
class HelperClass:public QObject
{
public:

    //设置全局为plastique样式
    //    QFile qss(":/QSS/ui_stytle.qss");
    //    qss.open(QFile::ReadOnly);
    //    //mainInterface w;

    //    w.setStyleSheet(qss.readAll());
    //    qss.close();
    static void SetStyle(const QString &StyleName)
    {
        QFile file(QString(":/QSS/%1.qss").arg(StyleName));
        file.open(QFile::ReadOnly);
        //QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(file.readAll());
        file.close();
        //qApp->setPalette(QPalette(QColor("#F0F0F0")));
    }
//    static void SetStyle()
//    {
//        QApplication::setStyle(QStyleFactory::create("Plastique"));
//        QApplication::setPalette(QApplication::style()->standardPalette());//替换应用程序的颜色组合
//    }



    //显示信息框，仅确定按钮
    static void ShowMessageBoxInfo(QString info,QWidget *parent)
    {
        QMessageBox msg(parent);
        msg.setStyleSheet("font:12pt '宋体'");
        msg.setWindowTitle("提示");
        msg.setText(info);
        msg.setIcon(QMessageBox::Information);
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.exec();
    }

    static void ShowMessageBoxInfoX(QString info)
    {
        QMessageBox::information(0,"提示",info,QMessageBox::Ok);
    }

    //显示错误框，仅确定按钮
    static void ShowMessageBoxError(QString info)
    {
        QMessageBox msg;
        msg.setStyleSheet("font:12pt '宋体'");
        msg.setWindowTitle("错误");
        msg.setText(info);
        msg.setIcon(QMessageBox::Critical);
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.exec();
    }

    static void ShowMessageBoxErrorX(QString info)
    {
        QMessageBox::critical(0,"错误",info,QMessageBox::Ok);
    }

    //显示询问框，确定和取消按钮
    static int ShowMessageBoxQuesion(QString info,QWidget *parent)
    {
        QMessageBox msg(parent);
        msg.setStyleSheet("font:14pt '宋体'");
        msg.setWindowTitle("询问");
        msg.setText(info);
        msg.setIcon(QMessageBox::Question);
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.addButton("取消",QMessageBox::RejectRole);

        return msg.exec();
    }

    static int ShowMessageBoxQuesionX(QString info)
    {
        return QMessageBox::question(0,"询问",info,QMessageBox::Yes|QMessageBox::No);
    }

    //显示标准输入框
    static QString ShowInputBox(QWidget *frm,QString info)
    {
        bool ok;
        return QInputDialog::getText(frm,"提示",info,QLineEdit::Password,"",&ok);
    }
 //10进制数转16进制字符
    static QString DecimalToStrHex(int decimal)
    {
        QString temp = QString::number(decimal, 16);

        if (temp.length() == 1) {
            temp = "0" + temp;
        }

        return temp.toUpper();
    }
    //16进制字符串转字节数组
    static QByteArray HexStrToByteArray(QString str)
    {
        QByteArray senddata;
        int hexdata,lowhexdata;
        int hexdatalen = 0;
        int len = str.length();
        senddata.resize(len/2);
        char lstr,hstr;
        for(int i=0; i<len; )
        {
            hstr=str[i].toLatin1();
            if(hstr == ' ')
            {
                i++;
                continue;
            }
            i++;
            if(i >= len)
                break;
            lstr = str[i].toLatin1();
            hexdata = ConvertHexChar(hstr);
            lowhexdata = ConvertHexChar(lstr);
            if((hexdata == 16) || (lowhexdata == 16))
                break;
            else
                hexdata = hexdata*16+lowhexdata;
            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }
        senddata.resize(hexdatalen);
        return senddata;
    }

    static char ConvertHexChar(char ch)
    {
        if((ch >= '0') && (ch <= '9'))
            return ch-0x30;
        else if((ch >= 'A') && (ch <= 'F'))
            return ch-'A'+10;
        else if((ch >= 'a') && (ch <= 'f'))
            return ch-'a'+10;
        else return (-1);
    }

    //字节数组转16进制字符串
    static QString ByteArrayToHexStr(QByteArray data)
    {
        QString temp="";
        QString hex=data.toHex();
        for (int i=0;i<hex.length();i=i+2)
        {
            temp+=hex.mid(i,2)+" ";
        }
        return temp.trimmed().toUpper();
    }

    static QString ByteArrayToAsciiStr(QByteArray data)
    {
        QString temp;
        int len = data.size();

        for (int i = 0; i < len; i++) {
            //0x20为空格,空格以下都是不可见字符
            char b = data.at(i);

            if (0x00 == b) {
                temp += QString("\\NUL");
            } else if (0x01 == b) {
                temp += QString("\\SOH");
            } else if (0x02 == b) {
                temp += QString("\\STX");
            } else if (0x03 == b) {
                temp += QString("\\ETX");
            } else if (0x04 == b) {
                temp += QString("\\EOT");
            } else if (0x05 == b) {
                temp += QString("\\ENQ");
            } else if (0x06 == b) {
                temp += QString("\\ACK");
            } else if (0x07 == b) {
                temp += QString("\\BEL");
            } else if (0x08 == b) {
                temp += QString("\\BS");
            } else if (0x09 == b) {
                temp += QString("\\HT");
            } else if (0x0A == b) {
                temp += QString("\\LF");
            } else if (0x0B == b) {
                temp += QString("\\VT");
            } else if (0x0C == b) {
                temp += QString("\\FF");
            } else if (0x0D == b) {
                temp += QString("\\CR");
            } else if (0x0E == b) {
                temp += QString("\\SO");
            } else if (0x0F == b) {
                temp += QString("\\SI");
            } else if (0x10 == b) {
                temp += QString("\\DLE");
            } else if (0x11 == b) {
                temp += QString("\\DC1");
            } else if (0x12 == b) {
                temp += QString("\\DC2");
            } else if (0x13 == b) {
                temp += QString("\\DC3");
            } else if (0x14 == b) {
                temp += QString("\\DC4");
            } else if (0x15 == b) {
                temp += QString("\\NAK");
            } else if (0x16 == b) {
                temp += QString("\\SYN");
            } else if (0x17 == b) {
                temp += QString("\\ETB");
            } else if (0x18 == b) {
                temp += QString("\\CAN");
            } else if (0x19 == b) {
                temp += QString("\\EM");
            } else if (0x1A == b) {
                temp += QString("\\SUB");
            } else if (0x1B == b) {
                temp += QString("\\ESC");
            } else if (0x1C == b) {
                temp += QString("\\FS");
            } else if (0x1D == b) {
                temp += QString("\\GS");
            } else if (0x1E == b) {
                temp += QString("\\RS");
            } else if (0x1F == b) {
                temp += QString("\\US");
            } else if (0x7F == b) {
                temp += QString("\\x7F");
            } else if (0x5C == b) {
                temp += QString("\\x5C");
            } else if (0x20 >= b) {
                temp += QString("\\x%1").arg(DecimalToStrHex((quint8)b));
            } else {
                temp += QString("%1").arg(b);
            }
        }

        return temp.trimmed();
    }
    static QByteArray asciiStrToByteArray(QString str)
    {
        QByteArray buffer;
        int len = str.length();
        QString letter;
        QString hex;

        for (int i = 0; i < len; i++) {
            letter = str.at(i);

            if (letter == "\\") {
                i++;
                letter = str.mid(i, 1);

                if (letter == "x") {
                    i++;
                    hex = str.mid(i, 2);
                    buffer.append(StrHexToDecimal(hex));
                    i++;
                    continue;
                } else if (letter == "N") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "U") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "L") {           //NUL=0x00
                            buffer.append((char)0x00);
                            continue;
                        }
                    } else if (hex == "A") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "K") {           //NAK=0x15
                            buffer.append(0x15);
                            continue;
                        }
                    }
                } else if (letter == "S") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "O") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "H") {           //SOH=0x01
                            buffer.append(0x01);
                            continue;
                        } else {                    //SO=0x0E
                            buffer.append(0x0E);
                            i--;
                            continue;
                        }
                    } else if (hex == "T") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "X") {           //STX=0x02
                            buffer.append(0x02);
                            continue;
                        }
                    } else if (hex == "I") {        //SI=0x0F
                        buffer.append(0x0F);
                        continue;
                    } else if (hex == "Y") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "N") {           //SYN=0x16
                            buffer.append(0x16);
                            continue;
                        }
                    } else if (hex == "U") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "B") {           //SUB=0x1A
                            buffer.append(0x1A);
                            continue;
                        }
                    }
                } else if (letter == "E") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "T") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "X") {           //ETX=0x03
                            buffer.append(0x03);
                            continue;
                        } else if (hex == "B") {    //ETB=0x17
                            buffer.append(0x17);
                            continue;
                        }
                    } else if (hex == "O") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "T") {           //EOT=0x04
                            buffer.append(0x04);
                            continue;
                        }
                    } else if (hex == "N") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "Q") {           //ENQ=0x05
                            buffer.append(0x05);
                            continue;
                        }
                    } else if (hex == "M") {        //EM=0x19
                        buffer.append(0x19);
                        continue;
                    } else if (hex == "S") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "C") {           //ESC=0x1B
                            buffer.append(0x1B);
                            continue;
                        }
                    }
                } else if (letter == "A") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "C") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "K") {           //ACK=0x06
                            buffer.append(0x06);
                            continue;
                        }
                    }
                } else if (letter == "B") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "E") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "L") {           //BEL=0x07
                            buffer.append(0x07);
                            continue;
                        }
                    } else if (hex == "S") {        //BS=0x08
                        buffer.append(0x08);
                        continue;
                    }
                } else if (letter == "C") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "R") {               //CR=0x0D
                        buffer.append(0x0D);
                        continue;
                    } else if (hex == "A") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "N") {           //CAN=0x18
                            buffer.append(0x18);
                            continue;
                        }
                    }
                } else if (letter == "D") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "L") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "E") {           //DLE=0x10
                            buffer.append(0x10);
                            continue;
                        }
                    } else if (hex == "C") {
                        i++;
                        hex = str.mid(i, 1);

                        if (hex == "1") {           //DC1=0x11
                            buffer.append(0x11);
                            continue;
                        } else if (hex == "2") {    //DC2=0x12
                            buffer.append(0x12);
                            continue;
                        } else if (hex == "3") {    //DC3=0x13
                            buffer.append(0x13);
                            continue;
                        } else if (hex == "4") {    //DC2=0x14
                            buffer.append(0x14);
                            continue;
                        }
                    }
                } else if (letter == "F") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "F") {               //FF=0x0C
                        buffer.append(0x0C);
                        continue;
                    } else if (hex == "S") {        //FS=0x1C
                        buffer.append(0x1C);
                        continue;
                    }
                } else if (letter == "H") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "T") {               //HT=0x09
                        buffer.append(0x09);
                        continue;
                    }
                } else if (letter == "L") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "F") {               //LF=0x0A
                        buffer.append(0x0A);
                        continue;
                    }
                } else if (letter == "G") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "S") {               //GS=0x1D
                        buffer.append(0x1D);
                        continue;
                    }
                } else if (letter == "R") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "S") {               //RS=0x1E
                        buffer.append(0x1E);
                        continue;
                    }
                } else if (letter == "U") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "S") {               //US=0x1F
                        buffer.append(0x1F);
                        continue;
                    }
                } else if (letter == "V") {
                    i++;
                    hex = str.mid(i, 1);

                    if (hex == "T") {               //VT=0x0B
                        buffer.append(0x0B);
                        continue;
                    }
                } else if (letter == "\\") {
                    //如果连着的是多个\\则对应添加\对应的16进制0x5C
                    buffer.append(0x5C);
                    continue;
                } else {
                    //将对应的\[前面的\\也要加入
                    buffer.append(0x5C);
                    buffer.append(letter.toLatin1());
                    continue;
                }
            }

            buffer.append(str.mid(i, 1).toLatin1());

        }

        return buffer;
    }
    //16进制字符串转10进制
    static int StrHexToDecimal(QString strHex)
    {
        bool ok;
        return strHex.toInt(&ok,16);
    }

    //10进制字符串转10进制
    static int StrDecimalToDecimal(QString strDecimal)
    {
        bool ok;
        return strDecimal.toInt(&ok,10);
    }

    //2进制字符串转10进制
    static int StrBinToDecimal(QString strBin)
    {
        bool ok;
        return strBin.toInt(&ok,2);
    }

    //16进制字符串转2进制字符串
    static QString StrHexToStrBin(QString strHex)
    {
        uchar decimal=StrHexToDecimal(strHex);
        QString bin=QString::number(decimal,2);

        uchar len=bin.length();
        if (len<8)
        {
            for (int i=0;i<8-len;i++)
            {
                bin="0"+bin;
            }
        }

        return bin;
    }

    //10进制转2进制字符串一个字节
    static QString DecimalToStrBin1(int decimal)
    {
        QString bin=QString::number(decimal,2);

        uchar len=bin.length();
        if (len<=8)
        {
            for (int i=0;i<8-len;i++)
            {
                bin="0"+bin;
            }
        }

        return bin;
    }

    //10进制转2进制字符串两个字节
    static QString DecimalToStrBin2(int decimal)
    {
        QString bin=QString::number(decimal,2);

        uchar len=bin.length();
        if (len<=16)
        {
            for (int i=0;i<16-len;i++)
            {
                bin="0"+bin;
            }
        }

        return bin;
    }

    //计算和校验码
    static uchar GetCheckCode(uchar data[],uchar len)
    {
        uchar temp=0;

        for (uchar i=0;i<len;i++)
        {
            temp+=data[i];
        }

        return temp%256;
    }

    //将溢出的char转为正确的uchar
    static uchar GetUChar(uchar data)
    {
        uchar temp;
        if (data>127)
        {
            temp=data+256;
        }
        else
        {
            temp=data;
        }
        return temp;
    }

    //延时
    static void Sleep(int sec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(sec);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    //获取当前路径
    static QString GetCurrentPath()
    {
        return QString(QCoreApplication::applicationDirPath()+"/");
    }

    //播放声音
    static void PlaySound(QString soundName)
    {
        QSound::play(soundName);
    }

    //设置系统日期时间
    static void setSystemDateTime(QString year, QString month, QString day, QString hour, QString min, QString sec)
    {
#ifdef Q_OS_WIN
        QProcess p(0);
        p.start("cmd");
        p.waitForStarted();
        p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toLatin1());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();
        p.start("cmd");
        p.waitForStarted();
        p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toLatin1());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();
#else
        QString cmd = QString("date %1%2%3%4%5.%6").arg(month).arg(day).arg(hour).arg(min).arg(year).arg(sec);
        system(cmd.toLatin1());
        system("hwclock -w");
#endif
    }

    //窗体居中，并且只有关闭按钮，不能调整大小
    static void FormOnlyCloseInCenter(QWidget *frm)
    {
        //设置窗体居中
        QDesktopWidget desktop;
        int screenX=desktop.availableGeometry().width();
        int screenY=desktop.availableGeometry().height()-40;
        int frmX=frm->width();
        int frmY=frm->height();
        QPoint movePoint(screenX/2-frmX/2,screenY/2-frmY/2);
        frm->move(movePoint);

        //设置窗体不能调整大小
        frm->setFixedSize(frmX,frmY);

        //设置窗体只有最小化按钮
        frm->setWindowFlags(Qt::WindowCloseButtonHint);
    }

    //窗体居中显示
    static void FormInCenter(QWidget *frm)
    {
        int screenX=qApp->desktop()->width();
        int screenY=qApp->desktop()->height()-60;
        int wndX=frm->width();
        int wndY=frm->height();
        QPoint movePoint((screenX-wndX)/2,(screenY-wndY)/2);
        frm->move(movePoint);
    }

    //窗体没有最大化按钮
    static void FormNoMaxButton(QWidget *frm)
    {
        frm->setWindowFlags(Qt::WindowMinimizeButtonHint);
    }

    //窗体只有关闭按钮
    static void FormOnlyCloseButton(QWidget *frm)
    {
        frm->setWindowFlags(Qt::WindowCloseButtonHint);
    }

    //窗体不能改变大小
    static void FormNotResize(QWidget *frm)
    {
        frm->setFixedSize(frm->width(),frm->height());
    }

    //获取桌面大小
    static QSize GetDesktopSize()
    {
        QDesktopWidget desktop;
        return QSize(desktop.availableGeometry().width(),desktop.availableGeometry().height());
    }

    //获取选择的文件
    static QString GetFileName(QString filter)
    {
        return QFileDialog::getOpenFileName(NULL,"选择文件",QCoreApplication::applicationDirPath(),filter);
    }

    //获取选择的文件集合
    static QStringList GetFileNames(QString filter)
    {
        return QFileDialog::getOpenFileNames(NULL,"选择文件",QCoreApplication::applicationDirPath(),filter);
    }

    //获取选择的目录
    static QString GetFolderName()
    {
        return QFileDialog::getExistingDirectory();;
    }

    //获取文件名，含拓展名
    static QString GetFileNameWithExtension(QString strFilePath)
    {
        QFileInfo fileInfo(strFilePath);
        return fileInfo.fileName();
    }

    //获取选择文件夹中的文件
    static QStringList GetFolderFileNames(QStringList filter)
    {
        QStringList fileList;
        QString strFolder = QFileDialog::getExistingDirectory();
        if (!strFolder.length()==0)
        {
            QDir myFolder(strFolder);

            if (myFolder.exists())
            {
                fileList= myFolder.entryList(filter);
            }
        }
        return fileList;
    }

    //文件夹是否存在
    static bool FolderIsExist(QString strFolder)
    {
        QDir tempFolder(strFolder);
        return tempFolder.exists();
    }

    //文件是否存在
    static bool FileIsExist(QString strFile)
    {
        QFile tempFile(strFile);
        return tempFile.exists();
    }

    //复制文件
    static bool CopyFile(QString sourceFile, QString targetFile)
    {
        if (FileIsExist(targetFile))
        {
            int ret=QMessageBox::information(NULL,"提示","文件已经存在，是否替换？",QMessageBox::Yes | QMessageBox::No);
            if (ret!=QMessageBox::Yes)
            {
                return false;
            }
        }
        return QFile::copy(sourceFile,targetFile);
    }

    static float AVG_DataFilter(QVector<float> Data_Value,  uint Data_Count)
    {
        uint i;
        float usTemp,uiSum;

        uiSum = 0;
        for(i = 0; i < Data_Count; i++)
        {
            uiSum += Data_Value.at(i);
        }
        usTemp = uiSum / Data_Count;
        return usTemp;
    }

};



#endif // HELPERCLASS_H
