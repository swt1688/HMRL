#include "logininfo.h"
#include "ui_logininfo.h"

#include <QtWidgets>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <Qt3DInput/QMouseEvent>
#include <QTextFormat>




#include "logoin.h"

LoginInfo::LoginInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginInfo)
{
    ui->setupUi(this);

    ui->cboxCustomer->installEventFilter(this);//安装监听器监听密码框回车键//ui->txtPassword->
    modelShow = new QSqlQueryModel(this);
    connect(ui->cboxProductName,&QComboBox::currentTextChanged,[=](){
            QStringList ProductLevelList;
            QString str=tr("SELECT distinct SpecificationLevel FROM `hmrl_erp`.storagerecordtable where ProductName='%1';").arg(ui->cboxProductName->currentText());
            if(query.exec(str)){while(query.next()) {ProductLevelList<<query.value(0).toString();}}
             ui->cboxSpecificationLevel->clear(); ui->cboxSpecificationLevel->addItems(ProductLevelList);
             CalculatePrice(ui->txtPrice->text());
            });
    connect(ui->cboxSpecificationLevel,&QComboBox::currentTextChanged,this,&LoginInfo::CalculatePrice);

    connect(ui->txtWeight,&QLineEdit::textChanged,this,&LoginInfo::CalculatePrice);
    connect(ui->txtPrice,&QLineEdit::textChanged,this,&LoginInfo::CalculatePrice);
    connect(ui->cboxColdStoreName,&QComboBox::currentTextChanged,[=](){
        QStringList ColdSNList;
        QString str=tr("SELECT  ColdStoreSN FROM `hmrl_erp`.coldstoretable where ColdStoreName='%1';").arg(ui->cboxColdStoreName->currentText());
        if(query.exec(str)){while(query.next()){ColdSNList<<query.value(0).toString();}}
        ui->cboxColdStoreSN->clear();ui->cboxColdStoreSN->addItems(ColdSNList);
    });

    connect(ui->btnPrint,&QPushButton::clicked,this,&LoginInfo::OnbtnPrintClicked);

    ui->tableView->setAlternatingRowColors(true);//隔行变色,以下是视觉方面的效果，不加也没影响,把表格的背景调成黄蓝相间
    /********setSectionResizeMode(ResizeMode mode)函数，ResizeMode接口枚举参数*******************
     * 1、QHeaderView::Interactive 用户可以重新调整表头的大小，也可以使用resizeSection()重新调整表头的大小。
     * 2、QHeaderView::Fixed 用户不可以重新调整表头的大小，只可以使用resizeSection()重新调整表头的大小。
     * 3、QHeaderView::Stretch 表头将会调整单元格到可得的空间。用户或者程序员通过代码都不能改变它的大小。
     * 4、QHeaderView::ResizeToContents 表头将自动根据整个行或者列的内容去调整表头单元格到最佳的大小。用户或者程序员通过代码都不能改变它的大小。
     * ******************************************************************************/
    /*****************设置第一列为固定60宽度，其他自适应 *************************/

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(100);
    //tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setObjectName("hHeader");

    ui->textBrowser->setStyleSheet(QString::fromUtf8("background-color: rgb(22, 0, 33);"));
    ui->textBrowser->setTextColor(QColor("green"));
    ui->textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->textBrowser->setLineWrapMode(QTextEdit::WidgetWidth);//自动换行
    //ui->textBrowser->setFixedWidth(420);

    Outting=0;//当前正在出库

    OutContent();
    InitInStroage();

}

LoginInfo::~LoginInfo()
{
    delete ui;
}

