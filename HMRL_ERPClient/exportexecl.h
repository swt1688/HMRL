#ifndef EXPORTEXECL_H
#define EXPORTEXECL_H

#include <QObject>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTableView>
#include <QtWidgets>
#include <QtGui>
#include <QtCore>

class ExportExecl : public QObject
{
    Q_OBJECT

public:

    explicit ExportExecl(QObject *parent = 0);
    // static ExportExecl *_instance;

    //static ExportExecl *Instance();
    //void OdbcExcel();
    //将数据保存为excel
    bool static save(QString filePath,QStringList headers,QList<QStringList> data,QString comment="");
    //将QTableView保存为excel
    bool static saveFromTable(QString filePath,QTableView *tableView,QString comment="");
    //获取错误信息
    QString static getError(){return error;}

    void ToExcel(QString fileName, QString sheetName,QString title, QString columnNames[],int columnCount, QStringList content);

    private:
    void static printError( QSqlError error);
    bool static insert(QSqlQuery& query, QString sheetName, QStringList slist);
    static QString error;
};
#endif // EXPORTEXECL_H
