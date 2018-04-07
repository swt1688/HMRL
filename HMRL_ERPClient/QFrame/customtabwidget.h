#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QObject>
#include <QPainter>
#include <QProxyStyle>


class CustomTabWidget : public QProxyStyle
{
public:
    CustomTabWidget();
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const Q_DECL_OVERRIDE;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;
};

#endif // CUSTOMTABWIDGET_H
