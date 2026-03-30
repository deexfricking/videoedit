# VideoEditor — C++ / Qt6 Non-Linear Video Editor

A full-featured, professional-grade non-linear video editor written in modern
C++17 using the Qt6 framework. Supports video, audio, and image tracks with a
multi-track timeline, frame-accurate playhead, and FFmpeg-based export.

---

## Features

| Feature | Details |
|---|---|
| **Multi-track Timeline** | Unlimited video + audio tracks, colour-coded clips |
| **Editing Tools** | Select, Razor (cut), Split at Playhead, Duplicate, Trim |
| **Import** | MP4, MOV, AVI, MKV, WEBM, MP3, WAV, AAC, FLAC, PNG, JPG, GIF, TIFF, WEBP… |
| **Thumbnail Generation** | Async FFmpeg frame extraction with memory cache |
| **Video Preview** | Qt Multimedia player with scrub, step-frame, volume control |
| **Crop Dialog** | Interactive drag-handle crop with rule-of-thirds overlay |
| **Canvas Settings** | 720p → 4K DCI presets, custom size, FPS, colour space |
| **Export** | FFmpeg export: H.264/H.265/VP9/ProRes, AAC/Opus/FLAC, CRF + bitrate |
| **Project Save/Load** | JSON `.veproj` format, recent files list |
| **Undo / Redo** | Qt command stack (Ctrl+Z / Ctrl+Y) |
| **Dark Theme** | Full Fusion dark palette + custom stylesheet |
| **Keyboard Shortcuts** | Space, V, C, S, D, T, arrows, Ctrl+I/E/Z/Y… |
| **Drag & Drop** | Drag media files from your file manager onto the window |
| **Snap** | Clip edges snap to playhead and other clip boundaries |

---

## File Structure

```
VideoEditor/
├── CMakeLists.txt                  Qt6 CMake build file
├── README.md                       This file
└── src/
    ├── main.cpp                    Entry point
    ├── MainWindow.h / .cpp         Main window — wires all panels together
    ├── Timeline.h  / .cpp          Multi-track timeline widget (ruler, clips, playhead)
    ├── TimelineClip.h              Clip & Track data structures
    ├── ToolBar.h   / .cpp          Edit tool selector (Select/Razor/Split/Dupe/Trim)
    ├── MediaBin.h  / .cpp          Import panel with thumbnail grid
    ├── VideoPlayer.h / .cpp        Preview player (Qt Multimedia)
    ├── ThumbnailGenerator.h / .cpp Async FFmpeg/Qt thumbnail extraction
    ├── CropDialog.h / .cpp         Interactive crop widget + numeric controls
    ├── CanvasSettingsDialog.h/.cpp Resolution/FPS/colour-space settings
    ├── ExportDialog.h / .cpp       FFmpeg export with codec/bitrate/CRF controls
    └── ProjectManager.h / .cpp     JSON project save/load + recent files
```

---

## Dependencies

### Required

| Dependency | Version | Purpose |
|---|---|---|
| **Qt6** | 6.4+ | UI framework, multimedia, concurrent |
| **CMake** | 3.16+ | Build system |
| **C++17** compiler | GCC 10+, Clang 12+, MSVC 2019+ | Language |

Qt6 modules used: `Core`, `Widgets`, `Multimedia`, `MultimediaWidgets`, `Concurrent`

### Optional but Strongly Recommended

| Dependency | Purpose |
|---|---|
| **FFmpeg** (libavcodec, libavformat, libavutil, libswscale, libswresample) | Video frame thumbnail extraction, export rendering |

Without FFmpeg, thumbnails show a filename placeholder and export uses a basic
concat demuxer without re-encoding.

---

## Build Instructions

### Ubuntu / Debian

```bash
# Install Qt6
sudo apt update
sudo apt install qt6-base-dev qt6-multimedia-dev libqt6multimedia6 \
                 libqt6multimediawidgets6 cmake build-essential

# Install FFmpeg development libraries (optional but recommended)
sudo apt install libavcodec-dev libavformat-dev libavutil-dev \
                 libswscale-dev libswresample-dev pkg-config

# Build
cd VideoEditor
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run
./VideoEditor
```

### macOS (Homebrew)

```bash
brew install qt@6 ffmpeg cmake

export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
export PKG_CONFIG_PATH="/opt/homebrew/opt/ffmpeg/lib/pkgconfig"

cd VideoEditor
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
make -j$(sysctl -n hw.ncpu)

./VideoEditor.app/Contents/MacOS/VideoEditor
```

### Windows (MSVC + vcpkg)

