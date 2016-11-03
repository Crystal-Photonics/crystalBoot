#include "testqstring.h"
#include <QRegularExpression>
#include <QBuffer>
#include <firmwareencoder.h>
#include "aes.h"

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


void TestQString::testAESDecrypt_raw_append()
{
#if 1

    // Example "simulating" a smaller buffer...

    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t in[]  = { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
                      0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
                      0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
                      0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7 };

    uint8_t out[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                      0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                      0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                      0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };

    uint8_t tmp_in[16];
    uint8_t tmp_out[16];
    uint8_t buffer[64];

    memcpy(tmp_in,&in[0], sizeof(tmp_in));
    AES128_CBC_decrypt_buffer(tmp_out, tmp_in,  16, key, iv);
    memcpy(&buffer[0],tmp_out,sizeof tmp_out);

    memcpy(tmp_in,&in[16], sizeof(tmp_in));
    AES128_CBC_decrypt_buffer(tmp_out, tmp_in, 16, 0, 0);
    memcpy(&buffer[16],tmp_out,sizeof tmp_out);

    memcpy(tmp_in,&in[32], sizeof(tmp_in));
    AES128_CBC_decrypt_buffer(tmp_out, tmp_in, 16, 0, 0);
    memcpy(&buffer[32],tmp_out,sizeof tmp_out);

    memcpy(tmp_in,&in[48], sizeof(tmp_in));
    AES128_CBC_decrypt_buffer(tmp_out, tmp_in, 16, 0, 0);
    memcpy(&buffer[48],tmp_out,sizeof tmp_out);

    for (size_t i=0;i< sizeof buffer;i++){

        uint8_t deciphered=buffer[i];
        uint8_t plain=out[i];
        if (deciphered!=plain){
            qDebug() << "compare error at:" << i << "or"<< "0x"+QString::number(i,16).toUpper() << " deciphered = " << deciphered <<"plain = " << plain;
            QFAIL("ciphered and cipher values not the same.");
        }
    }
#endif
}


void TestQString::testAESDecrypt_raw_from_orig()
{
#if 1

    // Example "simulating" a smaller buffer...

    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t in[]  = { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
                      0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
                      0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
                      0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7 };

    uint8_t out[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                      0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                      0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                      0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };

    uint8_t buffer[64];

    AES128_CBC_decrypt_buffer(buffer+0, in+0,  16, key, iv);
    AES128_CBC_decrypt_buffer(buffer+16, in+16, 16, 0, 0);
    AES128_CBC_decrypt_buffer(buffer+32, in+32, 16, 0, 0);
    AES128_CBC_decrypt_buffer(buffer+48, in+48, 16, 0, 0);


    for (size_t i=0;i< sizeof buffer;i++){

        uint8_t deciphered=buffer[i];
        uint8_t plain=out[i];
        if (deciphered!=plain){
            qDebug() << "compare error at:" << i << "or"<< "0x"+QString::number(i,16).toUpper() << " deciphered = " << deciphered <<"plain = " << plain;
            QFAIL("ciphered and cipher values not the same.");
        }
    }
#endif
}

