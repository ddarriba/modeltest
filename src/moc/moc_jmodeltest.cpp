/****************************************************************************
** Meta object code from reading C++ file 'jmodeltest.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/jmodeltest.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jmodeltest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_modeltest__jModelTest[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      38,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      29,   23,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   22,   22,   22, 0x08,
      81,   22,   22,   22, 0x08,
     106,   22,   22,   22, 0x08,
     131,   22,   22,   22, 0x08,
     156,   22,   22,   22, 0x08,
     182,   22,   22,   22, 0x08,
     209,   22,   22,   22, 0x08,
     231,   22,   22,   22, 0x08,
     260,   22,   22,   22, 0x08,
     287,   22,   22,   22, 0x08,
     312,   22,   22,   22, 0x08,
     345,  337,   22,   22, 0x08,
     378,  337,   22,   22, 0x08,
     409,  337,   22,   22, 0x08,
     438,  337,   22,   22, 0x08,
     467,   22,   22,   22, 0x08,
     515,  506,   22,   22, 0x08,
     546,   22,   22,   22, 0x08,
     573,   22,   22,   22, 0x08,
     601,   22,   22,   22, 0x08,
     629,  337,   22,   22, 0x08,
     658,  337,   22,   22, 0x08,
     684,  337,   22,   22, 0x08,
     719,  337,   22,   22, 0x08,
     746,  337,   22,   22, 0x08,
     773,  337,   22,   22, 0x08,
     801,   22,   22,   22, 0x08,
     829,   22,   22,   22, 0x08,
     849,   22,   22,   22, 0x08,
     879,  873,   22,   22, 0x08,
     915,   22,   22,   22, 0x08,
     939,  337,   22,   22, 0x08,
     968,  337,   22,   22, 0x08,
     997,  337,   22,   22, 0x08,
    1030,  337,   22,   22, 0x08,
    1063,   22,   22,   22, 0x08,
    1088,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_modeltest__jModelTest[] = {
    "modeltest::jModelTest\0\0model\0"
    "finishedModel(Model*)\0"
    "on_btnLoadAlignment_clicked()\0"
    "on_radSchemes3_clicked()\0"
    "on_radSchemes5_clicked()\0"
    "on_radSchemes7_clicked()\0"
    "on_radSchemes11_clicked()\0"
    "on_radSchemes203_clicked()\0"
    "on_radTopoU_clicked()\0"
    "on_radSetModelTest_clicked()\0"
    "on_radSetMrbayes_clicked()\0"
    "on_radSetRaxml_clicked()\0"
    "on_radSetPhyml_clicked()\0checked\0"
    "on_radSetModelTest_toggled(bool)\0"
    "on_radSetMrbayes_toggled(bool)\0"
    "on_radSetRaxml_toggled(bool)\0"
    "on_radSetPhyml_toggled(bool)\0"
    "on_listMatrices_itemSelectionChanged()\0"
    "position\0on_sliderNCat_sliderMoved(int)\0"
    "on_actionReset_triggered()\0"
    "on_menuFileLoad_triggered()\0"
    "on_menuTreeLoad_triggered()\0"
    "on_cbEqualFreq_toggled(bool)\0"
    "on_cbMlFreq_toggled(bool)\0"
    "on_cbNoRateVarModels_toggled(bool)\0"
    "on_cbIModels_toggled(bool)\0"
    "on_cbGModels_toggled(bool)\0"
    "on_cbIGModels_toggled(bool)\0"
    "on_btnResetConfig_clicked()\0"
    "on_btnRun_clicked()\0on_cbAdvanced_clicked()\0"
    "value\0on_sliderNThreads_valueChanged(int)\0"
    "updateModelsTableSize()\0"
    "on_cbShowFreqs_toggled(bool)\0"
    "on_cbShowRates_toggled(bool)\0"
    "on_cbShowHetParams_toggled(bool)\0"
    "on_cbShowSelection_toggled(bool)\0"
    "on_btnLoadTree_clicked()\0cancel_jobs()\0"
};

void modeltest::jModelTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        jModelTest *_t = static_cast<jModelTest *>(_o);
        switch (_id) {
        case 0: _t->finishedModel((*reinterpret_cast< Model*(*)>(_a[1]))); break;
        case 1: _t->on_btnLoadAlignment_clicked(); break;
        case 2: _t->on_radSchemes3_clicked(); break;
        case 3: _t->on_radSchemes5_clicked(); break;
        case 4: _t->on_radSchemes7_clicked(); break;
        case 5: _t->on_radSchemes11_clicked(); break;
        case 6: _t->on_radSchemes203_clicked(); break;
        case 7: _t->on_radTopoU_clicked(); break;
        case 8: _t->on_radSetModelTest_clicked(); break;
        case 9: _t->on_radSetMrbayes_clicked(); break;
        case 10: _t->on_radSetRaxml_clicked(); break;
        case 11: _t->on_radSetPhyml_clicked(); break;
        case 12: _t->on_radSetModelTest_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->on_radSetMrbayes_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->on_radSetRaxml_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->on_radSetPhyml_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->on_listMatrices_itemSelectionChanged(); break;
        case 17: _t->on_sliderNCat_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->on_actionReset_triggered(); break;
        case 19: _t->on_menuFileLoad_triggered(); break;
        case 20: _t->on_menuTreeLoad_triggered(); break;
        case 21: _t->on_cbEqualFreq_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->on_cbMlFreq_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->on_cbNoRateVarModels_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->on_cbIModels_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: _t->on_cbGModels_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: _t->on_cbIGModels_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 27: _t->on_btnResetConfig_clicked(); break;
        case 28: _t->on_btnRun_clicked(); break;
        case 29: _t->on_cbAdvanced_clicked(); break;
        case 30: _t->on_sliderNThreads_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 31: _t->updateModelsTableSize(); break;
        case 32: _t->on_cbShowFreqs_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 33: _t->on_cbShowRates_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 34: _t->on_cbShowHetParams_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 35: _t->on_cbShowSelection_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 36: _t->on_btnLoadTree_clicked(); break;
        case 37: _t->cancel_jobs(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData modeltest::jModelTest::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject modeltest::jModelTest::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_modeltest__jModelTest,
      qt_meta_data_modeltest__jModelTest, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &modeltest::jModelTest::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *modeltest::jModelTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *modeltest::jModelTest::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_modeltest__jModelTest))
        return static_cast<void*>(const_cast< jModelTest*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int modeltest::jModelTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    }
    return _id;
}

// SIGNAL 0
void modeltest::jModelTest::finishedModel(Model * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
