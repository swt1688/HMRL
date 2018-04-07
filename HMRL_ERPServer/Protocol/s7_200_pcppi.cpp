#include "s7_200_pcppi.h"

#define comm_array_max_xb 70 //定义串口数组最大下标.(当写时,可以定义一个要写的字寄存器的最大数
#define PPI_F_MASTER_REQUIRED 0
#define PPI_F_MASTER_ENTERED  1
#define PPI_F_MASTER_SEND_DC  2
#define PPI_F_MASTER_ANSW_DC  3 //这是接收函数发的DC ME ME 指令，发完后定时，再有主程序引发主发函数工作
#define PPI_F_MASTER_E5AGAIN  4
#define PPI_F_MASTER_SUCCESS  5

// only for ppi,是此文件的全局变量
QByteArray S7_200_PCPPI::uc_ppi_is_connect;
QByteArray S7_200_PCPPI::uc_ppi_comm_count;	   	  //=2是为主考虑的，当从时无所谓
//QByteArray S7_200_PCPPI::uc_ppi_acq_cmd_5c = {0,0};
//QByteArray S7_200_PCPPI::uc_ppi_EnterCount = {0,0};
QByteArray S7_200_PCPPI::ppi_master_step;
QByteArray S7_200_PCPPI::MasterSendData;

// only for ppi,是此文件的全局变量
unsigned int S7_200_PCPPI::DataAddress;//读取的寄存器地址
unsigned char S7_200_PCPPI::LastCommandType;

char S7_200_PCPPI::S_Register[256];//顺序控制寄存器
char S7_200_PCPPI::AI_Register[32];//模拟量输入寄存器
char S7_200_PCPPI::AQ_Register[32];//模拟量输出寄存器
char S7_200_PCPPI::I_Register[128];//输入映像寄存器
char S7_200_PCPPI::Q_Register[128];//输出映像寄存器
char S7_200_PCPPI::M_Register[256];//位存储寄存器
char S7_200_PCPPI::V_Register[5120];//变量存储寄存器

S7_200_PCPPI::S7_200_PCPPI()
{

}

