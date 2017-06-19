#ifndef UPDATEDOWNLOADDIALOG_H
#define UPDATEDOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateDownloadDialog;
}

class UpdateDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDownloadDialog(QWidget *parent = 0);
    ~UpdateDownloadDialog();

private:
    Ui::UpdateDownloadDialog *ui;

    void downloadUpdate();
};

#endif // UPDATEDOWNLOADDIALOG_H
