#include "tablemodel.h"
#include "tableview.h"
//#include "SystemMaintain/baseinfo.h"

#include <QDateTimeEdit>
#include <QDebug>
#include <QSqlQuery>


TableModel::TableModel(QObject *parent) :
QAbstractTableModel(parent)
{
}

TableModel::~TableModel()
{

}


int TableModel::rowCount(const QModelIndex &parent) const
{
    int i=m_data.size();
    return i;
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    int j=m_HorizontalHeader.count();
    return j;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())    return QVariant();
    if (role == Qt::DisplayRole)
    {
        int ncol = index.column();
        int nrow = index.row();
        QStringList values = m_data.at(nrow);
        if (values.size() > ncol)
        return values.at(ncol);
        else
        return QVariant();
    }
    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    return Qt::NoItemFlags;

    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

     flag|=Qt::ItemIsEditable; // 设置单元格可编辑,此处注释,单元格无法被编辑
    return flag;
}

void TableModel::setHorizontalHeader(const QStringList &headers)
{
    m_HorizontalHeader = headers;
}


QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return m_HorizontalHeader.at(section);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void TableModel::setData(const QVector<QStringList> &data)
{
    m_data = data;
}

/********************************************************************************
 * QSqlQueryModel这个模型默认是只读的，以下类改变模型为可读写
 *
 ********************************************************************************/
MySqlQueryModel::MySqlQueryModel(QString str1 ,QString str2,int low,int up)
{
    tableName=str1;
    strCondition=str2;
    lowLimt=low;
    upLimt=up;

    setQuery("SELECT * FROM "+tableName+" where "+strCondition+" ;");

    int i=0;
    for(i=0;i<columnCount();i++)
    {
        ModifyList<<headerData(i,Qt::Horizontal).toString();
        //qDebug()<<"表头"<<headerData(i,Qt::Horizontal)<<ModifyList.at(i);
    }

}
//MySqlQueryModel::~MySqlQueryModel()
//{

//}
Qt::ItemFlags MySqlQueryModel::flags(
        const QModelIndex &index) const //返回表格是否可更改的标志
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if ((index.column()>= lowLimt)&&(index.column() <= upLimt)) //第二个属性可更改
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool MySqlQueryModel::setData(const QModelIndex &index, const QVariant &value, int )
{
    if (index.column() < 1)
        return false;
    QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
    int id = data(primaryKeyIndex).toInt(); //获取id号
    clear();
    bool ok;

    ok = setName(id,  ModifyList.at(index.column()),value.toString());//存储在表格中修改的内容

    refresh(tableHeardList);
    return ok;
}
bool MySqlQueryModel::setName(int ID,const QString ColumnName, const QString &name) //添加name属性的值
{
    QSqlQuery query;
    query.prepare("update "+tableName+" set `"+ColumnName+"` = ? where ID = ?");
    query.addBindValue(name);
    query.addBindValue(ID);
    return query.exec();
}
void MySqlQueryModel::refresh(QStringList strList) //更新显示
{
    tableHeardList=strList;
    setQuery("SELECT * FROM "+tableName+" where "+strCondition+" ;");

    int i=0;
    for(i=0;i<strList.count();i++)
    {
        setHeaderData(i,Qt::Horizontal,strList.at(i));
    }
}

QVariant MySqlQueryModel::data(const QModelIndex &index, int role) const
        //更改数据显示样式
{
    QVariant value = QSqlQueryModel::data(index, role);

    if (role == Qt::TextColorRole && index.column() == 0)
        return qVariantFromValue(QColor(Qt::red)); //第一个属性的字体颜色为红色
    return value;

}