/********************************西门子PC/PPI协议分析****************************************************
 * PC与PLC采用主从方式通讯，PC按如下的格式发读写指令，PLC作出接收正确的响应（返回应答数据E5H或F9H见下文分析），
 * 上位机接到此响应则发出确认命令（10 02 5C 5E 16），PLC再返回给上位机相应数据。
 * SD LE LEr SD DA SA FC DSAP SSAP DU FCS ED
 * SD:开始符（68H） LE、Ler：长度（从DA到DU）
 * DA：目的地址 SA：源地址
 * FC：功能码 （6CH） DSAP：目的服务存取点
 * SSAP：源服务存取点 DU：数据单元
 * FCS：校验和 ED：结束符（16H）
 ******************************************PC读命令分析---->一次读一条数据***********************************
 * 对于一次读取一个数据，读命令都是33个字节。前面的0—21字节是相同的，为 ：
 * 68 1B 1B 68 02 00 6C 32 01 00 00 00 00 00 0E 00 00 04 01 12 0A 10
 * 因为是PC上发的读PLC数据的命令，SA=00，DA=02，如果有多个站，DA要改成相应的站号。读命令中从DA到DU的长度为1B即27个字节。从22字节开始根据读取数据的类型、位置不同而不同。表一是读不同存储器命令的Byte22—32。
 * 字节 22 23 24 25 26 27 28 29 30 31 32
 * 功能   读取长度  数据个数*  存储器类型  偏移量  校验  结束
 * 读q0.0 01 00 01 00 00 82 00 00 00 64 16
 * 读m0.0 01 00 01 00 00 83 00 00 00 65 16
 * 读M0.1 01 00 01 00 00 83 00 00 01 66 16
 * 读SMB34 02 00 01 00 00 05 00 01 10 F9 16
 * 读VB100 02 00 01 00 01 84 00 03 20 8B 16
 * 读VW100 04 00 01 00 01 84 00 03 20 8D 16
 * 读vd100 06 00 01 00 01 84 00 03 20 8F 16
 * 读i0.5 01 00 01 00 00 81 00 00 05 68 16
 * 读i0.7 01 00 01 00 00 81 00 00 07 6A 16
 * 读命令的Byte22-32从表中我们可以得出以下结果：
 * Byte 22 读取数据的长度--->>01：1 Bit 02：1 Byte 04：1 Word 06：Double Word
 * Byte 24数据个数,这里是01 ，一次读多个数据时见下面的说明。
 * Byte 26 存储器类型，01：V存储器 00：其它
 * Byte 27 存储器类型--->>04：S 05：SM 06：AI 07：AQ 1E: C  81：I 82：Q 83：M 84：V 1F: T
 * Byte 28,29,30存储器偏移量指针（存储器地址*8），如：VB100，存储器地址为100，偏移量指针为800,转换成16进制就是320H,则Byte 28—29这三个字节就是：00 03 20。
 * Byte 31 校验和，前面已说到这是从(DA+SA+DSAP+SSAP+DU) Mod 256 。
 * *********************************************一次读多条数据****************************************************
 * 对于一次读多个数据的情况，前21Byte与上面相似只是长度LD，LDr及Byte 14不同：
 * Byte 14 数据块占位字节，它指明数据块占用的字节数。与数据块数量有关，长度=4+数据块数*10,如：一条数据时为4+10=0E(H)；同时读M,V,Q三个不同的数据块时为4+3*10=22(H)。
 * Byte 22 总是02 即以Byte为单位。
 * Byte 24 以字节为单位，连续读取的字节数。如读2个VD则Byte24=8
 * Byte 19---30 按上述一次读一个数据的格式依次列出，
 * Byte 31---42 另一类型的数据，也是按上述格式给出。
 * 以此类推，一次最多读取222个字节的数据。
 * ***************************************写命令分析*****************************************************************
 * 一次写一个Double Word类型的数据，写命令是40个字节，其余为38个字节。
 * 写一个Double Word类型的数据，前面的0—21字节为 ：68 23 23 68 02 00 6C 32 01 00 00 00 00 00 0E 00 00 04 01 12 0A 10
 * 写一个其它类型的数据，前面的0—21字节为 ：68 21 21 68 02 00 6C 32 01 00 00 00 00 00 0E 00 00 04 01 12 0A 10 （与上面比较，只是长度字节发生变化）
 * 从22字节开始根据写入数据的值和位置不同而变化。表二是几个写命令的Byte22—40。
 * 字 节 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
 * 写入位置及值 长度 个数 类型 偏移量 位数 值、校验码、结束符
 * M0.0=1 01 00 01 00 00 83 00 00 00 00 03 00 01 01 00 71 16
 * M0.0=0 01 00 01 00 00 83 00 00 00 00 03 00 01 00 00 70 16
 * M0.1=1 01 00 01 00 00 83 00 00 01 00 03 00 01 01 00 72 16
 * vb100=10 02 00 01 00 01 84 00 03 20 00 04 00 08 10 00 AE 16
 * vb100=FF 02 00 01 00 01 84 00 03 20 00 04 00 08 FF 00 9D 16
 * VW100=FFFF 04 00 01 00 01 84 00 03 20 00 04 00 10 FF FF A6 16
 * VD100=FFFFFFFF 06 00 01 00 01 84 00 03 20 00 04 00 20 FF FF FF FF B8 16
 * 写命令的Byte22—40经分析我们可以得出以下结果：
 * Byte 22-- Byte 30 写入数据的长度、存储器类型、存储器偏移量与读命令相同。T，C等不能用写命令写入。
 * Byte 32 如果写入的是位数据这一字节为03，其它则为04
 * Byte 34 写入数据的位数  01: 1 Bit 08: 1 Byte 10H: 1 Word 20H: 1 Double Word
 * Byte 35--40值、校验码、结束符,如果写入的是位、字节数据，Byte35就是写入的值，Byte36=00，Byte37=检验码，Byte38=16H，结束。如果写个的是字数据（双字节），Byte35,Byte36就是写入的值， Byte37=检验码，Byte38=16H，结束。如果写个的是双字数据（四字节），Byte35—38就是写入的值， Byte39=检验码，Byte40=16H，结束。
 * ***************************************其它命令分析***********************************************
 * 强制写入
 * I、Q、S 等不能使用上述的写命令写入数据，只能用强制写入的方式。
 * 前0—35字节值如下（长度字段要根据实际情况而定）,需要注意的是Byte8=07,
 * 68 2B 2B 68 02 00 6C 32 07 00 00 00 00 00 0C 00 12 00 01 12 08 12 48 0B 00 00 00 00 00 FF 09 00 0E 00 01 10
 * 后面的内容如下：
 * Byte 32 占位字节，从下一字节开始到校验和前的字节数。说明同读数据的Byte 14.
 * Byte 36 强制写入数据的长度  01：1 Bit 02：1 Byte 04：1 Word 06：Double Word
 * Byte 38 数据个数,这里是01 ，一次强制写多个数据时见下面的说明。
 * Byte 40 存储器类型
 * Byte 41 存储器类型，见读命令的说明。
 * Byte 42、43、44存储器偏移量指针（存储器地址*8）
 * Byte 45、46、47、48 值、校验码、结束符
 * ***********************************取消强制写*******************************************************
 * 强制写入I、Q 等后，这些值就不能被程序改变，除非使用”取消强制命令”。 取消强制命令的格式与强制写入相似，变化的有以下几点：
 * （1）是没有”值”这一段，即没有Byte45—48。这影响到长度字节LE，LEr；占位字节Byte 32.
 * （2）Byte16=10H,
 *  (3) Byte32=0CH，也就是第一条，没有”值”这一段，数据块长度变短了。
 * 对于一次强制写入或取消多个数据的情况可以参照写入命令写出相应的报文。
 * *****************************************STOP命令
 * STOP命令使得S7-200 CPU从RUN状态转换到STOP状态（此时CPU模块上的模式开关开应打在RUN或TERM位置）。
 * PC发出如下命令，PLC返回F9，此时PLC已进入等待状态，PC再发确认报文（10 02 5C 5E 16）,完成一个命令过程。
 * 68 1D 1D 68 02 00 6C 32 01 00 00 00 00 00 10 00 00 29 00 00 00 00 00 09 50 5F 50 52 4F 47 52 41 4D AA 16
 * *****************************************RUN 命令
 * RUN命令使得S7-200 CPU从STOP状态转换到RUN状态（此时CPU模块上的模式开关开应打在RUN或TERM位置）。
 * PC发出下命令，PLC返回F9，此时PLC已进入运行状态，PC再发确认报文（10 02 5C 5E 16），完成一个命令过程。
 * 68 21 21 68 02 00 6C 32 01 00 00 00 00 00 14 00 00 28 00 00 00 00 00 00 FD 00 00 09 50 5F 50 52 4F 47 52 41 4D AA 16
 * *****************************************PLC响应读出数据分析*******************************************************
 * *****************************************一次读出一条数据
 * PLC响应的数据也是用PPI封装的。如果用一次读一条数据命令，响应的报文中就只包含一条数据，此响应报文的Byte16<=8。
 * Byte 04：DA=00 Byte 05：SA=02 即从02 PLC站发往PC。
 * Byte 16：数据块占位字节，从Byte21到校验和前的字节数。
 * 一条数据时：Word=06 Double Word=08 其它为 05。
 * Byte 22：数据类型，位=3，其它=4。
 * Byte 24：数据宽度，Bit=01,Byte=08,Word=10H,Double Word=20H
 * Byte 25—28：值。
 * 如果网络上只可能有一个站会发回响应报文，那么可以简单的根据LE长度字节判断返回值的位置：LE=16H，返回值是字节，或位类型的值，响应报文的Byte 25即是返回值；LE=17H，返回值是字（双字节）类型的值，响应报文的Byte 25，26即是返回值；LE=19H，返回值是双字（四字节）类型的值，响应报文的Byte 25—28即是返回值。更准确的方式是要根据返回报文的SA，DA，及存储器位置等信息识别目标地址和源地址，确认是这次申请的返回数据，然后经过校验检查，得到正确的数据。
 * *****************************************一次读出多条数据
 * 如果用的是一次读多条数据的命令，响应的报文中就包含有多条数据。这些数据只有类型参数，没有偏移量参数，所以要注意根据读命令的顺序将其一一对应起来。
 * Byte 16：数据块占位字节，从Byte21到校验和前的字节数，与数据块数量和类型有关。
 * Byte 20：数据块的个数。
 * Byte 21 开始为数据块，每一个数据块都以FF 04开始，接下来的两个字节表示这一数据块的长度，以位计算，然后依次是连续的数据。下一个数据块也是以FF 04开始，重复上述格式，直到结束。
*/

