#include <QtTest>
#include <QCoreApplication>
#include "autotest.h"

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

}

DECLARE_TEST(SettingsTests)

#include "tst_settings.moc"
