#ifndef RECEIPTPAINTER_H
#define RECEIPTPAINTER_H

#include <QPaintEvent>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPainter>
#include <QWidget>
#include <QList>

#include <QString>
#include <QChar>
#include <QVector>
//#include "orderdetailbean.h"
//#include "ordergoodsbean.h"

class ReceiptPainter : public QPainter
{
public:
    ReceiptPainter(QPrinter*,QString);
    ~ReceiptPainter();
    void doCustomerPrint();
    QString getSeperator(double);
private:
    //下面的数字单位是 像素
    // 字体大小
    enum {FONT_SIZE_NORMAL = 10,FONT_SIZE_MIDDLE = 12,FONT_SIZE_BIG = 20};  //正常、中等、大

    //预留尺寸
    enum {TOTAL_LENGTH = 140,GOODS_LENGTH = 40,GOODS_NUM_LENGTH = 40}; //小票每一行的可以打印的位数、商品名称的预留长度、商品数量的预留长度

    //OrderDetailBean* orderDetail;  //订单的详细信息：时间、编号、总价、收款、找零，以及订单中每样商品的详细信息：名称、数量、单价
    QString shopName;

};

#endif // RECEIPTPAINTER_H
