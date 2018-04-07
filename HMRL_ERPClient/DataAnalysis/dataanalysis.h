#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <QWidget>

#include <QSqlQuery>
#include <QSqlQueryModel>
//#include <QSqlRecord>
#include <QSqlResult>

namespace Ui {
class DataAnalysis;
}

class DataAnalysis : public QWidget
{
    Q_OBJECT

public:
    explicit DataAnalysis(QWidget *parent = 0);
    ~DataAnalysis();

public slots:
    void InitInStroage();
private slots:


    void CalculatePrice(const QString &);
    void LoadData();
    void on_pushButton_clicked();

    void on_btnUpdate_clicked();

private:
    Ui::DataAnalysis *ui;
    double totalAmount;
    float CurrentSum;
    int CurrentCount;

     QSqlQuery query;
     QSqlQueryModel *modelShow;
};

#endif // DATAANALYSIS_H
