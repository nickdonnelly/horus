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
    explicit UpdateDownloadDialog(QString url, QWidget *parent = 0);
    ~UpdateDownloadDialog();

signals:
    void downloadFinished();

protected slots:
    void progressUpdate(qint64 bytesReceived, qint64 bytesTotal);
    void downloaded(QNetworkReply*);

private:
    Ui::UpdateDownloadDialog *ui;
    QNetworkAccessManager * manager;
    QByteArray downloadedData;
    QString reqURL;

    void downloadUpdate();

    void attemptExtract();
};

#endif // UPDATEDOWNLOADDIALOG_H
