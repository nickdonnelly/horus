#ifndef EDITSETTINGSWINDOW_H
#define EDITSETTINGSWINDOW_H

#include <QMainWindow>

namespace Ui {
class EditSettingsWindow;
}

class EditSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditSettingsWindow(QWidget *parent = 0);
    ~EditSettingsWindow();

private slots:
    void switchPageUpload();
    void switchPageLocal();
    void switchPageServer();
    void switchPageAbout();
private:

    Ui::EditSettingsWindow *ui;
};

#endif // EDITSETTINGSWINDOW_H
