#include "exportexecl.h"

#include <QDebug>

QString ExportExecl::error;

//ExportExecl *ExportExecl::_instance = 0;
ExportExecl::ExportExecl(QObject *parent):QObject(parent)
{
}
//static *ExportExecl::Instance()
//{
//    if (!_instance)
//    {
//        _instance = new ExportExecl;
//    }
//    return _instance;
//}

bool ExportExecl::save(QString filePath, QStringList headers, QList<QStringList> data,QString comment)
{
    QString sheetName = "Sheet1";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC","excelexport");
    if( !db.isValid())
    {
        error="数据库驱动异常";
         return false;   //! type error
    }
    QString dsn = "DRIVER={Microsoft Excel Driver (*.xls)};"
     "DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\""+filePath+"\";DBQ="+filePath;
    db.setDatabaseName( dsn);
    // open connection
    if( !db.open())
    {
        error="无法打开数据库";
        return false;  //! db error
    }
    QSqlQuery query(db);
    QString sql;
    // drop the table if it's already exists
    sql = QString("DROP TABLE [%1]").arg(sheetName);
    query.exec( sql);
    //create the table (sheet in Excel file)
    sql = QString("CREATE TABLE [%1] (").arg(sheetName);
    foreach (QString name, headers)
    {
        sql +=QString("[%1] varchar(200)").arg(name);
        if(name!=headers.last())
        sql +=",";
    }
    sql += ")";
    query.prepare( sql);
    if( !query.exec())
    {
        ExportExecl::printError( query.lastError());
        db.close();
         return false;
    }
    foreach (QStringList slist, data)
    {
        insert(query,sheetName,slist);
    }
    if(!comment.isEmpty())
    {
        QStringList slist;
        slist<<comment;
        for(int i=0,n=headers.size()-1;i<n;i++)
        {
            slist<<"";
        }
        insert(query,sheetName,slist);
    }
    db.close();
    return true;
}

bool ExportExecl::saveFromTable(QString filePath,QTableView *tableView, QString comment)
{
    QAbstractItemModel* model=tableView->model();
    const int column=model->columnCount();
    const int row=model->rowCount();
    //header
    QStringList headers;

    for(int i=0;i<column;i++)
    {
        //隐藏列
        if(tableView->isColumnHidden(i))
        continue;
        headers<<model->headerData(i,Qt::Horizontal).toString();
    }
    //data
    QStringList list;
    QList<QStringList> data;
    for(int i=0;i<row;i++)
    {
        if(model->index(i,0).data().isNull())
        continue;
        list.clear();
        for(int j=0;j<column;j++){
        //隐藏列
         if(tableView->isColumnHidden(j))
         continue;
        list<<model->index(i,j).data().toString();
    }
    data<<list;
    }
    return ExportExecl::save(filePath,headers,data,comment);
}

void ExportExecl::printError(QSqlError error)
{
    QString sqlerr = error.text();
    error=sqlerr;
    qCritical()<<sqlerr;
}

bool ExportExecl::insert(QSqlQuery &query, QString sheetName, QStringList slist)
{
    QString sSql = QString("INSERT INTO [%1] VALUES(").arg( sheetName);
    for(int i=0,n=slist.size();i<n;i++)
    {
        sSql+=QString(":%1").arg(i);
        if(i!=n-1)
        sSql+=",";
        else
        sSql+=")";
    }
    query.prepare( sSql);
    for(int i=0,n=slist.size();i<n;i++)
    {
        query.bindValue(QString(":%1").arg(i),slist.at(i));
    }
    if( !query.exec())
    {
        printError( query.lastError());
        return false;
    }
    return true;
}

