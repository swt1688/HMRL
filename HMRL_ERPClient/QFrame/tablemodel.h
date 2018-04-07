#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QSqlQueryModel>
#include <QItemDelegate>
//#include <QSqlTableModel>
#include <QStandardItemModel>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

    void setData(const QVector<QStringList>& data) ;
    QVector<QStringList>& DataVector() {return m_data;}
    void setHorizontalHeader(const QStringList& headers);
    ~TableModel(void);

signals:

public slots:


private:
    QStringList m_HorizontalHeader;
    QVector<QStringList> m_data;
};

//QSqlQueryModel这个模型默认是只读的，以下类改变模型为可读写
class MySqlQueryModel : public QSqlQueryModel
{
public:
    MySqlQueryModel(QString str1="",QString str2="",int low=0,int up=0);
    //~MySqlQueryModel();
    //下面三个函数都是虚函数,我们对其进行重载
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    void refresh(QStringList strList);
    //
private:
    int lowLimt,upLimt;
    QString tableName,strCondition;
    QStringList ModifyList;
    QStringList tableHeardList;

    bool setName(int ID,const QString ColumnName, const QString &name);

};

#endif // TABLEMODEL_H
