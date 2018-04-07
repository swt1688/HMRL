#include "ccheckcombox.h"

CCheckCombox::CCheckCombox(QWidget *parent) :
    QComboBox(parent)
{

}

void CCheckCombox::appendItem(const QString &text, bool bChecked)
{
    QIcon icon;
    if( bChecked )
    {
        icon.addFile(":/image/checkbox_checked.png");
        SelectItems<<text;
    }
    else
    {
        icon.addFile(":/image/checkbox_unchecked.png");
    }

    addItem(icon, text, bChecked);
}

void CCheckCombox::updateIndexStatus(int index)
{
    bool bChecked = this->itemData(index).toBool();

    if( bChecked )
    {
        this->setItemIcon(index, QIcon(":/image/checkbox_checked.png"));
        CCheckCombox::SelectItems<<this->itemText(index);
    }
    else
    {
        this->setItemIcon(index, QIcon(":/image/checkbox_unchecked.png"));
        CCheckCombox::SelectItems.removeAt(index);
    }

    setItemData(index, !bChecked);

    emit checkedStateChange(index, !bChecked);
}

void CCheckCombox::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x();

    int iconWidth = this->iconSize().width();

    if( x <= iconWidth )
    {
        int index = this->currentIndex();

        updateIndexStatus(index);
    }
    else
    {
        QComboBox::mousePressEvent(event);
    }
}

void CCheckCombox::hidePopup()
{
    int iconWidth = this->iconSize().width();

    int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();

    int index = view()->selectionModel()->currentIndex().row();

    if( x >= 0 && x <= iconWidth )
    {
        updateIndexStatus(index);
    }
    else
    {
        QComboBox::hidePopup();
    }
}

