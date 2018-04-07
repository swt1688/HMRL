#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QWidget>
#include "myCalendar/DefDateTimeEdit.h"
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QStackedWidget>

#include "QFrame/navlistview.h"



namespace Ui {
class RemoteControl;
}

class RemoteControl : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteControl(QWidget *parent = 0);
    ~RemoteControl();



private slots:
    void Init();
    void TitleInit();
    void searchRecord();
    void listView_pressed();
    void searchStorageInfo();
    void searchPriceInfo();
    void CreateReport();

private:
    Ui::RemoteControl *ui;

    //page1
    QLabel *labSearchType;
    QLabel *labStartTime;
    QLabel *labEndTime;
    QStringList searchTypeList;
    QString CurrentListSelect;
    DefDateTimeEdit* m_defStartDateTime;
    DefDateTimeEdit* m_defEndDateTime;
    QComboBox *cboxSearchType;
    QTableView *tableView;
    QPushButton *btnSearch;

    //page2
    QLabel *labSearchReport;
    QLabel *labStartTimeReport;
    QComboBox *cboxSearchReport;
    QPushButton *btnSearchReport;
    QDateEdit *dayEdit;//日数据

    QWidget *widget;
    NavListView *listView1;
    QStackedWidget *stackedWidget;

    QSqlQuery query;
    QSqlQueryModel *modelShow;


};

#endif // REMOTECONTROL_H