void LoginInfo::CalculatePrice(const QString &)
{
    QString str=tr("SELECT UnitPrice FROM `hmrl_erp`.PorductPriceTable where ProductName='%1' and Quality='%2' order by UpdataTime desc limit 1;").arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText());
    qDebug()<<str;
    if(query.exec(str))
    {
        while (query.next())
        {
            ui->txtPrice->setText(query.value(0).toString());
        }
    }
    totalAmount=ui->txtWeight->text().toFloat()*ui->txtPrice->text().toFloat();
    ui->labTotalAmount->setText(tr("总价:%1元").arg(totalAmount, 14, 'f', 2));

    str=tr("SELECT Weight FROM `hmrl_erp`.currentstoragetable where ProductName='%1' and SpecificationLevel='%2' and Unit = '%3';").arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText()).arg(ui->cboxUnit->currentText());
    CurrentCount=0;
    if(query.exec(str))
    {
        CurrentCount=query.numRowsAffected();
        while (query.next())
        {
             CurrentSum=query.value(0).toFloat();
        }
    }

}
void LoginInfo::InitInStroage()
{
    QStringList ProductList;
    QString str;
    str="SELECT distinct ProductName FROM `hmrl_erp`.storagerecordtable;";
    if(query.exec(str))
    {
        while(query.next())
        {
            ProductList<<query.value(0).toString();
        }
    }
    qDebug()<<"产品列表"<<ProductList<<str;
    ui->cboxProductName->clear();
    ui->cboxProductName->addItems(ProductList);
    QStringList ColdStorageList;
    str="SELECT distinct ColdStoreName FROM `hmrl_erp`.coldstoretable;";
    if(query.exec(str))
    {
        while(query.next())
        {
            ColdStorageList<<query.value(0).toString();
        }
    }
    ui->cboxColdStoreName->clear();
    ui->cboxColdStoreName->addItems(ColdStorageList);
    QStringList MetagePersonList;
    str="SELECT UserName FROM `hmrl_erp`.userinfotable;";
    if(query.exec(str))
    {
        while(query.next())
        {
            MetagePersonList<<query.value(0).toString();
        }
    }
    ui->cboxMetagePerson->clear();
    ui->cboxMetagePerson->addItems(MetagePersonList);
    //加入客户信息
    str="SELECT CustomerName FROM `hmrl_erp`.customertable;";
    QStringList CustomerList;
    CustomerList<<tr("%1").arg("请选择客户")<<tr("%2").arg("个人");
    if(query.exec(str))
    {
        while(query.next())
        {
            CustomerList<<query.value(0).toString();
        }
    }
    ui->cboxCustomer->clear();
    ui->cboxCustomer->addItems(CustomerList);
    connect(ui->cboxCustomer,&QComboBox::currentTextChanged,[=](){ui->textBrowser->clear(); OutContent();
    qDebug()<<tr("当前客户序号%1").arg(ui->cboxCustomer->currentIndex());});
    LoadData();

}
void LoginInfo::LoadData()
{
    QString startTime,endTime;
    startTime=QDateTime::currentDateTime().date().toString("yyyy-MM-dd 00:00:00");
    endTime=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->label_12->setText(tr("今日出库情况:(出库人员:%1 %2--%3)").arg(Logoin::CurrentUserName).arg(startTime).arg(endTime));
    QString str;
    str=tr("SELECT DataTime,ProductName,ProductSN,SpecificationLevel,Unit,Weight,MetagePerson,ColdStoreName,ColdStoreSN,UnitPrice,TotalAmount,CustomerName FROM `hmrl_erp`.storagerecordtable where RecordPerson= '%1' and StroageType='出库' and DataTime between '%2' and '%3';")
            .arg(Logoin::CurrentUserName).arg(startTime).arg(endTime);
    modelShow->setQuery(str);
    qDebug()<<"出库人的货"<<str;
    int i=0;
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("出库时间"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("商品编号"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("规格等级"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单位"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("重量"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("称重人员"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("冷库名称"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("冷库编号"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("单价"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("总价"));
    //modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("出库人员"));
    modelShow->setHeaderData(i++,Qt::Horizontal,QObject::tr("客户名称"));

    //隔行变色,以下是视觉方面的效果，不加也没影响
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents ();//根据内容自动调整所有列的列宽  ui->tabUserInfo->resizeColumnToContents (0);
    //ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableView->setModel(modelShow);

}
void LoginInfo::on_btnSaveOutRecord_clicked()
{
    if(Outting==1)
    {
        Outting=0;
        ui->textBrowser->clear();
        OutContent();
    }
    if(ui->cboxCustomer->currentIndex()==0)
    {
        QMessageBox::critical(0,QObject::tr("当前未选择客户名称"),tr("请选择需要出库的客户名称"));
        return;
    }
    if(ui->txtWeight->text().isEmpty())
    {
        QMessageBox::critical(0,QObject::tr("重量为空值"),tr("请输入要出库的数量"));
        return;
    }

    if(CurrentSum<ui->txtWeight->text().toFloat())
    {
        QMessageBox::critical(0,QObject::tr("当前库存已不充足"),tr("当前库存：%1").arg(CurrentSum));
    }
    QString str=tr("INSERT INTO `hmrl_erp`.storagerecordtable ( `DataTime`, `ProductName`, `ProductSN`,`SpecificationLevel`,`Unit`,`Weight`,`MetagePerson`,`ColdStoreName`,`ColdStoreSN`,`UnitPrice`,`TotalAmount`,`StroageType`,`RecordPerson`,`CustomerName`) "
                   "VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10','%11','%12','%13','%14');")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(ui->cboxProductName->currentText())
                    .arg(ui->txtProductSN->text()).arg(ui->cboxSpecificationLevel->currentText()).arg(ui->cboxUnit->currentText())
                    .arg(ui->txtWeight->text()).arg(ui->cboxMetagePerson->currentText()).arg(ui->cboxColdStoreName->currentText()).arg(ui->cboxColdStoreSN->currentText())
                    .arg(ui->txtPrice->text()).arg(totalAmount).arg(tr("出库")).arg(Logoin::CurrentUserName).arg(ui->cboxCustomer->currentText());
    query.exec(str);
    qDebug()<<str;
    float sum = CurrentSum-ui->txtWeight->text().toFloat();
    double sumAmount=sum*ui->txtPrice->text().toFloat();

    str=tr("update `hmrl_erp`.currentstoragetable set ProductName='%1',SpecificationLevel='%2',Unit='%3',Weight='%4',UnitPrice='%5',TotalAmount='%6' where ProductName = '%7' and SpecificationLevel = '%8' and Unit = '%9';")
                .arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText())
                .arg(ui->cboxUnit->currentText()).arg(sum).arg(ui->txtPrice->text()).arg(sumAmount)
                .arg(ui->cboxProductName->currentText()).arg(ui->cboxSpecificationLevel->currentText()).arg(ui->cboxUnit->currentText());

    ui->cboxCustomer->setEnabled(false);


    int row = orderTable->rows();
    orderTable->insertRows(row, 1);
    cursor = orderTable->cellAt(row, 0).firstCursorPosition();
    cursor.insertText(ui->cboxProductName->currentText(), fmt2);
    cursor = orderTable->cellAt(row, 1).firstCursorPosition();
    cursor.insertText(ui->txtWeight->text(), fmt2);
    cursor = orderTable->cellAt(row, 2).firstCursorPosition();
    cursor.insertText(ui->cboxUnit->currentText(), fmt2);



    query.exec(str);


    qDebug()<<str;

    LoadData();


}

void LoginInfo::on_btnUpdate_clicked()
{
    InitInStroage();
}

void LoginInfo::OutContent()
{

    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Bold);
    fmt.setFontPointSize(13);
    fmt.setForeground(QColor("green"));// 前景色(即字体色)设为color色

    cursor=ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::Start);

    topFrame = cursor.currentFrame();
    QTextFrameFormat topFrameFormat = topFrame->frameFormat();
    topFrameFormat.setPosition(QTextFrameFormat::InFlow);
    topFrameFormat.setPadding(2);
    topFrame->setFrameFormat(topFrameFormat);

    cursor.insertFrame(topFrameFormat);
    ui->textBrowser->setAlignment(Qt::AlignHCenter);
    cursor.insertText(tr("湖北省麻城市惠民肉联有限公司"),fmt);
    cursor.insertBlock();
    cursor.insertText(tr("出库单"),fmt);
    cursor.insertBlock();
    cursor.insertBlock();

    fmt2.setFontWeight(QFont::Normal);
    fmt2.setFontPointSize(11);
    fmt2.setForeground(QColor("green"));// 前景色(即字体色)设为color色

    //插入表格
    QTextTableFormat orderTableFormat;
    orderTableFormat.setAlignment(Qt::AlignLeft);
    orderTableFormat.setCellPadding(8);
    orderTable = cursor.insertTable(1, 3, orderTableFormat);

    QTextFrameFormat orderFrameFormat = cursor.currentFrame()->frameFormat();
    orderFrameFormat.setBorder(0.2);
    orderFrameFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Ridge);

    cursor.currentFrame()->setFrameFormat(orderFrameFormat);
    cursor = orderTable->cellAt(0, 0).firstCursorPosition();
    cursor.insertText(tr("产品名称"), fmt2);
    cursor = orderTable->cellAt(0, 1).firstCursorPosition();
    cursor.insertText(tr("重量"), fmt2);
    cursor = orderTable->cellAt(0, 2).firstCursorPosition();
    cursor.insertText(tr("单位"), fmt2);

}
void LoginInfo::OnbtnPrintClicked()
{
    if(Outting==0)
    {
        cursor.setPosition(topFrame->lastPosition());
        cursor.insertBlock();
        cursor.insertText(tr("客户名称：%1").arg(ui->cboxCustomer->currentText()),fmt2);
        cursor.insertBlock();
        cursor.insertText(tr("出货时间：%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")),fmt2);
        cursor.insertBlock();
        cursor.insertText(tr("出货员：%1").arg(Logoin::CurrentUserName),fmt2);
        cursor.insertBlock();
    }

    ui->cboxCustomer->setEnabled(true);
    PrintPage();

}

