/****************************************************************************
** Meta object code from reading C++ file 'progressdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/gui/progressdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'progressdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProgressDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,   27,   15,   15, 0x0a,
      84,   57,   15,   15, 0x0a,
     133,  120,   15,   15, 0x0a,
     175,   15,   15,   15, 0x0a,
     200,   15,   15,   15, 0x0a,
     208,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ProgressDialog[] = {
    "ProgressDialog\0\0canceled()\0value\0"
    "set_progress_value(int)\0"
    "parameter,value,thread_idx\0"
    "set_delta_running(uint,double,uint)\0"
    "m,thread_idx\0set_model_running(modeltest::Model*,uint)\0"
    "optimized_single_model()\0reset()\0"
    "on_btn_stop_clicked()\0"
};

void ProgressDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProgressDialog *_t = static_cast<ProgressDialog *>(_o);
        switch (_id) {
        case 0: _t->canceled(); break;
        case 1: _t->set_progress_value((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->set_delta_running((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 3: _t->set_model_running((*reinterpret_cast< modeltest::Model*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 4: _t->optimized_single_model(); break;
        case 5: _t->reset(); break;
        case 6: _t->on_btn_stop_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ProgressDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ProgressDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ProgressDialog,
      qt_meta_data_ProgressDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProgressDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProgressDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProgressDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProgressDialog))
        return static_cast<void*>(const_cast< ProgressDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ProgressDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ProgressDialog::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