//PPI通讯协议如下：
//主发写命令，从返回E5（准备就绪命令），主收到后发确认命令（10 SLAVER,MASTER,5C,ADD,16），从发（68 12 12 68 。。。）完毕
//主发读命令，从返回E5（准备就绪命令），主收到后发确认命令（10 SLAVER,MASTER,5C,ADD,16），从发（68 xx xx 68 。。。）完毕

/************************************************************************************************
**描述: PPI校验,和校验或称偶校验，
**         丢弃进位.返回一个8位的数
************************************************************************************************/
unsigned char S7_200_PCPPI::AddCheck(QByteArray DataBuffer,uchar StartByte,uchar LostByte)
{
    uchar uc_sum,i;
    uc_sum  = 0;
    i=DataBuffer.count()-LostByte;

    while(i--)
    {
        uc_sum += DataBuffer.at(StartByte++);
    }
    return (uc_sum);
}

/*****************************************************************************************
 * PPI 主机发送10呼叫命令
*****************************************************************************************/
QByteArray S7_200_PCPPI::ppi_send_command_10(uchar PLC_Address,uchar Me_Address,uchar Command)
{
    QByteArray SendBuffer;
    uchar CheckValue;

    SendBuffer.append(0x10) ;
    SendBuffer.append(PLC_Address);
    SendBuffer.append(Me_Address);
    SendBuffer.append(Command);

    CheckValue = 0;
    //进行和校验
    CheckValue=AddCheck(SendBuffer,1,SendBuffer.count());
    SendBuffer.append(CheckValue);
    SendBuffer.append(0x16);
    return(SendBuffer);
}

QByteArray S7_200_PCPPI::PPI_Record(QByteArray Recordbuffer)
{
    QByteArray SendBuffer;
    //刚才自己发的是请求命令，应该收到的是E5,0为发送请求后的返回数据
    unsigned char MeAddress,PLCAddress;
    switch(Recordbuffer.at(0))
    {
        case 0xe5:
        {
            if(LastCommandType==0)
            {
                //S7_200_PCPPI::uc_ppi_is_connect.at(uc_ch) = 1;
                LastCommandType=1;//确认命令发送完毕
                return ppi_send_command_10(PLCAddress,MeAddress,0x5c);
            }
            else if(LastCommandType==2)
            {
                LastCommandType=3;//确认命令发送完毕
                return MasterSendData;
            }
        }
        break;
        case 0x10:
        {
            if((Recordbuffer.count()!=6)||(Recordbuffer.at(5)!=0x16))
            {
                SendBuffer.append(0xfe);
                return SendBuffer;
            }
            //if(Recordbuffer.at(1)!=MeAddress) return 0xfe;
            if(AddCheck(Recordbuffer,1,3) != Recordbuffer.at(Recordbuffer.count()-2))
            {
                SendBuffer.append(0xfe);
                return SendBuffer;
            }
        }
        break;
        case 0x68:
        {
            int DataLength=(Recordbuffer.at(23)<<8)|Recordbuffer.at(24);//数据长度
            if(Recordbuffer.at(19)==0x04)
            {
                switch(DataLength)
                {
                case 0x01: DataLength=1; break;
                case 0x08: DataLength=1; break;
                case 0x10: DataLength=2; break;
                case 0x20: DataLength=4; break;
                case 0x40: DataLength=8; break;
                }
                switch(LastCommandType)
                {
                case 0x04:
                    for(char i=0;i<DataLength;i++)
                    {
                        S_Register[DataAddress+i]=Recordbuffer.at(25+i);
                    }break;
                case 0x06:
                    for(char i=0;i<DataLength;i++)
                    {
                        AI_Register[DataAddress+i]=Recordbuffer.at(25+i);
                    }break;
                case 0x07:
                    for(char i=0;i<DataLength;i++)
                    {
                        AQ_Register[DataAddress+i]=Recordbuffer.at(25+i);
                    }break;
                case 0x81:
                    for(char i=0;i<DataLength;i++)
                    {
                        I_Register[DataAddress+i]=Recordbuffer.at(25+i);
                    }break;
                case 0x82:
                    for(char i=0;i<DataLength;i++)
                    {
                        Q_Register[DataAddress+i]=Recordbuffer.at(25+i);
                    }break;
                case 0x83:
                    for(char i=0;i<DataLength;i++)
                    {
                        M_Register[DataAddress+i]=Recordbuffer.at(25+i);
                    }break;
                case 0x84:
                    for(char i=0;i<DataLength;i++)
                    {
                        V_Register[DataAddress+i]=Recordbuffer.at(25+i);
                    }break;
                }

            }
        }
        break;
    }


}
//*****************************************************************************************
//**Purpose:当PPI为主时,分析接收帧,并根据接收帧作相应处理.
//**Entry: puc_rec_buf:---指向接收缓冲. uc_rec_len:---接收缓冲的长.uc_op_table_pos:指向发送命令表的下标。
//**当PPI为主时,接收帧有如下可能：
//** 1。返回E5，表示准备好命令，本机应该发送确认命令 10 ，PLC，ME，5C
//**    但返回2个E5，则忽略，发下个命令
//** 2。返回的是回复指令，如从PLC读数返回68，2x,2x,68,ME,PLC..... 向PLC写数返回68 12 12 68....
//** 3。异常：有两种，一个是E5反复发，一个是发 10,ME,PLC,02


//QByteArray S7_200_PCPPI::ppi_reqframe_anlys_master(uchar uc_ch,QByteArray puc_rec_buf,uchar uc_rec_len,uchar *uc_op_table_pos,
//                                uchar me_address,uchar uc_plc_address,uchar *next_send_ms_count)
//{
//    QByteArray puc_send_buf;
//    uchar uc_mid,uc_loop;
//    uchar ucp_send_count = 0;
//    uint  u16;

