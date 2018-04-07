#ifndef VERIFYCHECKSUM_H
#define VERIFYCHECKSUM_H

#include <QByteArray>

class VerifyChecksum
{
public:
    VerifyChecksum();

    //unsigned int CRC16(unsigned char *puchMsg, unsigned int usDataLen);
    static QByteArray gemfieldCRC(QByteArray gemfield);
    static unsigned int CRC16(const QByteArray &puchMsg);
    static quint16 crc16ForModbus(const QByteArray &data);
    static quint16 crc16ForX25(const QByteArray &data);
    static quint32 crc32(const QByteArray &data);

};

#endif // VERIFYCHECKSUM_H
