#ifndef UPDATEDOWNLOADDIALOG_H
#define UPDATEDOWNLOADDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class UpdateDownloadDialog;
}

class UpdateDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDownloadDialog(QString url, QString authtoken, QWidget *parent = 0);
    ~UpdateDownloadDialog();

signals:
    void downloadFinished();

protected slots:
    void progressUpdate(qint64 bytesReceived, qint64 bytesTotal);
    void downloaded(QNetworkReply*);
    void restartClicked();

private:
    Ui::UpdateDownloadDialog *ui;
    QNetworkAccessManager * manager;
    QByteArray downloadedData;
    QString reqURL;
    QString update_dir_str;
    QString AUTH_TOKEN;

    void downloadUpdate();

    void attemptExtract();
};

#endif // UPDATEDOWNLOADDIALOG_H
