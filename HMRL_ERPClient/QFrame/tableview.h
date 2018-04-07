#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>
#include "tablemodel.h"
#include "buttondelegate.h"



class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = 0);
    TableModel* tableModel() {return m_model;}
    //QVariant data(const QModelIndex &index, int role) const;
    ~TableView();

signals:

public slots:

private:
    void initData();

private:
    TableModel *m_model;
    ButtonDelegate *m_buttonDelegate;

};

class TimeEditDelegate : public QItemDelegate
{
    Q_OBJECT
    public:
    TimeEditDelegate(const QString timeFormat = "dd.MM.yyyy hh:mm:ss",QObject *parent = 0) : QItemDelegate(parent)
    {
        this->timeformat = timeFormat;
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const;
    private:
    QString timeformat;
};

//日期类型委托代理
class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DateDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//Combox类型委托代理
class ComboDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ComboDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QStringList CreateData;
};
//SpinBox类型委托代理
class SpinDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SpinDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;

};
#endif // TABLEVIEW_H
