#include "printview.h"

#include <QWidget>
#include <QAbstractItemModel>
#include <QTableView>
#include <QPrintPreviewDialog>
#include <QDebug>

PrintView::PrintView()
{

}




/* 说明:打印辅助操作类实现文件
 * 功能:打印数据,导出数据到PDF
 * 作者:史文涛
 * 时间:2016-10-29  检查:2016-10-29
 */
void PrintView::Print(QString title,int columnCount,QStringList content,QWidget *parent)
{
        QStringList html;
        int rowCount=content.count();//计算总行数

        //表格开始
        html.append("<table border='1' style='margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;' cellspacing='0' cellpadding='5'>");

        //标题占一行,居中显示
        html.append("<tr><td colspan='");
        html.append(QString::number(columnCount));
        html.append("' style='vertical-align:middle;'><p align='center'>");
        html.append(title);
        html.append("</p></td></tr>");

        //循环构建数据
        for(int i=0;i<rowCount;i++)
        {
            QStringList value=content[i].split(";");
            QString temp="";

            temp +="<tr>";

            for(int i=0;i<columnCount;i++)
            {
                //构建列
                temp+="<td width=5%>";
                temp+=value[i];
                temp+="</td>";
            }

            temp+="</tr>";
            html.append(temp);
        }

        html.append("</table>");

        //调用打印机打印
        QPrinter print(QPrinter::HighResolution);

        //询问是否要打印


        print.setOutputFormat(QPrinter::NativeFormat);
        print.setPageSize(QPrinter::A4);

        //QPrintPreviewDialog Preview(&print,parent);
        //Preview.exec();

        QPrintDialog printDialog(&print,parent);
        if(printDialog.exec())
        {
            QTextDocument txt;
            txt.setHtml(html.join(""));
            txt.print(&print);
        }

}

void PrintView::PrintPDF(QString fileName,QStringList title,QStringList content,QWidget *parent)
{
        QStringList html;
        int columnCount=content.at(0).split(";").count();
        int rowCount=content.count();//计算总行数
        html.append("<p align=center><FONT SIZE=5>"+title.at(0)+"</FONT></p>");
        html.append("<p align=center><FONT SIZE=5>污染源环境质量报表</FONT></p>");
        //表格开始
        html.append("<div align= Left> <p align=center>&nbsp;</p>");
        html.append("<table border='3' style='table-layout:fixed;' cellspacing='1' cellpadding='3'>");

        //标题占一行,居中显示
        html.append(QString("<tr><td colspan='%1'style='vertical-align:middle;'><p align='center'>").arg(columnCount));

        html.append(title.at(1));
        html.append("</p></td></tr>");

        //循环构建数据
        for(int i=0;i<rowCount;i++)
        {
            QStringList value=content[i].split(";");
            QString temp="";

            temp +="<tr>";
            for(int i=0;i<value.count();i++)
            {
                //构建列
                temp+=QString("<td width=10%>"+value[i]+"</td>");
            }


//            temp+="<td width='"+QString::number(columnWidths[4])+"'>";
//            temp+=columnNames[4]+":"+value[4];
//            temp+="</td>";

            temp+="</tr>";
            html.append(temp);
        }
        html.append("</table> <p align=center>&nbsp;</p> </div>");

        html.append("<p>制表人：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>审核人：<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>审核时间：</p>");

        //调用打印机导出PDF
        QPrinter print(QPrinter::HighResolution);;
        print.setOutputFileName(fileName);
        print.setOutputFormat(QPrinter::PdfFormat);
        if(columnCount>10)
        {
            print.setOrientation( QPrinter::/*Portrait*/ Landscape );
        }
        else
        {
            print.setOrientation( QPrinter::Portrait );
        }

        print.setPageMargins(1, 1, 1, 1, QPrinter::Millimeter);
        print.setPageSize(QPrinter::A4);

        QTextDocument txt(parent);
        txt.setHtml(html.join(""));
        txt.print(&print);
}


//计算行页数，及每页起始行号及区域高度
//void PrintView::CalcultPrintAreaRow(int h, int &pageCount, QVector<int> &printAreaStartRow, QVector<int> &printAreaHeight)
// {
//     int pix_height = 0;

//     QAbstractItemModel *item_model = this->model();
//     if (item_model == NULL) return;

//     QHeaderView *headerView = this->horizontalHeader ();
//     if (headerView != NULL) pix_height = headerView->height();

//     pageCount = 0;

//    int i;

//    printAreaStartRow.push_back(0);
//    for (i = 0; i < item_model->rowCount(); )
//     {
//         if ((pix_height + rowHeight(i)) > h)
//         {
//             printAreaStartRow.push_back(i);
//             printAreaHeight.push_back(pix_height);

