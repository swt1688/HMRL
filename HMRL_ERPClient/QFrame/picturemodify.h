#ifndef PICTUREMODIFY_H
#define PICTUREMODIFY_H

#include <QDialog>



namespace Ui {
class PictureModify;
}

class PictureModify : public QDialog
{
    Q_OBJECT

public:
    explicit PictureModify(QWidget *parent = 0);
    ~PictureModify();

private slots:
    void on_btnSearch_clicked();

    void on_btnOk_clicked();

private:
    Ui::PictureModify *ui;

};

#endif // PICTUREMODIFY_H
