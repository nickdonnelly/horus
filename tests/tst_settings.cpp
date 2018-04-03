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
    void hotkeys_test();
};

SettingsTests::SettingsTests()
{

}

SettingsTests::~SettingsTests()
{

}

void SettingsTests::hotkeys_test()
{
    HotkeySelector sel("Test Label");
    HotkeySelectorLineEdit* le = sel.findChild<HotkeySelectorLineEdit*>("lineEdit");
    QSignalSpy spy(&sel, SIGNAL(keyChanged(int,int)));
    QTest::mouseClick(le, Qt::LeftButton);
    QTest::keyClick(&sel, Qt::Key_F2, Qt::ShiftModifier);

    QCOMPARE(spy.count(), 1);

    QKeySequence test_sequence(Qt::ShiftModifier, Qt::Key_F2);
    QCOMPARE(test_sequence, sel.getValue());
}

DECLARE_TEST(SettingsTests)

#include "tst_settings.moc"
