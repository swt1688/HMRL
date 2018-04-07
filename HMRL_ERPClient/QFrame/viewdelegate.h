#ifndef VIEWDELEGATE_H
#define VIEWDELEGATE_H

#include <QWidget>
#include <QApplication>
#include <QSqlRelationalDelegate>
#include <QLineEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QPixmap>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

#include <QStandardItemModel>
#include <QItemDelegate>
#include <QListWidget>
#include <QCheckBox>
#include <QDebug>
#include <QMouseEvent>
#include <QTableWidget>
#include <QPainter>

#include <QPushButton>
#include <QLabel>

#include "viewdelegate.h"

//编号列，只读委托
//这个方法我还真想不到，呵呵
class ReadOnlyDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ReadOnlyDelegate(QObject *parent = 0): QItemDelegate(parent) { }
    QWidget *createEditor(QWidget*parent, const QStyleOptionViewItem &option,const QModelIndex &index) const
    {
        return NULL;
    }
};

//ID列，只能输入1－12个数字
//利用QLineEdit委托和正则表达式对输入进行限制
class UserIDDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    UserIDDelegate(QObject *parent = 0): QItemDelegate(parent) { }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
    {
        QLineEdit *editor = new QLineEdit(parent);
        QRegExp regExp("[0-9]{0,10}");
        editor->setValidator(new QRegExpValidator(regExp, parent));
        return editor;
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QString text = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        lineEdit->setText(text);
    }
   void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const
    {
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        QString text = lineEdit->text();
        model->setData(index, text, Qt::EditRole);
    }
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        editor->setGeometry(option.rect);
    }
};

//利用QSpinBox委托进行输入限制
class SpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SpinBoxDelegate(QObject *parent = 0);

    //完成创建控件的工作，由参数中的QModelIndex对象指定的表选项数据的编辑控件，并对家控件内容进行限定
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &Option, const QModelIndex &index) const;
    //设置控件显示数据，将MODEL中的数据更新到Delegate中，相关于初始化
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    //将Delegate中对数据的改变更新到Model中
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
    //更新控件区的显示
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;

};


//性别列，利用QComboBox委托对输入进行限制
//这一列的单元格只能输入Male或Female
class SexDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SexDelegate(QObject *parent = 0): QItemDelegate(parent) { }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem("Female");
        editor->addItem("Male");
        return editor;
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QString text = index.model()->data(index, Qt::EditRole).toString();
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        int tindex = comboBox->findText(text);
        comboBox->setCurrentIndex(tindex);
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QString text = comboBox->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        editor->setGeometry(option.rect);
    }
};

//头像列，只是在单元格中央放一张小图而已
class IconDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    IconDelegate(QObject *parent = 0): QItemDelegate(parent) { }
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex & index ) const
    {
        //show.bmp是在工程目录中的一张图片（其实就是QQ的图标啦，呵呵）
        QPixmap pixmap = QPixmap("show.bmp").scaled(24, 24);
        qApp->style()->drawItemPixmap(painter, option.rect,  Qt::AlignCenter, QPixmap(pixmap));
    }
};

//代理类，把所有单元格中的字符居中显示
class VIPModel : public QStandardItemModel
{
    Q_OBJECT
public:
    VIPModel(QObject *parent=NULL) : QStandardItemModel(parent) { }
    VIPModel(int row, int column, QObject *parent=NULL)
        : QStandardItemModel(row, column, parent) { }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if( Qt::TextAlignmentRole == role )
            return Qt::AlignCenter;
        return QStandardItemModel::data(index, role);
    }

};





//QCombobox下拉列表添加QCheckBox复选框
class MyListView : public QListWidget
{
    Q_OBJECT    //自定义信号槽时需加上该宏，否则链接信号槽将会失败
public:
    /* columnlabels  QCheckBox名称   cmb添加列表项的QComboBox,此处主要用于及时的将用户选择的结果显示出来 */
    explicit MyListView(QWidget* parent=0, QComboBox* ComBox=NULL)
    {
        setViewMode(QListWidget::ListMode);
        setSelectionMode(QAbstractItemView::SingleSelection);
        m_cmbox = ComBox;

    }
    ~MyListView()
    {

    }
    void Add_Items(QStringList *columnlabels=NULL)
    {
        m_data.clear();
        m_cmbox->clear();
        m_bchecked.clear();
        m_checklist.clear();
        m_fslist.clear();
        for( int i=0; i<columnlabels->size(); i++ )
        {
            m_bchecked.append(true);
            m_data.append(columnlabels->at(i));

            QListWidgetItem *item = new QListWidgetItem();
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
            item->setData(Qt::UserRole+1, columnlabels->at(i));
            insertItem(model()->rowCount(), item);

            QFont font;
            font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
            font.setPointSize(16);

            QCheckBox* box = new QCheckBox;//(tr(columnlabels->at(i).toLatin1().data()));
            box->setFont(font);

            box->setText((columnlabels->at(i)));

            setItemWidget(item,  box);
            m_checklist.append(box);
            m_fslist.append(columnlabels->at(i));
            box->setCheckState(Qt::Checked);    // 设置为选中状态
            //m_bchecked.at(i)== true;
            //链接复选状态改变信号槽
           connect(box, QCheckBox::stateChanged, this, MyListView::set_select_item);

            //qDebug() << tr("链接checkbox消息：%1").arg(val);
        }
    }
    //返回用户选择项的用户自定义数据
    void get_select_data(QStringList &data)
    {
        for( int i=0; i<m_bchecked.size(); i++)
        {
            if( m_bchecked.at(i)== true)
                data.append( m_data.at(i) );
        }
    }

    //QStringList getList;

private slots:
    //QCheckBox复选消息处理
    void set_select_item(int state)
    {
        QCheckBox*  box = qobject_cast<QCheckBox*>(sender());

        for( int off = 0;off<m_checklist.size(); off++)
        {
            if( box == m_checklist.at(off) )
            {
                m_bchecked[off] = (box->checkState() ==  Qt::Checked) ? true : false;
                break;
            }
        }

        QString strfs;
        for( int i=0; i<m_bchecked.size(); i++)
        {
            if( m_bchecked.at(i)== true)
            {
                if(strfs.isEmpty())
                {
                    strfs = m_fslist.at(i);
                }
                else
                {
                    strfs += ",";
                    strfs += m_fslist.at(i);

                }
            }
        }

        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(16);
        m_cmbox->setFont(font);

        m_cmbox->setEditText( strfs );
    }


private:
    QComboBox        *m_cmbox;
    QList<bool>       m_bchecked;
    QStringList       m_data;
    QList<QCheckBox*> m_checklist;
    QList<QString>    m_fslist;
};


//利用QCheckBox委托方法1
class CheckBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    CheckBoxDelegate(QObject *parent = 0);
    //完成创建控件的工作，由参数中的QModelIndex对象指定的表选项数据的编辑控件，并对家控件内容进行限定
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &Option, const QModelIndex &index) const;
    //设置控件显示数据，将MODEL中的数据更新到Delegate中，相关于初始化
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    //将Delegate中对数据的改变更新到Model中
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
    //更新控件区的显示
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;

};
//利用QCheckBox委托方法2
class CheckBoxDelegate2 : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CheckBoxDelegate2(QObject *parent = 0);
protected:
    void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;
    bool editorEvent(QEvent *event,QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index);
};

//利用QCheckBox委托方法3
class QLineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QLineDelegate(QTableView* tableView);
protected:
    void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;
    bool editorEvent(QEvent *event,QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index);
private:
    QPen         pen;
    QTableView*  view;
};





#endif // VIEWDELEGATE_H
