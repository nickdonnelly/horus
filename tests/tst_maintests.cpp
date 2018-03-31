#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

class MainTests : public QObject
{
    Q_OBJECT

public:
    MainTests();
    ~MainTests();

private slots:
    void test_case1();

};

MainTests::MainTests()
{

}

MainTests::~MainTests()
{

}

void MainTests::test_case1()
{

}

QTEST_MAIN(MainTests)

#include "tst_maintests.moc"
