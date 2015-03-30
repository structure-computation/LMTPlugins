/****************************************************************************
** Meta object code from reading C++ file 'Launcher.h'
**
** Created: Tue Mar 24 13:23:14 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Launcher.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Launcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Launcher[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      21,    9,    9,    9, 0x0a,
      35,    9,   31,    9, 0x0a,
      67,   47,    9,    9, 0x0a,
      88,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Launcher[] = {
    "Launcher\0\0finished()\0run_app()\0int\0"
    "run_app_2()\0tic,output_launcher\0"
    "log_tool(TicToc,int)\0launch()\0"
};

void Launcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Launcher *_t = static_cast<Launcher *>(_o);
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->run_app(); break;
        case 2: { int _r = _t->run_app_2();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: _t->log_tool((*reinterpret_cast< TicToc(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->launch(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Launcher::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Launcher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Launcher,
      qt_meta_data_Launcher, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Launcher::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Launcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Launcher::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Launcher))
        return static_cast<void*>(const_cast< Launcher*>(this));
    return QObject::qt_metacast(_clname);
}

int Launcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Launcher::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
