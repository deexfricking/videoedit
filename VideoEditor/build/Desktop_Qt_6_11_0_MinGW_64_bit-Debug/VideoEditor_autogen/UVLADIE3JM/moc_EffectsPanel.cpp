/****************************************************************************
** Meta object code from reading C++ file 'EffectsPanel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/EffectsPanel.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EffectsPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12EffectsPanelE_t {};
} // unnamed namespace

template <> constexpr inline auto EffectsPanel::qt_create_metaobjectdata<qt_meta_tag_ZN12EffectsPanelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EffectsPanel",
        "effectApplied",
        "",
        "TimelineClip",
        "clip",
        "Effect",
        "effect",
        "effectRemoved",
        "effectType",
        "onAddEffect",
        "onRemoveEffect",
        "onEffectValueChanged",
        "onEffectTypeChanged",
        "type",
        "updateEffectPreview"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'effectApplied'
        QtMocHelpers::SignalData<void(const TimelineClip &, const Effect &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 5, 6 },
        }}),
        // Signal 'effectRemoved'
        QtMocHelpers::SignalData<void(const TimelineClip &, const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::QString, 8 },
        }}),
        // Slot 'onAddEffect'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemoveEffect'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEffectValueChanged'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEffectTypeChanged'
        QtMocHelpers::SlotData<void(const QString &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'updateEffectPreview'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EffectsPanel, qt_meta_tag_ZN12EffectsPanelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EffectsPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12EffectsPanelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12EffectsPanelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12EffectsPanelE_t>.metaTypes,
    nullptr
} };

void EffectsPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EffectsPanel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->effectApplied((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Effect>>(_a[2]))); break;
        case 1: _t->effectRemoved((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->onAddEffect(); break;
        case 3: _t->onRemoveEffect(); break;
        case 4: _t->onEffectValueChanged(); break;
        case 5: _t->onEffectTypeChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->updateEffectPreview(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EffectsPanel::*)(const TimelineClip & , const Effect & )>(_a, &EffectsPanel::effectApplied, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EffectsPanel::*)(const TimelineClip & , const QString & )>(_a, &EffectsPanel::effectRemoved, 1))
            return;
    }
}

const QMetaObject *EffectsPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EffectsPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12EffectsPanelE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int EffectsPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void EffectsPanel::effectApplied(const TimelineClip & _t1, const Effect & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void EffectsPanel::effectRemoved(const TimelineClip & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
QT_WARNING_POP
