#include "remindinterface.h"
#include "ui_remindinterface.h"

RemindInterface::RemindInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemindInterface)
{
    ui->setupUi(this);
}

RemindInterface::~RemindInterface()
{
    delete ui;
}
