#ifndef DATAINFO_H
#define DATAINFO_H

#include <QHostInfo>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QTimer>

#include "DataInfo/mapwidget.h"



namespace Ui {
class DataInfo;

}

class DataInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DataInfo(QWidget *parent = 0);
    ~DataInfo();

    /****************************************************************
     * 关联存储容器中存储的一般是二元组，即键值对。QT提供两种关联容器类型：QMap和QHash。
     * 1、QMap保存一个根据主键排序的链表（主键，值），QMap是一个以升序键顺序存储键值对的数据结构，QMap原型为QMap模板。
     * 提供 O(log n)的查找时间常数。 QMap内部使用的是一个跳表的数据结构， 与红黑树提供类似的算法复杂度和性能。早期版本的 QMap 用的是红黑树，但是跳表代码简洁、节省内存。
     * QMap提供了keys()和values()函数，可以获得键的集合和值的集合。这两个集合都是使用QList作为返回值的。
     * QMultiMap继承自QMap，允许一个key索引多个value
     * QList QMap::uniqueKeys () const  ---->返回map中所有唯一的key的链表
     * int QMultiMap::count () const    ---->返回map中键为key的数量
     *
     * QHash<Key, T>是个在哈希表中存储键值对的结构。QHash原型类类模板QHash。它的接口几乎和QMap相同，QHash的查找速度比QMap快很多，并且存储是不排序的。
     * QHash中的键值对在内部无序排列,QHash为它的内部哈希表自动分配最初的存储区域，并在有项被插入或者删除时重新划分所分配的区域大小。
     * 也可以调用reserve()或者squeeze()来指定或者压缩希望存储到哈希表的项的数目，以进行性能调整。
     * 通常的做法是利用我们预期的最大的项的数目来调用reserve()，然后插入数据，最后如果有多出的项，则调用squeeze()以使内存减到最小。
     * ***************************************************************/


signals:
        void sigLookUpHostResult(bool networkStatus);
public slots:

    void checkNetWorkOnline();
    void onLookupHost(QHostInfo host);
    void datapross();

private slots:


private:
    Ui::DataInfo *ui;
    QSqlQueryModel *modelShow;
    QSqlQuery query;
    QTimer dataTimer;
};

#endif // DATAINFO_H
