#include "aboutus.h"
#include "ui_aboutus.h"

#include "nbaseqrencodewidget.h"//二维码

AboutUs::AboutUs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutUs)
{
    ui->setupUi(this);
    ui->widget->setQrData("软件作者:史文涛\n\rQQ:5649742\n\rMobile:18672388461");
    ui->widget->setQrLogo(":tree.ico");
    ui->widget->resize(200,200);
    this->setWindowTitle(tr("关于污染源在线监控系统"));
}

AboutUs::~AboutUs()
{
    delete ui;
}

void AboutUs::on_btnClose_clicked()
{
    this->close();
}
