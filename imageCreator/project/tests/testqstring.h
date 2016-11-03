#ifndef TESTQSTRING_H
#define TESTQSTRING_H

#include <QObject>
#include "autotest.h"

class TestQString : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void toUpper();
    void isSpace();
    void testDefineRegex();
    void testHexFile();
    void testHexFile_error();
    void testAESDecrypt();
    void testAESEncrypt();
    void testAESDecrypt_raw_from_orig();
    void testAESDecrypt_raw();
    void testAESDecrypt_raw_append();
};
DECLARE_TEST(TestQString)
#endif // TESTQSTRING_H
