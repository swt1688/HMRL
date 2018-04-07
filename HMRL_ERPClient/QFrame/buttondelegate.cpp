#include "buttondelegate.h"

#include <QApplication>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QStyleOption>
#include <QDesktopWidget>
#include <QDebug>
//QStringList ButtonDelegate::strtext;

//tableView中加入button
ButtonDelegate::ButtonDelegate(QObject *parent) :
QItemDelegate(parent)
{
    m_btnCnt=3;//! 默认为3个按钮
}
void ButtonDelegate::createStyleOptionButton(QVector<QStyleOptionButton*> & vecBtns, QStringList strtext) const
{
    for(int i=0;i<strtext.count();i++)
    {
        QStyleOptionButton* pBtn = new QStyleOptionButton();
        pBtn->text = strtext[i];
        pBtn->state |= QStyle::State_Enabled;
        vecBtns.append(pBtn);
    }
}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVector<QStyleOptionButton*> vecBtns=m_VecBtns.value(index);
    if (0==vecBtns.count())
    {
//        QStringList strtext;
//        if(m_btnCnt==1)
//        {
//            strtext<<tr("增加");
//        }
//        if(m_btnCnt==2)
//        {
//            strtext<<tr("增加")<<tr("删除");
//        }
//        if(m_btnCnt==3)
//        {
//            strtext<<tr("增加")<<tr("删除")<<tr("修改");
//        }
        createStyleOptionButton(vecBtns, strtext);
        (const_cast<ButtonDelegate *>(this))->m_VecBtns.insert(index, vecBtns);
    }
    int widLen=option.rect.width();
    if(m_btnCnt==3)
    {
        vecBtns[0]->rect = option.rect.adjusted(0, 4, -(widLen*2/3 + 4) , -4);
        vecBtns[1]->rect = option.rect.adjusted(widLen/3 + 4, 4, -(widLen/3+4), -4);
        vecBtns[2]->rect = option.rect.adjusted(widLen*2/3 + 4 , 4, -4, -4);
    }
    if(m_btnCnt==2)
    {
        vecBtns[0]->rect = option.rect.adjusted(4, 4, -(widLen/2 + 4) , -4);
        vecBtns[1]->rect = option.rect.adjusted(widLen/2 + 4, 4, -4, -4);
    }
    if(m_btnCnt==1)
    {
        vecBtns[0]->rect = option.rect.adjusted(0, 4, -4, -4);
    }

    painter->save();

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());

    }
    painter->restore();
    QApplication::style()->drawControl(QStyle::CE_PushButton, vecBtns[0], painter);
    if(m_btnCnt>=2) QApplication::style()->drawControl(QStyle::CE_PushButton, vecBtns[1], painter);
    if(m_btnCnt==3)QApplication::style()->drawControl(QStyle::CE_PushButton, vecBtns[2], painter);
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress)
    {

        QMouseEvent* e =(QMouseEvent*)event;

        if (m_VecBtns.contains(index))
        {
            QVector<QStyleOptionButton*> btns = m_VecBtns.value(index);
            if (btns[0]->rect.contains(e->x(), e->y()))
            {
                btns[0]->state |= QStyle::State_Sunken;
            }
            else if(btns[1]->rect.contains(e->x(), e->y()))
            {
                btns[1]->state |= QStyle::State_Sunken;
            }
            else if(btns[2]->rect.contains(e->x(), e->y()))
            {
                btns[2]->state |= QStyle::State_Sunken;
            }
        }
    }
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* e =(QMouseEvent*)event;

        if (m_VecBtns.contains(index))
        {
            QVector<QStyleOptionButton*> btns = m_VecBtns.value(index);
            if (btns[0]->rect.contains(e->x(), e->y()))
            {
                btns[0]->state &= (~QStyle::State_Sunken);
                showMsg(tr("btn1 column %1").arg(index.column()));
            }
            else if(btns[1]->rect.contains(e->x(), e->y()))
            {
                btns[1]->state &= (~QStyle::State_Sunken);
                showMsg(tr("btn2 row %1").arg(index.row()));
            }
            else if(btns[2]->rect.contains(e->x(), e->y()))
            {
                btns[2]->state &= (~QStyle::State_Sunken);
                showMsg(tr("btn2 row %1").arg(index.row()));
            }
        }
    }
    return true;
}

void ButtonDelegate::showMsg(QString str)
{
    QMessageBox msg;
    msg.setText(str);
    msg.exec();
}

