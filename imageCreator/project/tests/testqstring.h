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
};
DECLARE_TEST(TestQString)
#endif // TESTQSTRING_H
