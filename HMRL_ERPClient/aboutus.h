#ifndef ABOUTUS_H
#define ABOUTUS_H

#include <QDialog>

namespace Ui {
class AboutUs;
}

class AboutUs : public QDialog
{
    Q_OBJECT

public:
    explicit AboutUs(QWidget *parent = 0);
    ~AboutUs();

private slots:
    void on_btnClose_clicked();

private:
    Ui::AboutUs *ui;
};

#endif // ABOUTUS_H