//    if(S7_200_PCPPI::uc_ppi_master_step.at(uc_ch) == PPI_F_MASTER_REQUIRED)//刚才自己发的是请求命令，应该收到的是E5
//    {
//        //先写确认命令,只有PPI为MASTER时才调用，4.12后，收到连续两个E5，不再调用确认命令了
//        if(puc_rec_buf.at(0) == 0xe5)// && (uc_rec_len == 1))
//        {
//            S7_200_PCPPI::uc_ppi_is_connect.at(uc_ch) = 1;

//            if(S7_200_PCPPI::uc_ppi_acq_cmd_5c.at(uc_ch))//现在是确认指令，请求是5C，则确认是7C
//            {
//                uc_mid = 0x7c;
//            }
//            else
//            {
//                uc_mid = 0x5c;
//            }

//            uc_ppi_master_step.at(uc_ch) = PPI_F_MASTER_ENTERED;
//            *next_send_ms_count = 250;  //本命令只发了5个字节，然后该收了，只要收到，则该变量会变255ms,如果没有反应了，则50ms后主发DC
//            return(ppi_send_command_10(puc_send_buf,me_address,uc_plc_address,uc_mid));
//        }
//        else if(puc_rec_buf.at(0) == 0x68)//发现是68，转到下面处理了
//        {
//            uc_ppi_master_step.at(uc_ch) = PPI_F_MASTER_ENTERED;
//        }
//        else//可能是10 me plc 02 不管他，直接发DC
//        {
//            //*puc_send_buf++ = 0xDC;
//            //*puc_send_buf++ = me_address;
//            //*puc_send_buf = me_address;

//            uc_ppi_master_step[uc_ch] = PPI_F_MASTER_REQUIRED;
//            *next_send_ms_count = 100;  //本命令只发了5个字节，应该什么也收不到，50ms后主发DC

//            return(0xfe);
//        }
//    }
//    //不用else if 的原因是上面的程序要不就退出了或者跳转了，执行到这里，可能是上面发现了不发E5直接回复的现象
//    if((uc_ppi_master_step[uc_ch] == PPI_F_MASTER_ENTERED) || (uc_ppi_master_step[uc_ch] == PPI_F_MASTER_E5AGAIN)) //已经发过了确认命令了，现在接受来的应该是68,12,12(写),68,2x,2x,(读)
//    {
//        if((*puc_rec_buf == 0xe5) && (uc_rec_len == 1))//还来E5，发10,PLC,ME,7C/5C
//        {

//            if(++uc_ppi_EnterCount[uc_ch] >10)
//            {
//                uc_ppi_EnterCount[uc_ch] = 0;  //计数器清除
//                *puc_send_buf++ = 0xDC;
//                *puc_send_buf++ = me_address;
//                *puc_send_buf = me_address;

//                uc_ppi_master_step[uc_ch] = PPI_F_MASTER_ANSW_DC;
//                *next_send_ms_count = 250;  //本命令只发了3个字节，应该什么也收不到，50ms后主发确认，并把step = PPI_F_MASTER_E5AGAIN

//                return(2);
//            }
//            else
//            {
//                if(uc_ppi_acq_cmd_5c[uc_ch])//现在是确认指令，请求是5C，则此次就发5C,反转呼叫申请和确认的5C和7C
//                {
//                    uc_mid = 0x5c;
//                    uc_ppi_acq_cmd_5c[uc_ch] = 0;
//                }
//                else
//                {
//                    uc_mid = 0x7c;
//                    uc_ppi_acq_cmd_5c[uc_ch] = 1;
//                }


//                uc_ppi_master_step[uc_ch] = PPI_F_MASTER_E5AGAIN;
//                *next_send_ms_count = 250;
//                //应该收到68，或者E5
//                //50MS什么数也没收到的话，转主发了，并且发的实际上是DC
//                return(ppi_send_command_10(puc_send_buf,me_address,uc_plc_address,uc_mid));
//            }
//        }
//        //又来E5处理完毕了

//        if(*puc_rec_buf == 0x68)
//        {
//            if(stMod_Master_OP[*uc_op_table_pos].uc_cmd == PPI_READ_BYTE_REG)//我读PLC的数，要填写到我的表中
//            {
//                if(*(puc_rec_buf + 4) != me_address){return(0xfe);}   //地址效验
//                puc_rec_buf += 3;  //从第二个68开始
//                uc_rec_len -= 3;   //长度跟着调整

//                if(*puc_rec_buf != 0x68){return(0xfe);}  //是不是0x68
//                if(uc_rec_len < 24){return(0xfe);}       //至少要有的长度
//                if(*(puc_rec_buf-1) != *(puc_rec_buf-2)){return(0xfe);}//有错
//                if(*(puc_rec_buf-1) <= 15){return(0xfe);}

//                if(uc_ppi_addtest(puc_rec_buf+1,uc_rec_len-3) != *(puc_rec_buf + uc_rec_len - 2))  //和效验
//                {
//                    return(0xfe);
//                }

//                //以下效验也对了
//                //总的接受字节数量，注意以后是不是可能超过255
//                uc_mid = *(puc_rec_buf+13) - 4;
//                u16 = stMod_Master_OP[*uc_op_table_pos].ui_mem_begin_addr;
//                for(uc_loop = 0; uc_loop < uc_mid; uc_loop++)
//                {
//                    PPI_write_singleReg(u16 + uc_loop, *(puc_rec_buf+ 22 + uc_loop));
//                }
//            }
//            //else我写PLC的数，返回应该是68,12,12,不管是不是，都发DC了
//        }
//        //全当成功了
//        *next_send_ms_count = 1;//50;
//        uc_ppi_master_step[uc_ch] = PPI_F_MASTER_SUCCESS;
//        return(0xfe);
//        //上两个分支的解释在上面
//        //也就是说当PPI_F_MASTER_ENTERED时，收到E5则发7C的确认命令，收到68，并且大约对就都不发了
//        //但若PPI_F_MASTER_E5AGAIN后再来E5，也不是68，则发DC,ME,ME
//    }

