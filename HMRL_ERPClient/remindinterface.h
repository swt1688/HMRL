#ifndef REMINDINTERFACE_H
#define REMINDINTERFACE_H

#include <QDialog>

namespace Ui {
class RemindInterface;
}

class RemindInterface : public QDialog
{
    Q_OBJECT

public:
    explicit RemindInterface(QWidget *parent = 0);
    ~RemindInterface();

private:
    Ui::RemindInterface *ui;
};

#endif // REMINDINTERFACE_H