void ExportExecl::ToExcel(QString fileName, QString sheetName, QString title, QString columnNames[],int columnCount, QStringList content)
{
    //逐个拼接xml字符,再保存为xls文件
    QStringList html;

    //固定头部信息
    html.append("<?xml version=\"1.0\"?>\n");
    html.append("<?mso-application progid=\"Excel.Sheet\"?>\n");
    html.append("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
    html.append(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
    html.append(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n");

    //文档信息
    html.append(" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append("  <LastAuthor>污染源在线监测系统</LastAuthor>\n");
    html.append(QString("  <Created>%1</Created>\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    html.append("  <Version>11.6360</Version>\n");
    html.append(" </DocumentProperties>\n");

    html.append(" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append("  <AllowPNG/>\n");
    html.append(" </OfficeDocumentSettings>\n");

    html.append(" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("  <WindowHeight>29795</WindowHeight>\n");
    html.append("  <WindowWidth>21435</WindowWidth>\n");
    html.append("  <WindowTopX>120</WindowTopX>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <ProtectWindows>False</ProtectWindows>\n");
    html.append(" </ExcelWorkbook>\n");

    //样式表
    html.append(" <Styles>\n");
    html.append("  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n");
    html.append("   <Alignment ss:Vertical=\"Center\"/>\n");
    html.append("   <Borders/>\n");
    html.append("   <Font ss:FontName=\"宋体\" x:CharSet=\"134\" ss:Size=\"11\" ss:Color=\"#000000\"/>\n");
    html.append("   <Interior/>\n");
    html.append("   <NumberFormat/>\n");
    html.append("   <Protection/>\n");
    html.append("  </Style>\n");
    //标题样式
    html.append("  <Style ss:ID=\"s64\">\n");
    html.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
    html.append("   <Font ss:FontName=\"宋体\" x:CharSet=\"134\" ss:Size=\"14\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n");
    html.append("  </Style>\n");
    //带边框样式
    html.append("  <Style ss:ID=\"s91\">\n");
    html.append("  <Borders>\n");
    html.append("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("  </Borders>\n");
    html.append(" </Style>\n");

    html.append(" </Styles>\n");

    //工作表名称
    html.append(QString(" <Worksheet ss:Name=\"%1\">\n").arg(sheetName));
    int rowCount = content.count(); //计算总行数

    //表格开始
    html.append(QString("  <Table ss:ExpandedColumnCount=\"%1\" ss:ExpandedRowCount=\"%2\" x:FullColumns=\"1\"\n").arg(columnCount).arg(rowCount + 2));
    html.append("   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n");

    //设置字段宽度
    for (int i = 0; i < columnCount; i++) {
       // html.append(QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n").arg(QString::number(columnWidths[i])));
        html.append(QString("   <Column ss:AutoFitWidth=\"1\" />\n"));
    }

    //表格标题
    html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"26.25\">\n");
    html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s64\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(columnCount - 1).arg(title));
    html.append("   </Row>");

    //逐个添加字段名称
    html.append("   <Row ss:AutoFitHeight=\"0\">\n");
    for (int i = 0; i < columnCount; i++) {
        html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + columnNames[i] + "</Data></Cell>\n");
    }
    html.append("   </Row>\n");

    //逐个添加数据
    for (int i = 0; i < rowCount; i++) {
        html.append("   <Row ss:AutoFitHeight=\"0\">\n");
        QString temp = content[i];
        QStringList value = temp.split(";");
        foreach (QString str, value) {
            html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + str + "</Data></Cell>\n");
        }
        html.append("   </Row>\n");
    }
    html.append("  </Table>\n");

    //固定结尾格式
    html.append("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("   <PageSetup>\n");
    html.append("    <Header x:Margin=\"0.3\"/>\n");
    html.append("    <Footer x:Margin=\"0.3\"/>\n");
    html.append("    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n");
    html.append("   </PageSetup>\n");
    html.append("   <Unsynced/>\n");
    html.append("   <Selected/>\n");
    html.append("   <ProtectObjects>False</ProtectObjects>\n");
    html.append("   <ProtectScenarios>False</ProtectScenarios>\n");
    html.append("  </WorksheetOptions>\n");
    html.append(" </Worksheet>\n");
    html.append("</Workbook>\n");

    //写入文件
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream text(&file);
        text.setCodec("UTF-8");
        text << html.join("");
    }
}