//    return(0xfe);//为了严谨
//}
//****************************************************************************
//**Purpose: 当PPI本机为从时，接受到了主的命令，命令有两种，申请和确认
//**  uc_rec_len是长度，不是下标。先效验 返回为发送串长（下标），返回0xff为不发送
//QByteArray S7_200_PCPPI::ppi_reqframe_anlys_slaver(unsigned char uc_ch,QByteArray puc_rec_buf,uchar me_address)
//{
//    static uchar st_uc_ppi_var_type[2] = {2,2};//传输的数据类型
//    static uchar st_uc_ppi_var_num[2];//传输的数据个数
//    static uint  st_ui_ppi_me_var_add[2];
//    static uchar st_uc_ppi_connect_type[2]={0,0};
//    QByteArray puc_send_buf;
//    uchar ucp_send_count,uc_count;
//    uint  u16;
//    uchar uc_mid1,uc_mid2,uc_mid3;


//    if(puc_rec_buf.at(0) == 0x10)//确认命令
//    {
//        if((puc_rec_buf.count() != 6) || (puc_rec_buf.at(5)!=0x16))//长度不对，或者结束符号不对
//        {
//            return(0xfe);
//        }

//        if(puc_rec_buf.at(1) != me_address)//检查地址是否一致
//        {
//            return(0xfe);
//        }

//        if(AddCheck(puc_rec_buf,1,3) != puc_rec_buf.at(puc_rec_buf.count()-2))//检查效验和是否相等
//        {
//            return(0xfe);
//        }
//        //10 03 02 5c 61 16
//        //if(st_uc_ppi_connect_type[uc_ch] == 0)//还没连接
//        //{
//        //	return(0xfe);
//        //}
//        //以上是2008.6.24修改，因为如果上电或其他因素
//        //首发DC命令，再发49命令，以上导致对49命令无响应（因为无连接），而PLC死循环

//        ucp_send_count = 0;

//        if(puc_rec_buf.at(3) != 0x49)  //最多出现的是5C，而7C也见过,49寻呼指令
//        {

//            uc_count = st_uc_ppi_var_num[uc_ch];  //此后的计算结果，uc_count将成为字节数

//            uc_mid1 = st_uc_ppi_var_type[uc_ch];
//            if(uc_mid1 == 4)//如果是字
//            {uc_count *=2;}
//            else if(uc_mid1 == 6)//如果是双字
//            {uc_count *=4;}
//            else if(uc_mid1 == 1)//位
//            {return(0xfe);}

//            if((uc_count+26) >= comm_array_max_xb)//uc_count大于串口数组下标最大值
//            {return(0xfe);}

//            if(st_uc_ppi_connect_type[uc_ch] == 1)//连接的性质，主PLC读本机
//            {
//                uc_mid1 = 0x15 + uc_count;
//                uc_mid2 = 4 + uc_count;
//                uc_mid3 = 0x04;
//            }
//            else
//            {
//                uc_mid1 = 0x12;
//                uc_mid2 = 0x01;
//                uc_mid3 = 0x05;
//            }
//            puc_send_buf<<0x68<<uc_mid1<<uc_mid1<<0x68<<puc_rec_buf.at(2)<<puc_rec_buf.at(1)<<0x08<<0x32<<0x03<<0x00<<0x00
//                       <<uc_ppi_comm_count.at(uc_ch)<<uc_ppi_comm_count[uc_ch]<<0x00<<0x02<<uc_mid2<<0x00<<0x00<<uc_mid3
//                       <<0x01<<0xff;

//            if(st_uc_ppi_connect_type[uc_ch] == 1)//连接的性质，主PLC读本机
//            {
//                puc_send_buf << 0x04;

//                u16  = uc_count;
//                u16 *= 8;
//                puc_send_buf.append((u16>>8)&0xff);
//                puc_send_buf.append(u16&0xff);

//                for(uc_mid1 = 0;uc_mid1<uc_count;uc_mid1++)
//                {
//                    puc_send_buf.append(uc_PPI_ret_Word(st_ui_ppi_me_var_add[uc_ch] + uc_mid1)) ;
//                }
//            }
//            //从4开始，就是从地址开始,-4是个数
//            uc_mid1 = AddCheck(puc_send_buf,4,puc_send_buf.count()-4);//计算和校验
//            puc_send_buf.append(uc_mid1);
//            puc_send_buf.append(0x16);
//            return puc_send_buf;  //返回的是数据
//        }
//        else //if((*puc_rec_buf + 3) == 0x49) )似乎主机在问本机是不是也是主机，回答不是
//        {
//            puc_send_buf << 0x10<<puc_rec_buf.at(2)<<puc_rec_buf.at(1)<<0x00;
//            puc_send_buf<<AddCheck(puc_send_buf,1,3);

//            puc_send_buf << 0x16;
//            return puc_send_buf;  //返回的是数据
//        }//if((*puc_rec_buf + 3) == 0x5c))

//    }//确认命令至此完成了
//    /////////////////////////////////////////////////////////////////////////////////////////////////////
//    //热烈祝贺确认类型命令完成了，下面是申请命令，地址和效验都要重新做
//    /////////////////////////////////////////////////////////////////////////////////////////////////////
//    else if((puc_rec_buf.at(0) == 0xdc) || (puc_rec_buf.at(0) == 0x68))  //可能是呼叫申请，也可能是其他PPI的数据
//    {
//        if((puc_rec_buf.count() < 33) || (puc_rec_buf.at(puc_rec_buf.count()-1) != 0x16))//长度不对，或者结束符号不对
//        {
//            return(0xfe);
//        }

//        if(puc_rec_buf.at(0) == 0xdc)
//        {
//            //puc_rec_buf += 3;
//            //uc_rec_len -= 3;
//            if(puc_rec_buf.at(3) != 0x68){return(0xfe);}
//        }

//        if(puc_rec_buf.at(4) != me_address){return(0xfe);}   //地址效验
//        //puc_rec_buf += 3;  //从第二个68开始
//        //uc_rec_len -= 3;   //长度跟着调整

