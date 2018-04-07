#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>

namespace Ui {
class MapWidget;
}

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = 0);
    ~MapWidget();

    void webShow(const QString &url);
private slots:

    //void replyFinished(QNetworkReply *);
    void showAddrWeb();
    void showBaiduMap();

private:
    Ui::MapWidget *ui;
    QNetworkAccessManager *manager;
};

#endif // MAPWIDGET_H