void TestQString::testAESDecrypt()
{
#if 1
    QByteArray key         (QByteArray::fromHex("56e47a38c5598974bc46903dba290349"));
    QByteArray iv          (QByteArray::fromHex("8ce82eefbea0da3c44699ed7db51b7d9"));

    QByteArray binCipher   (QByteArray::fromHex("c30e32ffedc0774e6aff6af0869f71aa"));
    binCipher.append       (QByteArray::fromHex("0f3af07a9a31a9c684db207eb0ef8e4e"));
    binCipher.append       (QByteArray::fromHex("35907aa632c3ffdf868bb7b29d3d46ad"));
    binCipher.append       (QByteArray::fromHex("83ce9f9a102ee99d49a53e87f4c3da55"));


    QByteArray binPlain    (QByteArray::fromHex("a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"));
    binPlain.append        (QByteArray::fromHex("b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"));
    binPlain.append        (QByteArray::fromHex("c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"));
    binPlain.append        (QByteArray::fromHex("d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"));



    QByteArray deciphered_array = AES_CBC_128_decrypt(key, iv, binCipher);

    for (int i=0;i< deciphered_array.size();i++){
        uint8_t deciphered=deciphered_array.at(i);
        uint8_t plain=binPlain.at(i);
        if (deciphered!=plain){
            qDebug() << "compare error at:" << i << "or"<< "0x"+QString::number(i,16).toUpper() << " deciphered = " << deciphered <<"plain = " << plain;
            QFAIL("ciphered and cipher values not the same.");
        }
    }
#endif
}
void TestQString::testAESDecrypt_raw()
{
#if 1

    // Example "simulating" a smaller buffer...
   uint8_t key[] = { 0x56, 0xe4, 0x7a, 0x38, 0xc5, 0x59, 0x89, 0x74, 0xbc, 0x46, 0x90, 0x3d, 0xba, 0x29, 0x03, 0x49};
   uint8_t iv[]  = { 0x8c, 0xe8, 0x2e, 0xef, 0xbe, 0xa0, 0xda, 0x3c, 0x44, 0x69, 0x9e, 0xd7, 0xdb, 0x51, 0xb7, 0xd9};

   uint8_t in[]  = { 0xc3, 0x0e, 0x32, 0xff, 0xed, 0xc0, 0x77, 0x4e, 0x6a, 0xff, 0x6a, 0xf0, 0x86, 0x9f, 0x71, 0xaa,
                     0x0f, 0x3a, 0xf0, 0x7a, 0x9a, 0x31, 0xa9, 0xc6, 0x84, 0xdb, 0x20, 0x7e, 0xb0, 0xef, 0x8e, 0x4e,
                     0x35, 0x90, 0x7a, 0xa6, 0x32, 0xc3, 0xff, 0xdf, 0x86, 0x8b, 0xb7, 0xb2, 0x9d, 0x3d, 0x46, 0xad,
                     0x83, 0xce, 0x9f, 0x9a, 0x10, 0x2e, 0xe9, 0x9d, 0x49, 0xa5, 0x3e, 0x87, 0xf4, 0xc3, 0xda, 0x55};

   uint8_t out[] = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
                     0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
                     0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
                     0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf};
#if 0

    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t in[]  = { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
                      0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
                      0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
                      0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7 };

    uint8_t out[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                      0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                      0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                      0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };
#endif
    uint8_t buffer[64];

    AES128_CBC_decrypt_buffer(buffer+0, in+0,  16, key, iv);
    AES128_CBC_decrypt_buffer(buffer+16, in+16, 16, 0, 0);
    AES128_CBC_decrypt_buffer(buffer+32, in+32, 16, 0, 0);
    AES128_CBC_decrypt_buffer(buffer+48, in+48, 16, 0, 0);


    for (size_t i=0;i< sizeof buffer;i++){

        uint8_t deciphered=buffer[i];
        uint8_t plain=out[i];
        if (deciphered!=plain){
            qDebug() << "compare error at:" << i << "or"<< "0x"+QString::number(i,16).toUpper() << " deciphered = " << deciphered <<"plain = " << plain;
            QFAIL("ciphered and cipher values not the same.");
        }
    }
#endif
}

void TestQString::testAESEncrypt()
{

    //https://tools.ietf.org/html/rfc3602
    QByteArray binCipher   (QByteArray::fromHex("c30e32ffedc0774e6aff6af0869f71aa"));
    binCipher.append       (QByteArray::fromHex("0f3af07a9a31a9c684db207eb0ef8e4e"));
    binCipher.append       (QByteArray::fromHex("35907aa632c3ffdf868bb7b29d3d46ad"));
    binCipher.append       (QByteArray::fromHex("83ce9f9a102ee99d49a53e87f4c3da55"));


    QByteArray binPlain     (QByteArray::fromHex("a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"));
    binPlain.append         (QByteArray::fromHex("b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"));
    binPlain.append         (QByteArray::fromHex("c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"));
    binPlain.append         (QByteArray::fromHex("d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"));

    QByteArray key          (QByteArray::fromHex("56e47a38c5598974bc46903dba290349"));
    QByteArray iv           (QByteArray::fromHex("8ce82eefbea0da3c44699ed7db51b7d9"));
                                        //        8c e8 2e ef be a0 da 3c 44 69 9e d7 db 51 b7 d9
    QByteArray ciphered_array = AES_CBC_128_encrypt(key, iv, binPlain);

    for (int i=0;i< ciphered_array.size();i++){
        uint8_t ciphered=ciphered_array.at(i);
        uint8_t cipher=binCipher.at(i);
        if (ciphered!=cipher){
            qDebug() << "compare error at:" << i << "or"<< "0x"+QString::number(i,16).toUpper() << " ciphered = " << ciphered <<"cipher = " << cipher;
            QFAIL("ciphered and cipher values not the same.");
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
