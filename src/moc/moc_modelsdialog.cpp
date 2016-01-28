/****************************************************************************
** Meta object code from reading C++ file 'modelsdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/gui/modelsdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelsdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ModelsDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   14,   13,   13, 0x08,
      62,   13,   13,   13, 0x08,
      90,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ModelsDialog[] = {
    "ModelsDialog\0\0index\0"
    "on_cmb_partition_currentIndexChanged(int)\0"
    "on_action_close_triggered()\0"
    "on_toolButton_clicked()\0"
};

void ModelsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ModelsDialog *_t = static_cast<ModelsDialog *>(_o);
        switch (_id) {
        case 0: _t->on_cmb_partition_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_action_close_triggered(); break;
        case 2: _t->on_toolButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ModelsDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ModelsDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ModelsDialog,
      qt_meta_data_ModelsDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ModelsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ModelsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ModelsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelsDialog))
        return static_cast<void*>(const_cast< ModelsDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int ModelsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
