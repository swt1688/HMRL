#ifndef S7_200_PCPPI_H
#define S7_200_PCPPI_H

#include <QObject>

class S7_200_PCPPI
{
public:
    S7_200_PCPPI();

    // only for ppi,是此文件的全局变量
    static unsigned int DataAddress;//读取的寄存器地址
    static unsigned char LastCommandType;

    static char S_Register[256];//步进寄存器
    static char AI_Register[32];//模拟量输入寄存器
    static char AQ_Register[32];//模拟量输出寄存器
    static char I_Register[128];//输入映像寄存器
    static char Q_Register[128];//输出映像寄存器
    static char M_Register[256];//位存储寄存器
    static char V_Register[5120];//变量存储寄存器

    static QByteArray uc_ppi_is_connect;
    static QByteArray uc_ppi_comm_count;	   	  //=2是为主考虑的，当从时无所谓
//    static QByteArray uc_ppi_acq_cmd_5c;
//    static QByteArray uc_ppi_EnterCount;
    static QByteArray ppi_master_step;
    static QByteArray MasterSendData;


    static unsigned char AddCheck(QByteArray DataBuffer,uchar StartByte,uchar LostByte);
    static QByteArray PPI_Record(QByteArray Recordbuffer);
//    QByteArray ppi_reqframe_anlys_slaver(unsigned char uc_ch,QByteArray puc_rec_buf,uchar me_address);
    static QByteArray ppi_send_command_10(uchar PLC_Address,uchar Me_Address,uchar Command);

};

#endif // S7_200_PCPPI_H
