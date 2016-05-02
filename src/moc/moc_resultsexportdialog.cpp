/****************************************************************************
** Meta object code from reading C++ file 'resultsexportdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/gui/resultsexportdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'resultsexportdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ResultsExportDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,
      47,   20,   20,   20, 0x08,
      79,   73,   20,   20, 0x08,
     124,  115,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ResultsExportDialog[] = {
    "ResultsExportDialog\0\0on_tool_outfile_clicked()\0"
    "on_box_buttons_accepted()\0value\0"
    "on_slider_nmodels_valueChanged(int)\0"
    "newstate\0on_cb_tofile_stateChanged(int)\0"
};

void ResultsExportDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ResultsExportDialog *_t = static_cast<ResultsExportDialog *>(_o);
        switch (_id) {
        case 0: _t->on_tool_outfile_clicked(); break;
        case 1: _t->on_box_buttons_accepted(); break;
        case 2: _t->on_slider_nmodels_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_cb_tofile_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ResultsExportDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ResultsExportDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ResultsExportDialog,
      qt_meta_data_ResultsExportDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ResultsExportDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ResultsExportDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ResultsExportDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ResultsExportDialog))
        return static_cast<void*>(const_cast< ResultsExportDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ResultsExportDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
