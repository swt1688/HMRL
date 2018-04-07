#ifndef PRINTVIEW_H
#define PRINTVIEW_H

#include <QPainter>
#include <QPrinter>
#include <QRect>
#include <QTextDocument>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QString>

class PrintView
{
public:
    PrintView();
    static void Print(QString title,int columnCount,QStringList content,QWidget *parent = Q_NULLPTR);
    static void PrintPDF(QString fileName,QStringList title,QStringList content,QWidget *parent = Q_NULLPTR);

};

//class PrintView
//{
//public:
//    PrintView();
//    void CalcultPrintAreaRow(int h, int &pageCount, QVector<int> &printAreaStartRow, QVector<int> &printAreaHeight);
//    void CalcultPrintAreaCol(int w, int &pageCount, QVector<int> &printAreaStartCol, QVector<int> &printAreaWidth);
//    int CalcultTableViewWidth();
//    int CalcultTableViewHeight();
//    void print(QPrinter *printer);

//};

#endif // PRINTVIEW_H
