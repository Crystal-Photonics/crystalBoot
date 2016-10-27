#include "testqstring.h"
#include <QRegularExpression>



void TestQString::initTestCase(){

}

void TestQString::cleanupTestCase(){

}

void TestQString::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

void TestQString::isSpace()
{
    QChar c_space(' ');
    QChar c_uscore('_');
    QChar c_tab('\t');
    QCOMPARE(c_space.isSpace(), true);
    QCOMPARE(c_tab.isSpace(), true);
    QCOMPARE(c_uscore.isSpace(), false);
}

void TestQString::testDefineRegex()
{
    QRegularExpression re("([^\\s]*)\\s*([^\\s]*)\\s*(.*)");
    QStringList match1 = re.match("#define defname5 \"ich bin wasauchimmer\"").capturedTexts();
    QStringList match2 = re.match("#define  		 defname6  			 		\"blargj   		yo\" //dcflksdf").capturedTexts();
    QStringList match3 = re.match("#define defname2 2").capturedTexts();

    qDebug() << match1;
    qDebug() << match2;
    qDebug() << match3;

    QCOMPARE(match1.count(), 4);
    QCOMPARE(match2.count(), 4);
    QCOMPARE(match3.count(), 4);
    QCOMPARE(match1[0], QString("#define defname5 \"ich bin wasauchimmer\"")); //der full match
    QCOMPARE(match1[1], QString("#define"));
    QCOMPARE(match1[2], QString("defname5"));
    QCOMPARE(match1[3], QString( "\"ich bin wasauchimmer\""));

    QCOMPARE(match2[1], QString("#define"));
    QCOMPARE(match2[2], QString("defname6"));
    QCOMPARE(match2[3], QString( "\"blargj   		yo\" //dcflksdf"));

    QCOMPARE(match3[1], QString("#define"));
    QCOMPARE(match3[2], QString("defname2"));
    QCOMPARE(match3[3], QString( "2"));

}
