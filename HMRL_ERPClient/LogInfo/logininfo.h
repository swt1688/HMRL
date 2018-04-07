#ifndef LOGININFO_H
#define LOGININFO_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinterInfo>
#include <QTextDocument>

#include "helperclass.h"

namespace Ui {
class LoginInfo;
}

class LoginInfo : public QDialog
{
    Q_OBJECT

public:
    explicit LoginInfo(QWidget *parent = 0);
    ~LoginInfo();

public slots:

    void InitInStroage();
private slots:


    void CalculatePrice(const QString &);
    void LoadData();
    void on_btnSaveOutRecord_clicked();
    void OutContent();
    void ShowTips();
    void PrintPage();
    void OnbtnPrintClicked();
    void on_btnUpdate_clicked();
    bool eventFilter(QObject *obj, QEvent *event);


private:
    Ui::LoginInfo *ui;

    double totalAmount;
    int CurrentCount;
    float CurrentSum;

    QTextCursor cursor;
    QTextTable *orderTable;
    QTextFrame *topFrame;
    QTextCharFormat fmt2;

    int Outting;
    int rows;
     QSqlQuery query;
     QSqlQueryModel *modelShow;
};

#endif // LOGININFO_H
