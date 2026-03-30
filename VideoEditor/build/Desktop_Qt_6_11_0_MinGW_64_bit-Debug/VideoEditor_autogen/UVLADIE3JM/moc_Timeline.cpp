/****************************************************************************
** Meta object code from reading C++ file 'Timeline.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Timeline.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Timeline.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12TimelineViewE_t {};
} // unnamed namespace

template <> constexpr inline auto TimelineView::qt_create_metaobjectdata<qt_meta_tag_ZN12TimelineViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TimelineView",
        "playheadMoved",
        "",
        "ms",
        "clipSelected",
        "TimelineClip",
        "clip",
        "clipDoubleClicked",
        "requestScrub",
        "tracksChanged",
        "keyframeAdded",
        "posMs"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'playheadMoved'
        QtMocHelpers::SignalData<void(qint64)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 3 },
        }}),
        // Signal 'clipSelected'
        QtMocHelpers::SignalData<void(const TimelineClip &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'clipDoubleClicked'
        QtMocHelpers::SignalData<void(const TimelineClip &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'requestScrub'
        QtMocHelpers::SignalData<void(qint64)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 3 },
        }}),
        // Signal 'tracksChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'keyframeAdded'
        QtMocHelpers::SignalData<void(qint64)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TimelineView, qt_meta_tag_ZN12TimelineViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TimelineView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12TimelineViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12TimelineViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12TimelineViewE_t>.metaTypes,
    nullptr
} };

void TimelineView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TimelineView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->playheadMoved((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 1: _t->clipSelected((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1]))); break;
        case 2: _t->clipDoubleClicked((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1]))); break;
        case 3: _t->requestScrub((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 4: _t->tracksChanged(); break;
        case 5: _t->keyframeAdded((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TimelineView::*)(qint64 )>(_a, &TimelineView::playheadMoved, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TimelineView::*)(const TimelineClip & )>(_a, &TimelineView::clipSelected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TimelineView::*)(const TimelineClip & )>(_a, &TimelineView::clipDoubleClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (TimelineView::*)(qint64 )>(_a, &TimelineView::requestScrub, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (TimelineView::*)()>(_a, &TimelineView::tracksChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (TimelineView::*)(qint64 )>(_a, &TimelineView::keyframeAdded, 5))
            return;
    }
}

const QMetaObject *TimelineView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TimelineView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12TimelineViewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TimelineView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TimelineView::playheadMoved(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void TimelineView::clipSelected(const TimelineClip & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void TimelineView::clipDoubleClicked(const TimelineClip & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void TimelineView::requestScrub(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void TimelineView::tracksChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void TimelineView::keyframeAdded(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN8TimelineE_t {};
} // unnamed namespace

template <> constexpr inline auto Timeline::qt_create_metaobjectdata<qt_meta_tag_ZN8TimelineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Timeline",
        "playheadMoved",
        "",
        "ms",
        "clipSelected",
        "TimelineClip",
        "clip",
        "clipDoubleClicked",
        "requestScrub",
        "keyframeAdded",
        "posMs",
        "tracksModified",
        "setPlayheadMs",
        "onSplitAtPlayhead",
        "onDuplicateSelected",
        "onAddKeyframe",
        "onZoomIn",
        "onZoomOut",
        "onZoomReset"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'playheadMoved'
        QtMocHelpers::SignalData<void(qint64)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 3 },
        }}),
        // Signal 'clipSelected'
        QtMocHelpers::SignalData<void(const TimelineClip &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'clipDoubleClicked'
        QtMocHelpers::SignalData<void(const TimelineClip &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'requestScrub'
        QtMocHelpers::SignalData<void(qint64)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 3 },
        }}),
        // Signal 'keyframeAdded'
        QtMocHelpers::SignalData<void(qint64)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 10 },
        }}),
        // Signal 'tracksModified'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setPlayheadMs'
        QtMocHelpers::SlotData<void(qint64)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 3 },
        }}),
        // Slot 'onSplitAtPlayhead'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onDuplicateSelected'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onAddKeyframe'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onZoomIn'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onZoomOut'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onZoomReset'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Timeline, qt_meta_tag_ZN8TimelineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Timeline::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8TimelineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8TimelineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8TimelineE_t>.metaTypes,
    nullptr
} };

void Timeline::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Timeline *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->playheadMoved((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 1: _t->clipSelected((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1]))); break;
        case 2: _t->clipDoubleClicked((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1]))); break;
        case 3: _t->requestScrub((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 4: _t->keyframeAdded((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 5: _t->tracksModified(); break;
        case 6: _t->setPlayheadMs((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 7: _t->onSplitAtPlayhead(); break;
        case 8: _t->onDuplicateSelected(); break;
        case 9: _t->onAddKeyframe(); break;
        case 10: _t->onZoomIn(); break;
        case 11: _t->onZoomOut(); break;
        case 12: _t->onZoomReset(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Timeline::*)(qint64 )>(_a, &Timeline::playheadMoved, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Timeline::*)(const TimelineClip & )>(_a, &Timeline::clipSelected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Timeline::*)(const TimelineClip & )>(_a, &Timeline::clipDoubleClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Timeline::*)(qint64 )>(_a, &Timeline::requestScrub, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Timeline::*)(qint64 )>(_a, &Timeline::keyframeAdded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Timeline::*)()>(_a, &Timeline::tracksModified, 5))
            return;
    }
}

const QMetaObject *Timeline::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Timeline::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8TimelineE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Timeline::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void Timeline::playheadMoved(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Timeline::clipSelected(const TimelineClip & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void Timeline::clipDoubleClicked(const TimelineClip & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void Timeline::requestScrub(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void Timeline::keyframeAdded(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void Timeline::tracksModified()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