//        if(AddCheck(puc_rec_buf,4,puc_rec_buf.count()-6) != puc_rec_buf.at(puc_rec_buf.count()-2))  //和效验
//        {
//            //st_uc_ppi_connect_type[uc_ch] = 0;//地址对了，但是效验错了，取消连接？
//            //2008.6.24 不取消连接
//            return(0xfe);
//        }
//        st_uc_ppi_var_type[uc_ch]=puc_rec_buf.at(22);//读取的数据长度01->1bit,02->1Byte,04->1Word,06->DoubleWord
//        st_uc_ppi_var_num[uc_ch]=puc_rec_buf.at(24);//读取的数据个数
//        uc_ppi_comm_count[uc_ch]=puc_rec_buf.at(11);//协议数据
//        st_ui_ppi_me_var_add[uc_ch]=((puc_rec_buf.at(29)<<8)|puc_rec_buf.at(30))/8;//存储器偏移量指针
//        if(puc_rec_buf.at(2) == 0x1b)//st_uc_ppi_connect_type[uc_ch]=1是读，2是写
//        {
//            st_uc_ppi_connect_type[uc_ch] = 1;  //主PLC要从本机读数据
//            puc_send_buf.append(0xe5);
//            return puc_send_buf;  //返回的是下标,却只发一个字节
//        }
//        //PLC写入功能不用包含在只读的采集模块仪表中
//        else  if(puc_rec_buf.at(2) >= 0x20)//是写入命令，主PLC写入信息
//        {
//            st_uc_ppi_connect_type[uc_ch] = 2;

//            uc_count = st_uc_ppi_var_num[uc_ch];  //此后的计算结果，uc_count将成为字节数

//            uc_mid1 = st_uc_ppi_var_type[uc_ch];
//            if(uc_mid1 == 4)//如果是字
//            {uc_count *=2;}
//            else if(uc_mid1 == 6)//如果是双字
//            {uc_count *=4;}
//            else if(uc_mid1 == 1)//位
//            {return(0xfe);}

//            for(uc_mid1 = 0;uc_mid1<uc_count;uc_mid1++)
//            {

//                PPI_write_singleReg(st_ui_ppi_me_var_add[uc_ch] + uc_mid1, puc_rec_buf.at(32 + uc_mid1));//将数据写到对应的地址
//                //开始地址+字节指针            从32（35从第一个68开始）
//            }
//            puc_send_buf<<0xe5;//发E5
//            return puc_send_buf;  //返回的是下标,却只发一个字节
//        }
//        else  //不是读，也不是写，不认识
//        {
//            //st_uc_ppi_connect_type[uc_ch] = 0;
//            //2008.6.24不取消连接
//            return(0xfe);
//        }

//    }//else if(((*puc_rec_buf) == 0xdc) || ((*puc_rec_buf) == 0x68))

//    return(0xfe);
//}



/****************************************业务流程有必要写一下******************************************************
 * 主发写命令，从返回E5（准备就绪命令），主收到后发确认命令（10 SLAVER,MASTER,5C,ADD,16），从发（68 12 12 68 。。。）完毕
 * 主发读命令，从返回E5（准备就绪命令），主收到后发确认命令（10 SLAVER,MASTER,5C,ADD,16），从发（68 xx xx 68 。。。）完毕
 * 接受部分和MODBUS基本相同：
 * 在接受中断中，重置了3.5字节的接受完毕，并且将发送的计数器g_uc_ppi_main_send_count=-1，就是最大值，这样，就会先进入接收程序，而不会由定时器导致发送
 * 而在主程序中，要对g_uc_ppi_main_send_count每ms--,到0后发出is_ppi_master_need_send消息
 * 而主接收函数ppi_reqframe_anlys_master 和主发送函数ppi_master_send都可能会组成发送串，并且会重写g_uc_ppi_main_send_count（函数内叫next_send_ms_count）
 * 当g_uc_ppi_main_send_count（next_send_ms_count）到时间后叫“超时”，将导致发送，用STEP同步
//#define PPI_F_MASTER_REQUIRED 0
//#define PPI_F_MASTER_ENTERED  1
//#define PPI_F_MASTER_SEND_DC  2
//#define PPI_F_MASTER_ANSW_DC  3 //这是接收函数发的DC ME ME 指令，发完后定时，再有主程序引发主发函数工作
//#define PPI_F_MASTER_E5AGAIN  4
//#define PPI_F_MASTER_SUCCESS  5
 * 上电的时候， step == PPI_F_MASTER_REQUIRED, 进入发送程序，发的实际上是DC me me ,step 变成 PPI_F_MASTER_SEND_DC，10ms后发
 * 10ms后发送请求，将OP表++，查发送指令后发送申请，step变成PPI_F_MASTER_REQUIRED从机若无反应（超时），则实际上回到上一步
 * 若接受到信息，则进入ppi_reqframe_anlys_master，
 * 应该是E5，发确认命令，step变成PPI_F_MASTER_ENTERED，若从机没反应（超时），发DC ME ME
 * 若不是E5，而是直接收到数据，则转入“收到数据：”
 * 若不是E5，什么也不发，step变成PPI_F_MASTER_ENTERED，从机定没反应（超时），发DC ME ME
 * 若收到数据：
 * 如果还是E5，step变成PPI_F_MASTER_E5AGAIN，则重新发10命令，若干次后重新发送DC ME ME，STEP变PPI_F_MASTER_ANSW_DC，并转换5C，7C
 * 若是数据，分析数据，step变成PPI_F_MASTER_SUCCESS，1MS后发DC ME ME
*****************************************************************************************/


/*****************************************************************************
//以下是自己发出了写PLC内部命令组成发送数据串，返回串长度
*****************************************************************************/
//uchar S7_200_PCPPI::ppi_master_send(uchar uc_ch,uchar *uc_op_table_pos,uchar uc_plc_address,uchar *puc_send_buf,uchar me_address,uchar *next_send_ms_count)

//{
//    uchar uc_mid,uc_loop,uc_count;
//    uchar ucp_send_count = 0;
//    unsigned long l16;
//    uint u16;
//    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //进入本函数，有如下可能：				1：没响应 则由g_uc_commmain_send_count超时引起
//    //											  分两种 (1)没PLC <- 我发了REQ命令
//    //											  		 (2)我发了DC me me命令
//    //										都在本函数中预设g_uc_commmain_send_count
//    //									2: 接收程序发送了DC me me命令，导致无响应，也由g_uc_commmain_send_count导致
//    //									   在本函数中判断uc_ppi_master_step，看是谁导致的无响应
//    //
//    //能进入本函数，则必须发信息，因为进入本函数有可能是接收函数发了DC指令导致的没有响应，
//    //由接收函数g_uc_commmain_send_count设置的引起的超时，主程序中发送is_ppi_master_need_send
//    uc_mid = uc_ppi_master_step.at(uc_ch);
//    //  主接收程序发DC ME ME 指令引发的没反应
//    if(uc_mid == PPI_F_MASTER_ANSW_DC)
//    {
//        if(uc_ppi_acq_cmd_5c.at(uc_ch))  //上次请求命令是5C，则这次改为7C
//        {
//            uc_ppi_acq_cmd_5c.at(uc_ch) = 0;
//            uc_mid = 0x5c; //请求命令改为7C了，则确认命令发5C
//        }
//        else
//        {
//            uc_ppi_acq_cmd_5c.at(uc_ch) = 1;
//            uc_mid =0x7c;
//        }

