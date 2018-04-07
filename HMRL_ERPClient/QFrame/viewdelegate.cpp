#include "viewdelegate.h"

//利用QSpinBox委托进行输入限制，只能输入1－100之间的数字

SpinBoxDelegate::SpinBoxDelegate(QObject *parent):
    QItemDelegate(parent)
{
}
QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
        const QModelIndex &/*index*/) const
    {
        QSpinBox *editor = new QSpinBox(parent);
        //editor->setMinimum(1);
        //editor->setMaximum(100);
        editor->setRange(1,100);
        //editor->installEventFilter(const_cast<SpinDelegate*>(this));
        return editor;
    }
void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(value);
    }
void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const
    {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }
void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        editor->setGeometry(option.rect);
    }

//利用QCheckBox委托进行
CheckBoxDelegate::CheckBoxDelegate(QObject *parent):
    QItemDelegate(parent)
{
}
QWidget *CheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
        const QModelIndex &/*index*/) const
    {
        QCheckBox *editor = new QCheckBox(parent);
        //editor->setMinimum(1);
        //editor->setMaximum(100);
        editor->setCheckState(Qt::Checked);
        //editor->installEventFilter(const_cast<SpinDelegate*>(this));
        return editor;
    }
void CheckBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        bool value = index.model()->data(index, Qt::EditRole).toBool();

        QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
        checkBox->setChecked(value);
    }
void CheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const
    {
        QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
        checkBox->setCheckable(true);
        bool value = checkBox->checkState();
        model->setData(index, value, Qt::EditRole);
    }
void CheckBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        editor->setGeometry(option.rect);
}



CheckBoxDelegate2::CheckBoxDelegate2(QObject *parent):
   QStyledItemDelegate(parent)
{

}
static QRect CheckBoxRect(const QStyleOptionViewItem &viewItemStyleOptions)/*const*/
{
    //绘制按钮所需要的参数
    QStyleOptionButton checkBoxStyleOption;
    //按照给定的风格参数 返回元素子区域
    QRect checkBoxRect = QApplication::style()->subElementRect( QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
    //返回QCheckBox坐标
    QPoint checkBoxPoint(viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width() / 2 - checkBoxRect.width() / 2,
                         viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - checkBoxRect.height() / 2);
    //返回QCheckBox几何形状
    return QRect(checkBoxPoint, checkBoxRect.size());
}

void CheckBoxDelegate2::paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index)const
{
    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();

//    if(index.column() == 3)
//    {
        //qDebug() << checked;
        QStyleOptionButton checkBoxStyleOption;
        checkBoxStyleOption.state |= QStyle::State_Enabled;
        checkBoxStyleOption.state |= checked ? QStyle::State_On : QStyle::State_Off;
        checkBoxStyleOption.rect = CheckBoxRect(option);

        QApplication::style()->drawControl(QStyle::CE_CheckBox,&checkBoxStyleOption,painter);
//    }
//    else
//    {
//        QStyledItemDelegate::paint(painter, option, index);
//    }
}

bool CheckBoxDelegate2::editorEvent(QEvent *event,
                                QAbstractItemModel *model,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index)
{
//    if(index.column() == 0)
//    {
        if((event->type() == QEvent::MouseButtonRelease) ||(event->type() == QEvent::MouseButtonDblClick))
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() != Qt::LeftButton ||!CheckBoxRect(option).contains(mouseEvent->pos()))
            {
                return true;
            }
            if(event->type() == QEvent::MouseButtonDblClick)
            {
                return true;
            }
        }
        else if(event->type() == QEvent::KeyPress)
        {
            if(static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            {
                    return false;
            }
       }
        else
        {
            return false;
        }

       bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
       return model->setData(index, !checked, Qt::EditRole);
//    }
//    else
//    {
//        return QStyledItemDelegate::editorEvent(event, model, option, index);
//    }
}

QLineDelegate::QLineDelegate(QTableView* tableView)
{
    int gridHint = tableView->style()->styleHint(QStyle::SH_Table_GridLineColor, new QStyleOptionViewItemV4());
    QColor gridColor = static_cast<QRgb>(gridHint);
    pen = QPen(gridColor,0,tableView->gridStyle());
    view = tableView;
}

void QLineDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index)const
{
    bool checked = index.model()->data(index, Qt::EditRole).toBool();

    if(index.column() == 0)
    {
        QStyleOptionButton check_box_style_option;
        check_box_style_option.state |= QStyle::State_Enabled;
        if(checked)
        {
            check_box_style_option.state |= QStyle::State_On;
        }
       else
        {
            check_box_style_option.state |= QStyle::State_Off;
       }
        check_box_style_option.rect = CheckBoxRect(option);
        QApplication::style()->drawControl(QStyle::CE_CheckBox,&check_box_style_option,painter);
    }

    QStyledItemDelegate::paint(painter,option,index);
    QStyleOptionViewItem itemOption(option);
    if(itemOption.state & QStyle::State_HasFocus)
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
    QStyledItemDelegate::paint(painter,itemOption,index);
    QPen oldPen = painter->pen();
    painter->setPen(pen);
    painter->drawLine(option.rect.topRight(),option.rect.bottomRight());
    painter->drawLine(itemOption.rect.bottomLeft(),itemOption.rect.bottomRight());
    painter->setPen(oldPen);
}

bool QLineDelegate::editorEvent(QEvent *event,QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    if((event->type() == QEvent::MouseButtonRelease) ||(event->type() == QEvent::MouseButtonDblClick))
    {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
        if(mouse_event->button() != Qt::LeftButton ||
           !CheckBoxRect(option).contains(mouse_event->pos()))
        {
            return false;
        }
        if(event->type() == QEvent::MouseButtonDblClick)
        {
            return true;
        }
    }
    else if(event->type() == QEvent::KeyPress)
    {
        if(static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
           static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
    return model->setData(index, !checked, Qt::EditRole);
}



