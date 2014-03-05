/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[30];
    char stringdata[266];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 4),
QT_MOC_LITERAL(2, 16, 0),
QT_MOC_LITERAL(3, 17, 4),
QT_MOC_LITERAL(4, 22, 8),
QT_MOC_LITERAL(5, 31, 8),
QT_MOC_LITERAL(6, 40, 4),
QT_MOC_LITERAL(7, 45, 6),
QT_MOC_LITERAL(8, 52, 4),
QT_MOC_LITERAL(9, 57, 5),
QT_MOC_LITERAL(10, 63, 5),
QT_MOC_LITERAL(11, 69, 19),
QT_MOC_LITERAL(12, 89, 19),
QT_MOC_LITERAL(13, 109, 10),
QT_MOC_LITERAL(14, 120, 10),
QT_MOC_LITERAL(15, 131, 8),
QT_MOC_LITERAL(16, 140, 9),
QT_MOC_LITERAL(17, 150, 9),
QT_MOC_LITERAL(18, 160, 9),
QT_MOC_LITERAL(19, 170, 6),
QT_MOC_LITERAL(20, 177, 6),
QT_MOC_LITERAL(21, 184, 6),
QT_MOC_LITERAL(22, 191, 7),
QT_MOC_LITERAL(23, 199, 10),
QT_MOC_LITERAL(24, 210, 5),
QT_MOC_LITERAL(25, 216, 10),
QT_MOC_LITERAL(26, 227, 8),
QT_MOC_LITERAL(27, 236, 4),
QT_MOC_LITERAL(28, 241, 10),
QT_MOC_LITERAL(29, 252, 12)
    },
    "MainWindow\0open\0\0save\0openMask\0saveMask\0"
    "undo\0repeat\0copy\0paste\0print\0"
    "resizeButtonClicked\0removeButtonClicked\0"
    "cairRemove\0cairResize\0newWidth\0newHeight\0"
    "clearMask\0paintMask\0oldPos\0newPos\0"
    "zoomIn\0zoomOut\0normalSize\0about\0"
    "changeView\0QAction*\0view\0updateView\0"
    "updateCursor\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  124,    2, 0x08,
       3,    0,  125,    2, 0x08,
       4,    0,  126,    2, 0x08,
       5,    0,  127,    2, 0x08,
       6,    0,  128,    2, 0x08,
       7,    0,  129,    2, 0x08,
       8,    0,  130,    2, 0x08,
       9,    0,  131,    2, 0x08,
      10,    0,  132,    2, 0x08,
      11,    0,  133,    2, 0x08,
      12,    0,  134,    2, 0x08,
      13,    0,  135,    2, 0x08,
      14,    2,  136,    2, 0x08,
      17,    0,  141,    2, 0x08,
      18,    2,  142,    2, 0x08,
      21,    0,  147,    2, 0x08,
      22,    0,  148,    2, 0x08,
      23,    0,  149,    2, 0x08,
      24,    0,  150,    2, 0x08,
      25,    1,  151,    2, 0x08,
      28,    0,  154,    2, 0x08,
      29,    0,  155,    2, 0x08,

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
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPointF, QMetaType::QPointF,   19,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 26,   27,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->open(); break;
        case 1: _t->save(); break;
        case 2: _t->openMask(); break;
        case 3: _t->saveMask(); break;
        case 4: _t->undo(); break;
        case 5: _t->repeat(); break;
        case 6: _t->copy(); break;
        case 7: _t->paste(); break;
        case 8: _t->print(); break;
        case 9: _t->resizeButtonClicked(); break;
        case 10: _t->removeButtonClicked(); break;
        case 11: _t->cairRemove(); break;
        case 12: _t->cairResize((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->clearMask(); break;
        case 14: _t->paintMask((*reinterpret_cast< QPointF(*)>(_a[1])),(*reinterpret_cast< QPointF(*)>(_a[2]))); break;
        case 15: _t->zoomIn(); break;
        case 16: _t->zoomOut(); break;
        case 17: _t->normalSize(); break;
        case 18: _t->about(); break;
        case 19: _t->changeView((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 20: _t->updateView(); break;
        case 21: _t->updateCursor(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 19:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