//        uc_ppi_master_step.at(uc_ch)   = PPI_F_MASTER_E5AGAIN;
//        *next_send_ms_count = 250;      //无响应超时，PLC可能发E5，或者苏醒发68,XX,XX,68....都将在接受程序中处理了next_send_ms_count
//        //所以再进入本函数的话，那是没反应了，所以应该发DC
//        return( ppi_send_command_10(puc_send_buf,me_address,uc_plc_address,uc_mid)  );
//    }

//    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //  发申请指令
//    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //先把OP表的指针++
//    if(uc_mid == PPI_F_MASTER_SEND_DC)
//    {
//        if(++(*uc_op_table_pos) >= OP_TABLE_LENTH){*uc_op_table_pos = 0;}

//        uc_mid = stMod_Master_OP[*uc_op_table_pos].uc_cmd;

//        if(uc_mid == PPI_READ_BYTE_REG)
//        {uc_count = 0x1b;}
//        else
//        {uc_count = 0x1f + stMod_Master_OP[*uc_op_table_pos].uc_lenth;}

//        *(puc_send_buf + ucp_send_count++) = 0x68;
//        *(puc_send_buf + ucp_send_count++) = uc_count;
//        *(puc_send_buf + ucp_send_count++) = uc_count;
//        *(puc_send_buf + ucp_send_count++) = 0x68;
//        *(puc_send_buf + ucp_send_count++) = uc_plc_address;  //address
//        *(puc_send_buf + ucp_send_count++) = me_address;

//        if(uc_ppi_is_connect[uc_ch] == 0)
//        {
//            *(puc_send_buf + ucp_send_count++) = 0x6c;
//            uc_ppi_comm_count[uc_ch] = 2;
//            uc_ppi_is_connect[uc_ch] = 1;
//        }
//        else
//        {
//            uc_ppi_is_connect[uc_ch]++;

//            if(uc_ppi_is_connect[uc_ch] > 5)
//            {
//             if(uc_ppi_acq_cmd_5c[uc_ch]){uc_ppi_acq_cmd_5c[uc_ch]=0;}else{uc_ppi_acq_cmd_5c[uc_ch]=-1;}
//             uc_ppi_is_connect[uc_ch] = 1;
//            }

//            if(uc_ppi_acq_cmd_5c[uc_ch])//如果申请是5C，则确认是7C，否则反之
//            {
//                *(puc_send_buf + ucp_send_count++) = 0x5c;
//            }
//            else
//            {
//                *(puc_send_buf + ucp_send_count++) = 0x7c;
//            }
//        }
//        *(puc_send_buf + ucp_send_count++) = 0x32;
//        *(puc_send_buf + ucp_send_count++) = 0x01;
//        *(puc_send_buf + ucp_send_count++) = 0x00;
//        *(puc_send_buf + ucp_send_count++) = 0x00;

//        *(puc_send_buf + ucp_send_count++) = uc_ppi_comm_count[uc_ch];
//        *(puc_send_buf + ucp_send_count++) = uc_ppi_comm_count[uc_ch];
//        uc_ppi_comm_count[uc_ch] += 1;
//        *(puc_send_buf + ucp_send_count++) = 0x00;
//        *(puc_send_buf + ucp_send_count++) = 0x0e;
//        *(puc_send_buf + ucp_send_count++) = 0x00;

//        l16 =stMod_Master_OP[*uc_op_table_pos].ui_plc_begin_addr;
//        l16 *=8;
//        if(uc_mid == PPI_READ_BYTE_REG)
//        {
//            *(puc_send_buf + ucp_send_count++) = 0x00;
//            *(puc_send_buf + ucp_send_count++) = 0x04;
//        }
//        else
//        {
//            *(puc_send_buf + ucp_send_count++) =stMod_Master_OP[*uc_op_table_pos].uc_lenth + 4;
//            *(puc_send_buf + ucp_send_count++) = 0x05;
//        }
//        *(puc_send_buf + ucp_send_count++) = 0x01;
//        *(puc_send_buf + ucp_send_count++) = 0x12;
//        *(puc_send_buf + ucp_send_count++) = 0x0A;
//        *(puc_send_buf + ucp_send_count++) = 0x10;

//        *(puc_send_buf + ucp_send_count++) = 0x02;  //即以Byte为单位,01：1Bit    02：1 Byte    04：1 Word    06：Double Word
//        *(puc_send_buf + ucp_send_count++) = 0x00;
//        *(puc_send_buf + ucp_send_count++) = stMod_Master_OP[*uc_op_table_pos].uc_lenth;

//        *(puc_send_buf + ucp_send_count++) = 0x00;
//        *(puc_send_buf + ucp_send_count++) = 0x01;
//        *(puc_send_buf + ucp_send_count++) = 0x84;

//        *(puc_send_buf + ucp_send_count++) = (UCHAR)(l16>>16);
//        *(puc_send_buf + ucp_send_count++) = (UCHAR)(l16>>8);
//        *(puc_send_buf + ucp_send_count++) = (UCHAR)(l16);
//        if(uc_mid != PPI_READ_BYTE_REG)
//        {
//            *(puc_send_buf + ucp_send_count++) = 0x00;
//            *(puc_send_buf + ucp_send_count++) = 0x04;

//            u16 =stMod_Master_OP[*uc_op_table_pos].uc_lenth;
//            u16 *=8;

//            *(puc_send_buf + ucp_send_count++) = HI_UINT(u16);
//            *(puc_send_buf + ucp_send_count++) = LO_UINT(u16);

//            uc_mid = stMod_Master_OP[*uc_op_table_pos].uc_lenth;
//            u16    = stMod_Master_OP[*uc_op_table_pos].ui_mem_begin_addr;

