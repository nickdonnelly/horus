#ifndef EDITSETTINGSWINDOW_H
#define EDITSETTINGSWINDOW_H

#include <QMainWindow>
#include <QSettings>

namespace Ui {
class EditSettingsWindow;
}

class EditSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditSettingsWindow(QSettings *settings, QWidget *parent = 0);
    ~EditSettingsWindow();

private:
    void setUIElementValues();

private slots:
    void switchPageUpload();
    void switchPageLocal();
    void switchPageServer();
    void switchPageAbout();
    void selectLocalFolder();
private:
    QSettings * sets;

    Ui::EditSettingsWindow *ui;
};

#endif // EDITSETTINGSWINDOW_H
