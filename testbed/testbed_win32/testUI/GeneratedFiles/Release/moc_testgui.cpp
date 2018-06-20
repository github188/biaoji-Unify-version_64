/****************************************************************************
** Meta object code from reading C++ file 'testgui.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../testgui.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'testgui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_testgui_t {
    QByteArrayData data[16];
    char stringdata[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_testgui_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_testgui_t qt_meta_stringdata_testgui = {
    {
QT_MOC_LITERAL(0, 0, 7), // "testgui"
QT_MOC_LITERAL(1, 8, 5), // "start"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 5), // "flag1"
QT_MOC_LITERAL(4, 21, 5), // "flag2"
QT_MOC_LITERAL(5, 27, 5), // "flag3"
QT_MOC_LITERAL(6, 33, 6), // "minnum"
QT_MOC_LITERAL(7, 40, 7), // "buchang"
QT_MOC_LITERAL(8, 48, 10), // "zhizhennum"
QT_MOC_LITERAL(9, 59, 6), // "color1"
QT_MOC_LITERAL(10, 66, 6), // "color2"
QT_MOC_LITERAL(11, 73, 5), // "withe"
QT_MOC_LITERAL(12, 79, 10), // "xingzhuang"
QT_MOC_LITERAL(13, 90, 7), // "leixing"
QT_MOC_LITERAL(14, 98, 7), // "qufendu"
QT_MOC_LITERAL(15, 106, 5) // "fenxi"

    },
    "testgui\0start\0\0flag1\0flag2\0flag3\0"
    "minnum\0buchang\0zhizhennum\0color1\0"
    "color2\0withe\0xingzhuang\0leixing\0qufendu\0"
    "fenxi"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_testgui[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08 /* Private */,
       3,    0,   85,    2, 0x08 /* Private */,
       4,    0,   86,    2, 0x08 /* Private */,
       5,    0,   87,    2, 0x08 /* Private */,
       6,    0,   88,    2, 0x08 /* Private */,
       7,    0,   89,    2, 0x08 /* Private */,
       8,    0,   90,    2, 0x08 /* Private */,
       9,    0,   91,    2, 0x08 /* Private */,
      10,    0,   92,    2, 0x08 /* Private */,
      11,    0,   93,    2, 0x08 /* Private */,
      12,    0,   94,    2, 0x08 /* Private */,
      13,    0,   95,    2, 0x08 /* Private */,
      14,    0,   96,    2, 0x08 /* Private */,
      15,    0,   97,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void testgui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        testgui *_t = static_cast<testgui *>(_o);
        switch (_id) {
        case 0: _t->start(); break;
        case 1: _t->flag1(); break;
        case 2: _t->flag2(); break;
        case 3: _t->flag3(); break;
        case 4: _t->minnum(); break;
        case 5: _t->buchang(); break;
        case 6: _t->zhizhennum(); break;
        case 7: _t->color1(); break;
        case 8: _t->color2(); break;
        case 9: _t->withe(); break;
        case 10: _t->xingzhuang(); break;
        case 11: _t->leixing(); break;
        case 12: _t->qufendu(); break;
        case 13: _t->fenxi(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject testgui::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_testgui.data,
      qt_meta_data_testgui,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *testgui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *testgui::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_testgui.stringdata))
        return static_cast<void*>(const_cast< testgui*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int testgui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