bool LoginInfo::eventFilter(QObject *obj, QEvent *event)
{
    //用户按下鼠标左键,触发信号.
    if ((event->type() == QEvent::MouseButtonPress)&&(!ui->cboxCustomer->isEnabled()))
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            ShowTips();
            return true;
        }
    }
    return QObject::eventFilter(obj,event);
}

void LoginInfo::ShowTips()
{
    QMessageBox::critical(0,QObject::tr("友情提示"),tr("在一个客户全部出库完毕后才能更改客户"));
}
void LoginInfo::PrintPage()
{

    QPrinter printer(QPrinter::HighResolution);// 创建打印机对象


    //第一步
    printer.setPageSize(QPrinter::Custom);
    //第二步 设置大小的单位
    QSizeF size;
    size.setHeight(150);//单位mm   表示色带的长(对于PT-P700，实际最小打印是24.7)   567
    size.setWidth(100);//单位mm  表示色带的宽（标准规格）
    printer.setPaperSize(QSizeF(size),QPrinter::Millimeter);

    printer.setFullPage(true);//设置page和paper的尺寸一样
    printer.setPageMargins(3,3,3,3,QPrinter::Millimeter);
    printer.setOrientation(QPrinter::Portrait);
    //printer.setOutputFormat(QPrinter::NativeFormat);

    qDebug()<<ui->textBrowser->rect();

   QTextDocument *textDocument=ui->textBrowser->document();
   QSizeF size2;
   size2.setHeight(1000);
   size2.setWidth(600);
   textDocument->setPageSize(size2);
   textDocument->setDocumentMargin(30);

    // 创建打印预览对话框
     QPrintPreviewDialog preview(&printer, this);
     // 当要生成预览页面时，发射paintRequested()信号
     connect(&preview, &QPrintPreviewDialog::paintRequested,[=,this](QPrinter* printer){textDocument->print(printer);});
     preview.exec();

    QString fileName = QFileDialog::getSaveFileName(this, tr("导出PDF文件"), "20180405", "*.pdf");
   if (!fileName.isEmpty()) {



        // 如果文件后缀为空，则默认使用.pdf
        if (QFileInfo(fileName).suffix().isEmpty())
        fileName.append(".pdf");
        //QPrinter printer;
        // 指定输出格式为pdf
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        //ui->textEdit->print(&printer);
    }

/*
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    */
    Outting=1;
   textDocument->print(&printer);
}


