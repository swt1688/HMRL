#include "tableview.h"

#include "tablemodel.h"
//#include "buttondelegate.h"
#include <QHeaderView>
#include <QSqlQuery>
#include <QtWidgets>

TableView::TableView(QWidget *parent) :
QTableView(parent)
{
    initData();
}

TableView::~TableView()
{
    delete m_model;
}

//QVariant TableView::data(const QModelIndex &index, int role) const
//{
//    if (!index.isValid())
//        return QVariant();
//    if (index.row() >= fEntries.size() || index.row() < 0)
//        return QVariant();
//    if(role == Qt::DisplayRole) {
//        const Entry& entry = fEntries.at(index.row());
//        const QString& key = getColumnId(index.column());
//        return entry.value(key);
//    }
//    if(role == Qt::BackgroundRole)
//    {
//        if((1 == index.column())&(fEntries[index.row()].value("LandType") == QString::fromLocal8Bit("登陆失败")))
//        {
//            return QVariant(Qt::GlobalColor(Qt::red));
//        }
//        else if(((1 == index.column())&(fEntries[index.row()].value("LandType") == QString::fromLocal8Bit("登陆成功"))))
//        {
//            return QVariant(Qt::GlobalColor(Qt::green));
//        }
//    }
//    return QVariant();
//}

void TableView::initData()
{

}

void TimeEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
//    QDateTime dtval = index.model()->data(index, Qt::DisplayRole).toDateTime();
//    QString indexvalue = tr("Date not set");
//    if(dtval.isValid())
//    {
//        indexvalue = dtval.toString(timeformat);
//    }
//    Q_ASSERT(index.isValid());
//    QStyleOptionViewItemV3 opt = setOptions(index, option);
//    const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
//    opt.features = v2 ? v2->features  : QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
//    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
//    opt.locale = v3 ? v3->locale : QLocale();
//    opt.widget = v3 ? v3->widget : 0;
//    // prepare
//    painter->save();
//    painter->setClipRect(opt.rect);
//    // get the data and the rectangles
//    QVariant value;
//    QPixmap pixmap;
//    QRect decorationRect;
//    value = index.data(Qt::DecorationRole);
//    QString text;
//    QRect displayRect;
//    value = index.data(Qt::DisplayRole);
//    if (value.isValid()) {
//        text = indexvalue;
//        displayRect = textRectangle(painter, option.rect, opt.font, text);
//    }
//    QRect checkRect;
//    Qt::CheckState checkState = Qt::Unchecked;
//    value = index.data(Qt::CheckStateRole);
//    if (value.isValid()) {
//        checkState = static_cast<Qt::CheckState>(value.toInt());
//        checkRect = check(opt, opt.rect, value);
//    }
//    // do the layout
//    doLayout(opt, &checkRect, &decorationRect, &displayRect, false);
//    // draw the item
//    drawBackground(painter, opt, index);
//    drawCheck(painter, opt, checkRect, checkState);
//    drawDecoration(painter, opt, decorationRect, pixmap);
//    drawDisplay(painter, opt, displayRect, text);
//    drawFocus(painter, opt, displayRect);
//    // done
//    painter->restore();
}

//日期类型委托代理
DateDelegate::DateDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *DateDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QDateTimeEdit *editor = new QDateTimeEdit(parent);
    editor->setDisplayFormat("yyyy-MM-dd");
    editor->setCalendarPopup(true);
    editor->installEventFilter(const_cast<DateDelegate*>(this));

    return editor;
}

void DateDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    QString dateStr = index.model()->data(index).toString();
    QDate date = QDate::fromString(dateStr,Qt::ISODate);

    QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
    edit->setDate(date);
}

void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
    QDate date = edit->date();

    model->setData(index, QVariant(date.toString(Qt::ISODate)));
}

void DateDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//Combox类型委托代理
ComboDelegate::ComboDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *ComboDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);

//    editor->addItem(QString::fromLocal8Bit("工人"));
//    editor->addItem(QString::fromLocal8Bit("农民"));
//    editor->addItem(QString::fromLocal8Bit("医生"));
//    editor->addItem(QString::fromLocal8Bit("律师"));
//    editor->addItem(QString::fromLocal8Bit("军人"));
    editor->addItems(CreateData);

    editor->installEventFilter(const_cast<ComboDelegate*>(this));

    return editor;
}

void ComboDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    QString str = index.model()->data(index).toString();

    QComboBox *box = static_cast<QComboBox*>(editor);
    int i = box->findText(str);
    box->setCurrentIndex(i);
}

void ComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    QString str = box->currentText();

    model->setData(index, str);
}

void ComboDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//SpinBox类型委托代理

SpinDelegate::SpinDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *SpinDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setRange(1000,10000);

    editor->installEventFilter(const_cast<SpinDelegate*>(this));

    return editor;
}

void SpinDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    int value = index.model()->data(index).toInt();

    QSpinBox *spin = static_cast<QSpinBox*>(editor);

    spin->setValue(value);
}

void SpinDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QSpinBox *spin = static_cast<QSpinBox*>(editor);
    int value = spin->value();

    model->setData(index, value);
}

void SpinDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
