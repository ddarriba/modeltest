/****************************************************************************
** Meta object code from reading C++ file 'progressdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/progressdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'progressdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_modeltest__ProgressDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   38,   26,   26, 0x0a,
      63,   58,   26,   26, 0x0a,
      87,   85,   26,   26, 0x0a,
     104,   26,   26,   26, 0x0a,
     112,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_modeltest__ProgressDialog[] = {
    "modeltest::ProgressDialog\0\0canceled()\0"
    "value\0setValue(int)\0text\0setLabelText(QString)\0"
    "m\0addModel(Model*)\0reset()\0"
    "on_btnStop_clicked()\0"
};

void modeltest::ProgressDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProgressDialog *_t = static_cast<ProgressDialog *>(_o);
        switch (_id) {
        case 0: _t->canceled(); break;
        case 1: _t->setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setLabelText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->addModel((*reinterpret_cast< Model*(*)>(_a[1]))); break;
        case 4: _t->reset(); break;
        case 5: _t->on_btnStop_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData modeltest::ProgressDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject modeltest::ProgressDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_modeltest__ProgressDialog,
      qt_meta_data_modeltest__ProgressDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &modeltest::ProgressDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *modeltest::ProgressDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *modeltest::ProgressDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_modeltest__ProgressDialog))
        return static_cast<void*>(const_cast< ProgressDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int modeltest::ProgressDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void modeltest::ProgressDialog::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
