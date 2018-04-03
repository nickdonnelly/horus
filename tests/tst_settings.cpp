#include "autotest.h"
#include <components/hotkeyselector.h>
#include <components/hotkeyselectorlineedit.h>

#include <QtTest>
#include <QTextStream>
#include <QCoreApplication>
#include <QSignalSpy>

class SettingsTests : public QObject
{
    Q_OBJECT

public:
    SettingsTests();
    ~SettingsTests();

private slots:
    void initTestCase () {
        spy_count = 0;
        sel = new HotkeySelector("Test Label");
        le = sel->findChild<HotkeySelectorLineEdit*>("lineEdit");
        btnClear = sel->findChild<QPushButton*>("btnClear");
        btnCancel = sel->findChild<QPushButton*>("btnCancel");
        spy = new QSignalSpy(sel, SIGNAL(keyChanged(int,int)));
    }

    void cleanupTestCase() {
        delete spy;
        delete le;
        delete sel;
    }

    void hotkey_selector_test();
    void hotkey_selector_cancel();
    void hotkey_selector_clear();

private:
    HotkeySelector * sel;
    HotkeySelectorLineEdit* le;
    QPushButton *btnClear, *btnCancel;
    QSignalSpy * spy;

    int spy_count;
};

SettingsTests::SettingsTests()
{
}

SettingsTests::~SettingsTests()
{

}

void SettingsTests::hotkey_selector_test()
{
    QTest::mouseClick(le, Qt::LeftButton);
    QTest::keyClick(sel, Qt::Key_F2, Qt::ShiftModifier);

    QCOMPARE(spy->count(), ++spy_count);

    QKeySequence test_sequence(Qt::ShiftModifier, Qt::Key_F2);
    QCOMPARE(test_sequence, sel->getValue());
}

void SettingsTests::hotkey_selector_cancel()
{
    QTest::mouseClick(le, Qt::LeftButton);
    QTest::keyClick(sel, Qt::Key_F3);
    QTest::mouseClick(btnClear, Qt::LeftButton);

    QCOMPARE(QString(""), le->text());
}

void SettingsTests::hotkey_selector_clear()
{
    QTest::mouseClick(le, Qt::LeftButton);
    QTest::keyClick(sel, Qt::Key_F2, Qt::ShiftModifier);

    QCOMPARE(spy->count(), ++spy_count);

    QTest::mouseClick(btnClear, Qt::LeftButton);

    QCOMPARE(le->text(), QString(""));
}

DECLARE_TEST(SettingsTests)

#include "tst_settings.moc"
