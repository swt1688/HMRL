#include "receiptpainter.h"

#include <QByteArray>
#include <QTextCodec>
#include <QDebug>
#include <QRect>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QDateTime>


ReceiptPainter::ReceiptPainter(QPrinter* printer,QString sn):QPainter(printer)
{

    shopName = sn;
    doCustomerPrint();

}


QString ReceiptPainter::getSeperator(double width){
    int per = 12;  //360pt可以打30个*
    int cnt = width / per;
    QString sep;
    for(int i = 0; i < cnt; i++) sep.append("* ");
    return sep;
}

void ReceiptPainter::doCustomerPrint(){
    double rt = 0.38;
    QFont font1("宋体",FONT_SIZE_NORMAL,QFont::Bold);
    QFont font2("宋体",FONT_SIZE_NORMAL);
    QFont font3("宋体",FONT_SIZE_NORMAL,QFont::Bold);
    QFont font4("宋体",FONT_SIZE_MIDDLE);

    int xx1 = 50 * rt;
    int xx2 = 410 * rt;
    int xx3 = 545 * rt;

    //w1 w2 w3 分别是品名、数目、单价的所在栏占的宽度
    int w1 = 360 * rt;
    int w2 = 135 * rt;
    int w3 = 150 * rt;

    //h1 h2 分别适用于两种字体大小
    int h1 = 48 * rt;
    int h2 = 35 * rt;
    int h3 = 60 * rt;

    QString seperator = getSeperator(w1+w2+w3);

    //打印店名
    QRect rect(xx1,0,w1+w2+w3,h1);
    this->setFont(font1);
    this->drawText(rect,Qt::AlignCenter,shopName);

    //打印订单号和时间
    rect.setY(rect.y()+rect.height());
    rect.setHeight(h2);
    this->setFont(font2);
    this->drawText(rect,QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    rect.setY(rect.y()+rect.height());
    rect.setHeight(h2);
    this->drawText(rect,"堂食: 湖北省武汉市第一人民食堂");
    //分隔符
    rect.setY(rect.y()+rect.height());
    rect.setHeight(h2);

    this->drawText(rect,seperator);

    //品名                数量 单价
    this->setFont(font3);
    rect.setY(rect.y()+rect.height());
    rect.setHeight(h2);
    rect.setX(xx1);
    rect.setWidth(w1);
    this->drawText(rect,Qt::AlignLeft,"品名");
    rect.setX(xx2);
    rect.setWidth(w2);
    this->drawText(rect,Qt::AlignLeft,"数量");
    rect.setX(xx3);
    rect.setWidth(w3);
    this->drawText(rect,Qt::AlignLeft,"单价");

    //打印商品
    this->setFont(font2);

    for (int i = 0; i < 5; i++) {

        QString nname = "猪小明%1";
        QString num = "12.%1";
        QString price = "33.%1";
        rect.setY(rect.y()+rect.height());
        rect.setHeight(h2);
        rect.setX(xx1);
        rect.setWidth(w1);
        this->drawText(rect,Qt::AlignLeft,nname);
        rect.setX(xx2);
        rect.setWidth(w2);
        this->drawText(rect,Qt::AlignLeft,num);
        rect.setX(xx3);
        rect.setWidth(w3);
        this->drawText(rect,Qt::AlignLeft,"￥" + price);
    }

    //分隔符
    rect.setY(rect.y()+rect.height());
    rect.setHeight(h2);
    rect.setX(xx1);
    rect.setWidth(w1+w2+w3);
    this->drawText(rect,seperator);

    //打印总价、付款、找零信息
    QString tp = "25.23";
    QString pm = "32.56";
    QString cg = "18.88";
    this->setFont(font4);
    rect.setY(rect.y()+rect.height());
    rect.setHeight(h1);
    rect.setX(xx1);
    rect.setWidth(w1+w2);
    this->drawText(rect,Qt::AlignLeft,"共计：");
    rect.setX(xx3);
    rect.setWidth(w3);
    this->drawText(rect,Qt::AlignLeft,tp);

    rect.setY(rect.y()+rect.height());
    rect.setHeight(h1);
    rect.setX(xx1);
    rect.setWidth(w1+w2);
    this->drawText(rect,Qt::AlignLeft,"已收款：");
    rect.setX(xx3);
    rect.setWidth(w3);
    this->drawText(rect,Qt::AlignLeft,pm);

    rect.setY(rect.y()+rect.height());
    rect.setHeight(h1);
    rect.setX(xx1);
    rect.setWidth(w1+w2);
    this->drawText(rect,Qt::AlignLeft,"找零：");
    rect.setX(xx3);
    rect.setWidth(w3);
    this->drawText(rect,Qt::AlignLeft,cg);


    //分隔符
    this->setFont(font2);
    rect.setY(rect.y()+rect.height());
    rect.setHeight(h2);
    rect.setX(xx1);
    rect.setWidth(w1+w2+w3);
    this->drawText(rect,seperator);

}

ReceiptPainter::~ReceiptPainter()
{

}
