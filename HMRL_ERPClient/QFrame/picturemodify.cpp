#include "picturemodify.h"
#include "ui_picturemodify.h"

#include <QFileDialog>
#include "logoin.h"

PictureModify::PictureModify(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PictureModify)
{
    ui->setupUi(this);
    this->setAttribute (Qt::WA_DeleteOnClose);//QWidget及其派生类的对象，设置QT::WA_DeleteOnClose,当close时会析构该对象


}

PictureModify::~PictureModify()
{
    delete ui;
}

void PictureModify::on_btnSearch_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(nullptr, QStringLiteral("选择图片"), ".", "*.*;;*.jpg;;*.png;;");

    if (filepath.trimmed().isEmpty() == false)
    {
        ui->widget->LoadPicture(filepath);

    }
    ui->lineEdit->setText(filepath);

}

void PictureModify::on_btnOk_clicked()
{
    QString filename;
    filename = QFileDialog::getSaveFileName(this,tr("保存图像"),tr("%1").arg(Logoin::AppPath),tr("*.jpg;; *.png;;"));
    ui->widget->SavePicture(filename);

}
