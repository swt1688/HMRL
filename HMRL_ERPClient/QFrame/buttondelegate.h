#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QItemDelegate>

//QTableView 一个单元格中添加按钮
class ButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:

    explicit ButtonDelegate(QObject *parent = 0);
    void setBtnCnt(int btnCnt) {m_btnCnt=btnCnt;}
    void createStyleOptionButton(QVector<QStyleOptionButton *> &vecBtns, QStringList strtext) const;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;
    QStringList strtext;

signals:

public slots:

private:
    void showMsg(QString str);
    int m_btnCnt;

private:

    typedef QMap<QModelIndex, QPair<QStyleOptionButton*, QStyleOptionButton*>* > collButtons;
    // collButtons m_btns;

    typedef QMap<QModelIndex, QVector<QStyleOptionButton*> > vecButtons;
    vecButtons m_VecBtns;
};

#endif // BUTTONDELEGATE_H
