#include "testqstring.h"
#include <QRegularExpression>
#include <firmwareencoder.h>


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

   // qDebug() << match1;
   // qDebug() << match2;
   // qDebug() << match3;

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

void TestQString::testHexFile()
{
    QByteArray hexInput;
    QByteArray binInput;
    uint32_t startAddress = 0;
    bool result_hex = readHexFile("scripts/test_cmp1.hex", hexInput,startAddress);
    bool result_bin = readBinFile("scripts/test_cmp1.bin", binInput);

    QCOMPARE(result_hex, true);
    QCOMPARE(result_bin, true);
    QCOMPARE(startAddress, (uint32_t)0x8000000);
    QVERIFY(hexInput.size() > 0);
    QCOMPARE(hexInput.size(), binInput.size());
    for (int i=0;i< hexInput.size();i++){
        uint8_t h=hexInput.at(i);
        uint8_t b=binInput.at(i);
        if (h!=b){
            qDebug() << "compare error at:" << i << "or"<< "0x"+QString::number(i,16).toUpper() << " hex = " << h <<"bin = " << b;
            QFAIL("hex and bin values not the same.");
        }
    }
}

void TestQString::testHexFile_error()
{
    QByteArray hexInput;
    uint32_t startAddress = 0;
    bool result_hex = readHexFile("scripts/test_error1.hex", hexInput,startAddress);
    QCOMPARE(result_hex, false);
}
