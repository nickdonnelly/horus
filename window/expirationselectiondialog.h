#ifndef EXPIRATIONSELECTIONDIALOG_H
#define EXPIRATIONSELECTIONDIALOG_H

#include <net/models.h>

#include <QDialog>
#include <QDateTime>

namespace Ui {
class ExpirationSelectionDialog;
}

class ExpirationSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExpirationSelectionDialog(QWidget *parent = 0);
    ~ExpirationSelectionDialog();
        ExpirationDuration GetResult();

public slots:
    void minPressed(bool);
    void hourPressed(bool);
    void dayPressed(bool);


private slots:
    void update_ui(int val);
    void confirmCurrent();

private:


    Duration sel;
    Ui::ExpirationSelectionDialog *ui;
    QDateTime selectedTime;
};

#endif // EXPIRATIONSELECTIONDIALOG_H