//            for(uc_loop = 0;uc_loop < uc_mid;uc_loop++)
//            {
//                *(puc_send_buf + ucp_send_count++) = uc_PPI_ret_Word(u16 + uc_loop);
//            }


//        }//if(uc_mid == PPI_MAIN_READ)
//        //从4开始，就是从地址开始,-4是个数
//        uc_mid = uc_ppi_addtest(puc_send_buf + 4,ucp_send_count - 4);
//        *(puc_send_buf + ucp_send_count++) = uc_mid;
//        *(puc_send_buf + ucp_send_count) = 0x16;

//        uc_ppi_master_step[uc_ch]  = PPI_F_MASTER_REQUIRED;
//        if(ucp_send_count<128){uc_mid = ucp_send_count<<2;}else{uc_mid = 255;}
//        *next_send_ms_count = 250;  //uc_mid;      //应该返回E5，若无反应是不应该的，重发DC
//        //假设在9600下，最多等满255MS
//        return(ucp_send_count);  //返回的是下标
//    }

//    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    // ****************发：DC ME ME
//    //  可能是申请呼叫了     没反应        或       通讯已经成功          或   本函数或接收函数发了确认命令后没响应而超时了
//    //  除主发DC及从发了DC外的其他一切情况
//    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//        *puc_send_buf++ = 0xdc;
//        *puc_send_buf++ = me_address;
//        *puc_send_buf   = me_address;
//        *next_send_ms_count = 10;  //小到5是不行的
//        uc_ppi_master_step[uc_ch]   = PPI_F_MASTER_SEND_DC;

//        return(2);  //返回的是下标
//         //7MS后发申请指令

//    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//}



//附上PPI的监听结果
/*
发的字节数    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37
1    DC 02 02 68 20 20 68 03 02 7C 32 01 00 00 0A 0A 00 0E 00 05 05 01 12 0A 10 02 00 01 00 01 84 00 09 60 00 04 00 08 01 0B 16                            68 12 12 68 02 03 08 32 03 00 00 02 02 00 02 00 01 00 00 05 01 FF 4E 16                           68 12 12 68 02 03 08 32 03 00 00 06 06 00 02 00 01 00 00 05 01 FF 56 16
2    DC 02 02 68 21 21 68 03 02 7C 32 01 00 00 09 09 00 0E 00 06 05 01 12 0A 10 02 00 02 00 01 84 00 09 60 00 04 00 10 01 02 15 16
3    DC 02 02 68 22 22 68 03 02 7C 32 01 00 00 0A 0A 00 0E 00 07 05 01 12 0A 10 02 00 03 00 01 84 00 09 60 00 04 00 18 01 02 03 24 16                      68 12 12 68 02 03 08 32 03 00 00 02 02 00 02 00 01 00 00 05 01 FF 4E 16                           68 12 12 68 02 03 08 32 03 00 00 06 06 00 02 00 01 00 00 05 01 FF 56 16
3首  DC 02 02 68 22 22 68 03 02 6C 32 01 00 00 02 02 00 0E 00 07 05 01 12 0A 10 02 00 03 00 01 84 00 09 60 00 04 00 18 01 02 03 04 16
                                                                                长度 个数    类型   偏移量    V   位数 值、   校验码、结束符

读的字节数    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37
1F   DC 02 02 68 1B 1B 68 03 02 6C 32 01 00 00 02 02 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 03 20 92 16                                           68 16 16 68 02 03 08 32 03 00 00 02 02 00 02 00 05 00 00 04 01 FF 04 00 08 04 61 16
1    DC 02 02 68 1B 1B 68 03 02 7C 32 01 00 00 08 08 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 03 20 AE 16                                           68 16 16 68 02 03 08 32 03 00 00 04 04 00 02 00 05 00 00 04 01 FF 04 00 08 05 66 16
2F   DC 02 02 68 1B 1B 68 03 02 6C 32 01 00 00 02 02 00 0E 00 00 04 01 12 0A 10 02 00 02 00 01 84 00 03 20 93 16                                           68 17 17 68 02 03 08 32 03 00 00 02 02 00 02 00 06 00 00 04 01 FF 04 00 10 02 00 68 16
2    DC 02 02 68 1B 1B 68 03 02 7C 32 01 00 00 09 09 00 0E 00 00 04 01 12 0A 10 02 00 02 00 01 84 00 03 20 B1 16                                           68 17 17 68 02 03 08 32 03 00 00 09 09 00 02 00 06 00 00 04 01 FF 04 00 10 02 00 76 16
                        0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29
*/


//附上反复发E5的处理监视
/*
DC 04 04 68 1B 1B 68 02 04 6C 32 01 00 00 02 02 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 70 16 E5 10 02 04 5C 62 16
E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16
E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16
E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16
E5 10 02 04 7C 82 16 E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16
E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16
E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16
E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 6 E5 10 02 04 7C 82 16
E5 DC 04 04 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16
E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 02 04 5C 62 16 E5 10 02 04 7C 82 16 E5 10 00 04 49 4D 16 DC
*/

//附上出02号错误时PLC的处理方法
/*
DC 04 04 68 1B 1B 68 02 04 6C 32 01 00 00 02 02 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 70 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 6C 32 01 00 00 02 02 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 70 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 6C 32 01 00 00 02 02 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 70 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 03 03 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 62 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 03 03 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 62 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 03 03 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 62 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 04 04 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 64 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 04 04 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 64 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 04 04 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 64 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 05 05 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 66 16 10 04 02 02 08 16
10 00 04 49 4D 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 05 05 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 66 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 05 05 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 66 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 06 06 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 68 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 06 06 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 68 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 06 06 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 68 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 07 07 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6A 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 07 07 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6A 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 07 07 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6A 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 08 08 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6C 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 08 08 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6C 16 10 04 02 02 08 16
10 01 04 49 4E 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 08 08 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6C 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 09 09 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6E 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 09 09 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6E 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 09 09 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 6E 16 10 04 02 02 08 16
DC 04 04 68 1B 1B 68 02 04 5C 32 01 00 00 0A 0A 00 0E 00 00 04 01 12 0A 10 02 00 01 00 01 84 00 00 00 70 16 10 04 02 02 08 16
DC 04 04
*/
