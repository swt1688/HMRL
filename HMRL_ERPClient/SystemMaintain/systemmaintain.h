#ifndef SYSTEMMAINTAIN_H
#define SYSTEMMAINTAIN_H

#include <QWidget>
#include <QSqlQuery>
#include <QSqlQueryModel>

namespace Ui {
class SystemMaintain;
}

class SystemMaintain : public QWidget
{
    Q_OBJECT

public:
    explicit SystemMaintain(QWidget *parent = 0);
    ~SystemMaintain();

private slots:
    void buttonClick();

    void LoadUserData();
    void LoadColdData();
    void LoadProductData();
    void LoadProductPriceData();
    void LoadCustomerData();
    void on_btnAdd_clicked();

    void on_btnRefresh_clicked();

    void on_btnColdAdd_clicked();

    void on_btnColdRefresh_clicked();

    void on_btnAddProduct_clicked();

    void on_btnUpdataPrice_clicked();

    void on_btnAddCustomer_clicked();

private:
    Ui::SystemMaintain *ui;

    QSqlQuery query;
    QSqlQueryModel *modelShow;


};

#endif // SYSTEMMAINTAIN_H
