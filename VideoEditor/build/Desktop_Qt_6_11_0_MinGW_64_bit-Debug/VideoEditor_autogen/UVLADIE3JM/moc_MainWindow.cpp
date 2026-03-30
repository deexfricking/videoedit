/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/MainWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onNewProject",
        "",
        "onOpenProject",
        "onSaveProject",
        "onSaveProjectAs",
        "onImportMedia",
        "onExport",
        "onUndo",
        "onRedo",
        "onCut",
        "onCopy",
        "onPaste",
        "onDelete",
        "onSelectAll",
        "onCanvasSettings",
        "onCropSelected",
        "onToggleMediaBin",
        "visible",
        "onPlay",
        "onPause",
        "onStop",
        "onTogglePlayPause",
        "onSkipForward",
        "onSkipBackward",
        "onStepForward",
        "onStepBackward",
        "onPlayheadMoved",
        "ms",
        "updatePlaybackClip",
        "onMediaItemDoubleClicked",
        "MediaItem",
        "item",
        "onClipAddedToTimeline",
        "onClipSelected",
        "TimelineClip",
        "clip",
        "onClipDoubleClicked",
        "onKeyframeAdded",
        "posMs",
        "onOpenRecentFile",
        "updateStatusBar"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onNewProject'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenProject'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveProject'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveProjectAs'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImportMedia'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExport'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUndo'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRedo'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCut'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCopy'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPaste'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDelete'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSelectAll'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCanvasSettings'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCropSelected'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleMediaBin'
        QtMocHelpers::SlotData<void(bool)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 18 },
        }}),
        // Slot 'onPlay'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPause'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStop'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTogglePlayPause'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSkipForward'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSkipBackward'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStepForward'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStepBackward'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPlayheadMoved'
        QtMocHelpers::SlotData<void(qint64)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 28 },
        }}),
        // Slot 'updatePlaybackClip'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMediaItemDoubleClicked'
        QtMocHelpers::SlotData<void(const MediaItem &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 31, 32 },
        }}),
        // Slot 'onClipAddedToTimeline'
        QtMocHelpers::SlotData<void(const MediaItem &)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 31, 32 },
        }}),
        // Slot 'onClipSelected'
        QtMocHelpers::SlotData<void(const TimelineClip &)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 35, 36 },
        }}),
        // Slot 'onClipDoubleClicked'
        QtMocHelpers::SlotData<void(const TimelineClip &)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 35, 36 },
        }}),
        // Slot 'onKeyframeAdded'
        QtMocHelpers::SlotData<void(qint64)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 39 },
        }}),
        // Slot 'onOpenRecentFile'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateStatusBar'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onNewProject(); break;
        case 1: _t->onOpenProject(); break;
        case 2: _t->onSaveProject(); break;
        case 3: _t->onSaveProjectAs(); break;
        case 4: _t->onImportMedia(); break;
        case 5: _t->onExport(); break;
        case 6: _t->onUndo(); break;
        case 7: _t->onRedo(); break;
        case 8: _t->onCut(); break;
        case 9: _t->onCopy(); break;
        case 10: _t->onPaste(); break;
        case 11: _t->onDelete(); break;
        case 12: _t->onSelectAll(); break;
        case 13: _t->onCanvasSettings(); break;
        case 14: _t->onCropSelected(); break;
        case 15: _t->onToggleMediaBin((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->onPlay(); break;
        case 17: _t->onPause(); break;
        case 18: _t->onStop(); break;
        case 19: _t->onTogglePlayPause(); break;
        case 20: _t->onSkipForward(); break;
        case 21: _t->onSkipBackward(); break;
        case 22: _t->onStepForward(); break;
        case 23: _t->onStepBackward(); break;
        case 24: _t->onPlayheadMoved((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 25: _t->updatePlaybackClip(); break;
        case 26: _t->onMediaItemDoubleClicked((*reinterpret_cast<std::add_pointer_t<MediaItem>>(_a[1]))); break;
        case 27: _t->onClipAddedToTimeline((*reinterpret_cast<std::add_pointer_t<MediaItem>>(_a[1]))); break;
        case 28: _t->onClipSelected((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1]))); break;
        case 29: _t->onClipDoubleClicked((*reinterpret_cast<std::add_pointer_t<TimelineClip>>(_a[1]))); break;
        case 30: _t->onKeyframeAdded((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 31: _t->onOpenRecentFile(); break;
        case 32: _t->updateStatusBar(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 33;
    }
    return _id;
}
QT_WARNING_POP