```powershell
# Install Qt6 from https://www.qt.io/download (use Qt Online Installer)
# Install FFmpeg via vcpkg
vcpkg install ffmpeg:x64-windows

cd VideoEditor
mkdir build; cd build
cmake .. -DCMAKE_BUILD_TYPE=Release `
         -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64" `
         -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build . --config Release

.\Release\VideoEditor.exe
```

---

## Keyboard Shortcuts

| Key | Action |
|---|---|
| `Space` | Play / Pause |
| `Escape` | Stop |
| `←` / `→` | Step backward / forward one frame |
| `Home` / `End` | Jump to start / end |
| `V` | Select tool |
| `C` | Razor (cut) tool |
| `T` | Trim tool |
| `S` | Split clip at playhead |
| `D` | Duplicate selected clip |
| `Delete` | Delete selected clips |
| `Ctrl+Z` | Undo |
| `Ctrl+Y` | Redo |
| `Ctrl+I` | Import media |
| `Ctrl+E` | Export |
| `Ctrl+S` | Save project |
| `Ctrl+N` | New project |
| `Ctrl+O` | Open project |
| `Ctrl+A` | Select all clips |
| `Ctrl+=` | Zoom in timeline |
| `Ctrl+-` | Zoom out timeline |
| `Ctrl+0` | Reset zoom |
| `Ctrl+Shift+S` | Canvas settings |
| `Ctrl+Shift+C` | Crop selected clip |

---

## Usage Guide

### Importing Media
1. Click **+ Import** in the Media Bin (left panel), or press `Ctrl+I`
2. Select one or more video, audio, or image files
3. Thumbnails are generated asynchronously in the background
4. Double-click a media item to preview it in the player

### Adding Clips to the Timeline
- Double-click a clip in the Media Bin → it loads in the preview player
- Drag from the Media Bin and drop onto a timeline track
- (Or use File → Import which auto-places at end of first compatible track)

### Editing
- **Select tool (V):** Click to select clips, drag to move, drag edges to trim
- **Razor tool (C):** Click anywhere on a clip to cut it at that point
- **Split (S):** Splits the clip under the playhead
- **Duplicate (D):** Copies selected clips and places them immediately after
- **Trim tool (T):** Drag-only mode focused on in/out point adjustment
- Hold `Shift` to select multiple clips

### Crop
1. Select a clip on the timeline
2. Go to Clip → Crop… (`Ctrl+Shift+C`)
3. Drag the crop handles or enter values numerically
4. Rule-of-thirds grid is displayed for composition guidance

### Canvas / Resolution
- Clip → Canvas Settings (`Ctrl+Shift+S`)
- Choose from presets (720p, 1080p, 4K, Instagram, TikTok) or enter a custom size
- Set frame rate, sample rate, colour space, and pixel format

### Exporting
1. File → Export (`Ctrl+E`)
2. Choose output path and format (MP4, MOV, MKV, WEBM, GIF…)
3. Select codec (H.264, H.265, VP9, ProRes), CRF quality, and speed preset
4. Set audio codec and bitrate
5. Click Export — requires FFmpeg in system PATH

### Saving / Loading Projects
- Projects are saved as `.veproj` files (human-readable JSON)
- All clip positions, in/out points, track layout, and canvas settings are preserved
- Source media files are referenced by absolute path (not embedded)

---

## Architecture Notes

- **Timeline rendering** is entirely custom-painted with QPainter — no Qt widgets
  inside the timeline itself, which keeps performance high even with many clips.
- **Undo/redo** uses Qt's QUndoStack + QUndoCommand pattern; every clip
  move and add is a discrete undoable command.
- **Thumbnails** are extracted on a background thread pool via QtConcurrent and
  cached in a mutex-protected QMap.
- **Export** shells out to FFmpeg via QProcess using an ffconcat demuxer file.
  For full compositing export (multiple tracks mixed), a render pipeline using
  libavfilter would be the next step.
- **ProjectManager** serialises everything to JSON using Qt's built-in
  QJsonDocument — no third-party serialisation library needed.

---

## Extending the Editor

Some features left as exercises for further development:

- **Effects/Filters** — add a clip property panel with brightness/contrast/colour grading
- **Transitions** — cross-dissolve, wipe etc. between adjacent clips
- **Speed Ramp** — variable playback speed per clip (time stretch via libsoundtouch)
- **Text/Title** — overlay QGraphicsTextItem rendered into the video frame
- **Multi-camera** — sync multiple angles by audio waveform matching
- **Audio mixer** — per-track volume envelopes with keyframe automation
- **Proxy editing** — auto-generate low-res proxies for smoother editing of 4K footage
- **GPU rendering** — replace QPainter timeline with a QOpenGLWidget surface

---

## Licence

MIT — use freely in personal and commercial projects.
