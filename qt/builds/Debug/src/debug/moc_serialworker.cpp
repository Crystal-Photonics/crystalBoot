/****************************************************************************
** Meta object code from reading C++ file 'serialworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../project/src/serialworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SerialWorker_t {
    QByteArrayData data[17];
    char stringdata[158];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialWorker_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialWorker_t qt_meta_stringdata_SerialWorker = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SerialWorker"
QT_MOC_LITERAL(1, 13, 8), // "finished"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 9), // "updateADC"
QT_MOC_LITERAL(4, 33, 4), // "adc1"
QT_MOC_LITERAL(5, 38, 14), // "updateKeyState"
QT_MOC_LITERAL(6, 53, 14), // "rpcKeyStatus_t"
QT_MOC_LITERAL(7, 68, 8), // "keyState"
QT_MOC_LITERAL(8, 77, 8), // "openPort"
QT_MOC_LITERAL(9, 86, 4), // "name"
QT_MOC_LITERAL(10, 91, 8), // "baudrate"
QT_MOC_LITERAL(11, 100, 9), // "closePort"
QT_MOC_LITERAL(12, 110, 12), // "isPortOpened"
QT_MOC_LITERAL(13, 123, 8), // "sendData"
QT_MOC_LITERAL(14, 132, 4), // "data"
QT_MOC_LITERAL(15, 137, 7), // "process"
QT_MOC_LITERAL(16, 145, 12) // "on_readyRead"

    },
    "SerialWorker\0finished\0\0updateADC\0adc1\0"
    "updateKeyState\0rpcKeyStatus_t\0keyState\0"
    "openPort\0name\0baudrate\0closePort\0"
    "isPortOpened\0sendData\0data\0process\0"
    "on_readyRead"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    1,   60,    2, 0x06 /* Public */,
       5,    1,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,   66,    2, 0x0a /* Public */,
      11,    0,   71,    2, 0x0a /* Public */,
      12,    0,   72,    2, 0x0a /* Public */,
      13,    1,   73,    2, 0x0a /* Public */,
      15,    0,   76,    2, 0x0a /* Public */,
      16,    0,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    9,   10,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QByteArray,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SerialWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialWorker *_t = static_cast<SerialWorker *>(_o);
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->updateADC((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->updateKeyState((*reinterpret_cast< rpcKeyStatus_t(*)>(_a[1]))); break;
        case 3: _t->openPort((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->closePort(); break;
        case 5: { bool _r = _t->isPortOpened();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: _t->sendData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 7: _t->process(); break;
        case 8: _t->on_readyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SerialWorker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialWorker::finished)) {
                *result = 0;
            }
        }
        {
            typedef void (SerialWorker::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialWorker::updateADC)) {
                *result = 1;
            }
        }
        {
            typedef void (SerialWorker::*_t)(rpcKeyStatus_t );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialWorker::updateKeyState)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject SerialWorker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SerialWorker.data,
      qt_meta_data_SerialWorker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SerialWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SerialWorker.stringdata))
        return static_cast<void*>(const_cast< SerialWorker*>(this));
    return QObject::qt_metacast(_clname);
}

int SerialWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SerialWorker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void SerialWorker::updateADC(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialWorker::updateKeyState(rpcKeyStatus_t _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_SerialThread_t {
    QByteArrayData data[14];
    char stringdata[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialThread_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialThread_t qt_meta_stringdata_SerialThread = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SerialThread"
QT_MOC_LITERAL(1, 13, 8), // "openPort"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 4), // "name"
QT_MOC_LITERAL(4, 28, 8), // "baudrate"
QT_MOC_LITERAL(5, 37, 9), // "closePort"
QT_MOC_LITERAL(6, 47, 12), // "isPortOpened"
QT_MOC_LITERAL(7, 60, 8), // "sendData"
QT_MOC_LITERAL(8, 69, 4), // "data"
QT_MOC_LITERAL(9, 74, 9), // "updateADC"
QT_MOC_LITERAL(10, 84, 4), // "adc1"
QT_MOC_LITERAL(11, 89, 14), // "updateKeyState"
QT_MOC_LITERAL(12, 104, 14), // "rpcKeyStatus_t"
QT_MOC_LITERAL(13, 119, 8) // "keyState"

    },
    "SerialThread\0openPort\0\0name\0baudrate\0"
    "closePort\0isPortOpened\0sendData\0data\0"
    "updateADC\0adc1\0updateKeyState\0"
    "rpcKeyStatus_t\0keyState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       5,    0,   49,    2, 0x06 /* Public */,
       6,    0,   50,    2, 0x06 /* Public */,
       7,    1,   51,    2, 0x06 /* Public */,
       9,    1,   54,    2, 0x06 /* Public */,
      11,    1,   57,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QByteArray,    8,
    QMetaType::Void, QMetaType::Float,   10,
    QMetaType::Void, 0x80000000 | 12,   13,

       0        // eod
};

void SerialThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialThread *_t = static_cast<SerialThread *>(_o);
        switch (_id) {
        case 0: _t->openPort((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->closePort(); break;
        case 2: { bool _r = _t->isPortOpened();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->sendData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 4: _t->updateADC((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: _t->updateKeyState((*reinterpret_cast< rpcKeyStatus_t(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SerialThread::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialThread::openPort)) {
                *result = 0;
            }
        }
        {
            typedef void (SerialThread::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialThread::closePort)) {
                *result = 1;
            }
        }
        {
            typedef bool (SerialThread::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialThread::isPortOpened)) {
                *result = 2;
            }
        }
        {
            typedef void (SerialThread::*_t)(QByteArray );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialThread::sendData)) {
                *result = 3;
            }
        }
        {
            typedef void (SerialThread::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialThread::updateADC)) {
                *result = 4;
            }
        }
        {
            typedef void (SerialThread::*_t)(rpcKeyStatus_t );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialThread::updateKeyState)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject SerialThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SerialThread.data,
      qt_meta_data_SerialThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SerialThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SerialThread.stringdata))
        return static_cast<void*>(const_cast< SerialThread*>(this));
    return QObject::qt_metacast(_clname);
}

int SerialThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SerialThread::openPort(QString _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialThread::closePort()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
bool SerialThread::isPortOpened()
{
    bool _t0 = bool();
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
    return _t0;
}

// SIGNAL 3
void SerialThread::sendData(QByteArray _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SerialThread::updateADC(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SerialThread::updateKeyState(rpcKeyStatus_t _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