//             pix_height = headerView->height();

//             pageCount ++;

//             continue;
//         }
//        pix_height += rowHeight(i);
//        i++;

//    }

//if (pix_height <= h)
//{
//    printAreaStartRow.push_back(i);
//    printAreaHeight.push_back(pix_height);

//    pageCount ++;
// }

//}
////计算列页数及每页起始列号及打印区域宽度
//void PrintView::CalcultPrintAreaCol(int w, int &pageCount, QVector<int> &printAreaStartCol, QVector<int> &printAreaWidth)
//{
//    int pix_width = 0;

//    QAbstractItemModel *item_model = this->model();
//     if (item_model == NULL) return;

//    QHeaderView *headerView = this->verticalHeader ();
//     if (headerView != NULL) pix_width = headerView->width();

//    pageCount = 0;

//    int i;
//     printAreaStartCol.push_back(0);
//     for (i = 0; i < item_model->columnCount(); )
//     {
//         if ((pix_width + columnWidth(i)) > w)
//         {
//             printAreaStartCol.push_back(i);
//             printAreaWidth.push_back(pix_width);

//            pix_width = headerView->width();

//            pageCount ++;

//            continue;
//         }

//        pix_width += columnWidth(i);

//        i++;
//     }

//    if (pix_width <= w)
//    {
//         printAreaStartCol.push_back(i);
//         printAreaWidth.push_back(pix_width);

//         pageCount ++;
//     }
//}

////计算整个页面宽度
//int PrintView::CalcultTableViewWidth()
// {
//    int pix_width = 0;

//    QAbstractItemModel *item_model = this->model();
//     if (item_model == NULL) return 0;

//    QHeaderView *headerView = this->verticalHeader ();
//     if (headerView != NULL) pix_width = headerView->width();

//    for (int i = 0; i < item_model->columnCount(); i++)
//    {
//        pix_width += columnWidth(i);
//    }

//    return pix_width;
// }
////计算整个页面高度
//int PrintView::CalcultTableViewHeight()
//{
//    int pix_height = 0;

//    QAbstractItemModel *item_model = this->model();
//     if (item_model == NULL) return 0;

//    QHeaderView *headerView = this->horizontalHeader ();
//     if (headerView != NULL) pix_height = headerView->height();

//    for (int i = 0; i < item_model->rowCount(); i++)
//    {
//        pix_height += rowHeight(i);
//    }

//    return pix_height;
// }

//void PrintView::print(QPrinter *printer)
//{
//    #ifndef QT_NO_PRINTER
//     resize(printer->width(), printer->height());

//    int marginLeft = 20, marginTop = 20, marginRight = 20, marginBottom = 20;

//    QRect paperRect = printer->paperRect();
//    paperRect.adjust(0, 0, - marginLeft - marginRight, - marginTop - marginBottom);

//    int i,j,k;
//    int row_pageCount = 1, col_pageCount = 1;
//    QVector<int> printAreaStartRow, printAreaHeight;
//    QVector<int> printAreaStartCol, printAreaWidth;

//    CalcultPrintAreaRow(paperRect.height(), row_pageCount, printAreaStartRow, printAreaHeight);
//    CalcultPrintAreaCol(paperRect.width(), col_pageCount, printAreaStartCol, printAreaWidth);

//    //限制打印页数, 处理实际打印页(根据需要自己处理，判断有效打印区域)
//    CalcultTableViewPage(printAreaStartRow, printAreaStartCol, row_pageCount, col_pageCount);

//    QPainter painter(printer);

//    painter.begin(printer);

//    //隐藏行列很重要，慢慢体会吧
//    for (i = 0; i < row_pageCount; i ++)
//    {

//        for (k = 0; k < this->model()->columnCount(); k++)
//        {
//            setColumnHidden(k, false); //重新把列显示出来
//        }

//        for (j = 0; j < col_pageCount; j++)
//        {
//            resize(printAreaWidth[j], printAreaHeight[i]);

//            render(&painter, QPoint(marginLeft, marginTop), QRegion(0, 0, printAreaWidth[j], printAreaHeight[i]), QWidget::DrawChildren); //绘制到指定区域

//            //隐藏列
//            for (k = printAreaStartCol[j]; k < printAreaStartCol[j + 1]; k++)
//            {
//                setColumnHidden(k, true);
//            }

//            if (i != (row_pageCount - 1) ||j != (col_pageCount - 1))
//            {
//                printer->newPage(); //创建新页
//            }
//        }

//        //隐藏行
//        for (k = printAreaStartRow[i]; k < printAreaStartRow[i + 1]; k++)
//        {
//            setRowHidden(k, true);
//        }
//     }

//    painter.end();

//    #endif

//}
