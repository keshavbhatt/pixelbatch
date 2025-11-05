# PixelBatch - Developer Documentation

## Table of Contents
- [Overview](#overview)
- [Technology Stack](#technology-stack)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
- [Core Components](#core-components)
- [Build System](#build-system)
- [Development Workflow](#development-workflow)
- [Key Design Patterns](#key-design-patterns)
- [Contributing](#contributing)

## Overview

PixelBatch is a Qt-based desktop application for batch processing and optimization of images. It provides a graphical interface for processing multiple images concurrently with various optimization tools, supporting formats like JPEG, PNG, GIF, and SVG.

**Version:** 1.0  
**License:** GNU GPL v3  
**Platform:** Linux (primary target)

## Technology Stack

- **Framework:** Qt 5+ (Core, Gui, Widgets)
- **Language:** C++17
- **Build System:** qmake (.pro files)
- **UI Design:** Qt Designer (.ui files)
- **External Tools:** jpegoptim, pngquant, pngout (called as external processes)

## Project Structure

```
PixelBatch/
├── src/                          # Main source directory
│   ├── *.cpp, *.h               # Core application files
│   ├── *.ui                     # Qt Designer UI files
│   ├── PixelBatch.pro           # Qt project file (qmake)
│   ├── resources.qrc            # Qt resource file
│   │
│   ├── OptimizerPrefWidgets/    # Optimizer preference UI components
│   │   ├── imageformatprefwidgetfactory.*
│   │   ├── imageoptimizerprefwidget.*
│   │   └── jpegoptimprefwidget.*
│   │
│   ├── worker/                  # Image processing workers
│   │   ├── ImageWorker.h        # Abstract base class for workers
│   │   ├── imageoptimizer.*     # Optimizer metadata
│   │   ├── imageworkerfactory.* # Factory for creating workers
│   │   ├── jpegoptimworker.*    # JPEG optimization worker
│   │   ├── pngquantworker.*     # PNG quantization worker
│   │   └── pngoutworker.*       # PNG optimization worker
│   │
│   ├── widegts/                 # Custom widgets (note: typo in folder name)
│   │   └── SlidingStackedWidget/
│   │
│   ├── resources/               # Application resources
│   │   └── icons/               # Icon assets
│   │
│   └── build/                   # Build output directory
│       └── Desktop-Debug/       # Debug build artifacts
│
├── snap/                         # Snapcraft packaging
│   ├── snapcraft.yaml
│   └── gui/
│       ├── icon.png
│       └── icon.svg
│
├── LICENSE                       # GNU GPL v3 license
├── README.md                     # User-facing README
├── TODO.txt                      # Development TODOs
└── DEVELOPER_README.md          # This file
```

## Architecture

### High-Level Architecture

PixelBatch follows a **Model-View-Controller (MVC)** pattern with Qt's signal/slot mechanism for component communication:

```
┌─────────────────┐
│  PixelBatch     │  Main Window (Controller)
│  (QMainWindow)  │
└────────┬────────┘
         │
         ├──────────────┬──────────────┬──────────────┬──────────────┐
         │              │              │              │              │
    ┌────▼─────┐  ┌────▼─────┐  ┌────▼─────┐  ┌────▼─────┐  ┌─────▼────┐
    │TaskWidget│  │ Prefs    │  │  File    │  │  Task    │  │ Settings │
    │          │  │ Widget   │  │ Handler  │  │ Action   │  │(Singleton)│
    └────┬─────┘  └──────────┘  └──────────┘  │ Widget   │  └──────────┘
         │                                     └──────────┘
         │
    ┌────▼──────────────┐
    │ ImageWorkerFactory│
    │   (Singleton)     │
    └────┬──────────────┘
         │
         ├──────────┬──────────┬──────────┐
         │          │          │          │
    ┌────▼────┐┌───▼────┐┌────▼────┐┌───▼────┐
    │JpegOptim││PngQuant││PngOut   ││  ...   │
    │Worker   ││Worker  ││Worker   ││        │
    └─────────┘└────────┘└─────────┘└────────┘
```

### Data Flow

1. **User adds images** → FileHandler validates → TaskWidget creates ImageTask
2. **User starts processing** → TaskWidget queues tasks → Workers process concurrently
3. **Worker completes** → Signals back to TaskWidget → UI updates
4. **Results displayed** → Statistics calculated → Status bar updated

## Core Components

### 1. Main Window & Application Entry

#### `main.cpp`
- Application entry point
- Sets up QApplication metadata (organization, version, etc.)
- Registers custom types with Qt's meta-object system
- Launches PixelBatch main window

#### `pixelbatch.h/cpp`
- **Type:** `QMainWindow` (Application controller)
- **Responsibilities:**
  - Orchestrates all major components
  - Manages menu bar, status bar, and toolbar
  - Connects signals/slots between components
  - Handles application-level actions (quit, about, settings)
- **Key Members:**
  - `m_taskWidget`: Main task management widget
  - `m_preferencesWidget`: Settings dialog
  - `m_taskActionWidget`: Context actions for selected tasks
  - `m_fileHandler`: File selection and validation
  - `m_settings`: Application settings singleton

### 2. Task Management

#### `taskwidget.h/cpp`
- **Type:** `QTableWidget` (View & Controller for tasks)
- **Responsibilities:**
  - Displays image tasks in tabular format
  - Manages task queue and concurrent processing
  - Handles drag-and-drop file operations
  - Updates task status and statistics
  - Coordinates worker thread pool
- **Key Features:**
  - Drag-and-drop support for adding files
  - Real-time status updates during processing
  - Task filtering (pending, completed, error)
  - Context menu operations (remove, open, view)
  - **Color-coded status indicators** for visual feedback
  - **Keyboard shortcuts** (Delete, Enter, Ctrl+A)
  - **Rich tooltips** with detailed information
  - **Smart duplicate detection**
  - **Compression quality indicators** with color coding
- **Columns:**
  1. File name (with elision and full path tooltip)
  2. Status (color-coded: gray/blue/orange/green/red)
  3. Size Before (with exact byte count tooltip)
  4. Size After (with exact byte count tooltip)
  5. Savings (with compression quality rating and color coding)

**UX Improvements:**
- Status colors: Pending (gray), Queued (blue), Processing (orange), Completed (green), Error (red)
- Savings colors: Excellent (≥30%, dark green), Good (15-29%, green), Moderate (5-14%, orange), Minimal (<5%, gray)
- Keyboard shortcuts: Delete to remove, Enter to view, Ctrl+A to select all
- Enhanced drag & drop with validation and user feedback
- Detailed progress messages showing active/remaining tasks
- See [TASKWIDGET_UX_IMPROVEMENTS.md](DOCS/TASKWIDGET_UX_IMPROVEMENTS.md) for complete details

#### `imagetask.h`
- **Type:** Struct (Data model)
- **Purpose:** Represents a single image optimization task
- **Fields:**
  - `imagePath`: Source image file path
  - `optimizedPath`: Destination (optimized) file path
  - `taskStatus`: Current status enum
- **Status States:**
  - `Pending`: Task created but not started
  - `Queued`: Waiting for worker thread
  - `Processing`: Currently being optimized
  - `Completed`: Successfully optimized
  - `Error`: Failed during processing

#### `taskactionwidget.h/cpp`
- **Type:** `QWidget`
- **Purpose:** Provides context-sensitive actions for selected tasks
- **Actions:**
  - Remove selected task
  - Open optimized image in file manager
  - View optimized image in image viewer
  - View original image in image viewer
- **Visibility:** Shown only when a task is selected

#### `taskwidgetoverlay.h/cpp`
- **Type:** `QWidget`
- **Purpose:** Displays overlay messages on TaskWidget (e.g., "Drop files here")
- **Usage:** Shown when table is empty to guide users

### 3. File Handling

#### `filehandler.h/cpp`
- **Type:** `QObject`
- **Responsibilities:**
  - Opens file selection dialog
  - Validates selected files (format, existence)
  - Remembers last opened directory
  - Emits signals to add valid files to TaskWidget
- **Supported Formats:** Defined by `ImageType` enum

### 4. Image Processing Workers

#### `worker/ImageWorker.h`
- **Type:** Abstract base class (`QObject`)
- **Purpose:** Defines interface for all image optimization workers
- **Key Methods:**
  - `optimize(ImageTask *task)`: Pure virtual method to implement
- **Signals:**
  - `optimizationFinished(ImageTask*, bool)`: Emitted on completion
  - `optimizationError(ImageTask*, QString)`: Emitted on failure
- **Implementation:** Workers execute external CLI tools via `QProcess`

#### `worker/imageworkerfactory.h/cpp`
- **Type:** Singleton factory
- **Responsibilities:**
  - Creates appropriate worker based on image type
  - Maintains registry of available optimizers
  - Maps file extensions to image types
- **Pattern:** Factory + Singleton
- **Key Method:** `getWorker(filePath)` → Returns appropriate worker instance

#### `worker/imageoptimizer.h/cpp`
- **Type:** Data class
- **Purpose:** Metadata about an optimizer tool
- **Fields:**
  - `optimizerName`: Tool name (e.g., "jpegoptim")
  - `supportedFormats`: File extensions
  - `imageType`: ImageType enum

#### Concrete Workers

##### `worker/jpegoptimworker.h/cpp`
- **Tool:** jpegoptim (external)
- **Purpose:** Lossless JPEG optimization
- **Process:** Executes jpegoptim CLI with configured parameters

##### `worker/pngquantworker.h/cpp`
- **Tool:** pngquant (external)
- **Purpose:** PNG quantization and compression

##### `worker/pngoutworker.h/cpp`
- **Tool:** pngout (external)
- **Purpose:** PNG optimization

### 5. Preferences & Settings

#### `settings.h/cpp`
- **Type:** Singleton (`QObject`)
- **Purpose:** Persistent application settings using `QSettings`
- **Backed By:** Platform-specific storage (INI file on Linux)
- **Key Settings:**
  - `optimizedPath`: Output directory for optimized images
  - `outputFilePrefix`: Prefix for optimized filenames
  - `lastOpenedImageDirPath`: Last directory user browsed
  - `maxConcurrentTasks`: Thread pool size
  - `rememberOpenLastOpenedPath`: Boolean preference
- **Pattern:** Singleton with lazy initialization

#### `preferenceswidget.h/cpp`
- **Type:** `QWidget` (Dialog)
- **Purpose:** UI for modifying application settings
- **Layout:** Defined in `preferenceswidget.ui`
- **Integrations:**
  - Reads/writes through Settings singleton
  - Validates user input before saving

#### `constants.h/cpp`
- **Type:** Static constants class
- **Purpose:** Centralized definition of setting keys and defaults
- **Usage:** Referenced by Settings class

### 6. Optimizer Preferences

#### `OptimizerPrefWidgets/imageoptimizerprefwidget.h/cpp`
- **Type:** Abstract base widget
- **Purpose:** Base class for format-specific optimization settings

#### `OptimizerPrefWidgets/jpegoptimprefwidget.h/cpp`
- **Type:** `QWidget`
- **Purpose:** JPEG-specific optimization preferences
- **Layout:** Defined in `jpegoptimprefwidget.ui`

#### `OptimizerPrefWidgets/imageformatprefwidgetfactory.h/cpp`
- **Type:** Factory
- **Purpose:** Creates appropriate preference widget for image format
- **Pattern:** Factory pattern

### 7. Utility Components

#### `imagestats.h/cpp`
- **Type:** Utility class
- **Purpose:** Calculates statistics for optimized images
- **Metrics:**
  - Original file size
  - Optimized file size
  - Bytes saved
  - Compression percentage

#### `imagetype.h`
- **Type:** Enum class + utility
- **Purpose:** Defines supported image formats
- **Types:** JPG, PNG, GIF, SVG, Unsupported

#### `elideditemdelegate.h/cpp`
- **Type:** `QStyledItemDelegate`
- **Purpose:** Custom delegate for eliding long filenames in table
- **Usage:** Applied to TaskWidget's filename column

#### `imageformatprefwidget.h/cpp`
- **Type:** `QWidget`
- **Purpose:** Widget for selecting image format preferences
- **Layout:** `imageformatprefwidget.ui`

## Build System

### qmake Project File (`PixelBatch.pro`)

The project uses Qt's qmake build system. Key configurations:

```qmake
QT += core gui widgets
CONFIG += c++17
TARGET = pixelbatch
TEMPLATE = app
VERSION = 1.0
```

### Build Configurations

- **Debug:** Includes all debug symbols and output
- **Release:** Optimized, strips debug output via `QT_NO_DEBUG_OUTPUT`

### Versioning

Version information is embedded at compile time:
- `VERSION`: From .pro file
- `GIT_HASH`: Current git commit
- `GIT_BRANCH`: Current git branch
- `BUILD_TIMESTAMP`: UTC build time

### Dependencies

**Qt Modules:**
- Qt5Core
- Qt5Gui
- Qt5Widgets

**External Tools (runtime):**
- jpegoptim (v1.5.6+)
- pngquant (v3.0.3+ recommended)
- gifsicle (v1.95+)

### JPEG Optimization Settings

The application uses **jpegoptim v1.5.6** for JPEG compression. Users can configure the following options through the JPEG preferences widget:

#### Quality Settings
- **Maximum Quality (0-100)**: Controls compression level
  - 100 = Lossless optimization (default)
  - < 100 = Lossy optimization with specified quality
  - Setting Key: `jpegoptim/maxQuality`
  
- **Target Size (KB)**: Optimize to specific file size
  - 0 = No target (default)
  - > 0 = Target size in kilobytes
  - Setting Key: `jpegoptim/targetSize`
  
- **Compression Threshold (%)**: Minimum savings to keep optimized file
  - 0 = Always optimize (default)
  - > 0 = Keep original if savings below threshold
  - Setting Key: `jpegoptim/compressionThreshold`

#### Metadata Handling
Configurable metadata stripping options:
- **Mode 0**: Keep all metadata (default) - `--strip-none`
- **Mode 1**: Strip all metadata - `--strip-all`
- **Mode 2**: Keep EXIF only - `--strip-all --keep-exif`
- **Mode 3**: Keep ICC profile only - `--strip-all --keep-icc`
- **Mode 4**: Keep EXIF + ICC - `--strip-all --keep-exif --keep-icc`
- Setting Key: `jpegoptim/metadataMode`

#### Output Format
- **Mode 0**: Auto mode (default) - `--auto-mode` - Chooses progressive/baseline automatically
- **Mode 1**: Force progressive JPEG - `--all-progressive`
- **Mode 2**: Force baseline JPEG - `--all-normal`
- Setting Key: `jpegoptim/outputMode`

#### Additional Options
- **Force Optimization**: Force optimization even if already optimized
  - Setting Key: `jpegoptim/force`
  - Default: false
  
- **Preserve Timestamps**: Keep original file modification times
  - Setting Key: `jpegoptim/preserveTimes`
  - Default: true
  
- **Retry**: Recursively optimize until no improvement
  - Setting Key: `jpegoptim/retry`
  - Default: false

#### Performance
- **Parallel Threads**: Number of threads jpegoptim uses per image
  - Range: 1-16
  - Default: 1
  - Setting Key: `jpegoptim/maxWorkers`
  - Note: This is per-image parallelism, separate from PixelBatch's task concurrency

### jpegoptim Command Construction

The worker builds commands like:
```bash
jpegoptim --quiet --max=90 --strip-all --keep-exif --auto-mode --preserve -- output.jpg
```

See `worker/jpegoptimworker.cpp` for full implementation details.

### PNG Optimization Settings

The application uses **pngquant v3.0.3** for PNG compression. pngquant provides lossy compression with quality control, achieving 40-70% file size reduction.

#### Quality Settings
- **Quality Range (Min-Max)**: Controls compression level
  - Min: 0-100 (default: 65) - Don't save if quality falls below
  - Max: 0-100 (default: 80) - Use fewer colors below this quality
  - Setting Keys: `pngquant/qualityMin`, `pngquant/qualityMax`
  - CLI: `--quality=MIN-MAX`
  
#### Speed vs Quality Trade-off
- **Speed**: Balances processing time and output quality
  - Range: 1-11 (default: 4)
  - 1 = Slowest, best quality
  - 4 = Default, balanced
  - 11 = Fastest, rougher quality
  - Setting Key: `pngquant/speed`
  - CLI: `--speed=N`

#### Dithering
- **Enable Floyd-Steinberg Dithering**: Improves gradient quality
  - true = Enable dithering (default, better gradients)
  - false = Disable (pixel art, simple graphics)
  - Setting Key: `pngquant/enableDithering`
  - CLI: `--nofs` (when disabled)

#### Color Precision (Advanced)
- **Posterize**: Reduces color precision
  - Range: 0-8 (default: 0 = disabled)
  - Use for ARGB4444 or reduced color output
  - Setting Key: `pngquant/posterize`
  - CLI: `--posterize=N` (when > 0)

#### Additional Options
- **Strip Metadata**: Remove optional chunks
  - Setting Key: `pngquant/stripMetadata`
  - Default: true
  - CLI: `--strip`
  
- **Skip if Larger**: Keep original if optimization increases size
  - Setting Key: `pngquant/skipIfLarger`
  - Default: true
  - CLI: `--skip-if-larger`
  
- **Force Overwrite**: Overwrite existing files
  - Setting Key: `pngquant/force`
  - Default: true
  - CLI: `--force`

### pngquant Command Construction

The worker builds commands like:
```bash
pngquant --quality=65-80 --speed=4 --output=out.png --force --strip --skip-if-larger -- input.png
```

**Why pngquant over pngout?**
- Superior compression (40-70% vs 5-15%)
- Quality control (min-max range)
- Modern & actively maintained (v3.0.3, Rust)
- Better dithering (Floyd-Steinberg)
- Smart skip-if-larger protection

See `worker/pngquantworker.cpp` for full implementation details.

### GIF Optimization Settings

The application uses **gifsicle v1.95** for GIF compression. gifsicle supports both lossless and lossy compression for static and animated GIFs, achieving 10-80% file size reduction.

#### Optimization Level
- **Level**: Controls optimization depth
  - 1 = Basic optimization (default: -O1, fast)
  - 2 = Normal optimization (default, balanced)
  - 3 = Maximum optimization (-O3, slower but best)
  - Setting Key: `gifsicle/optimizationLevel`
  - CLI: `-O1`, `-O2`, or `-O3`

#### Compression Type
- **Type**: Lossless or Lossy
  - 0 = Lossless (no quality loss, 10-30% reduction)
  - 1 = Lossy (quality trade-off, 30-60% reduction)
  - Setting Key: `gifsicle/compressionType`
  
- **Lossy Level**: Controls compression aggressiveness
  - Range: 1-200 (default: 80)
  - Lower = better quality, larger file
  - Higher = more compression, artifacts
  - Setting Key: `gifsicle/lossyLevel`
  - CLI: `--lossy=N` (when type=1)

#### Color Reduction
- **Reduce Colors**: Enable palette reduction
  - Setting Key: `gifsicle/reduceColors` (default: false)
  - CLI: `--colors=N`
  
- **Color Count**: Maximum palette colors
  - Range: 2-256 (default: 256)
  - Setting Key: `gifsicle/colorCount`
  - CLI: `--colors=N`
  
- **Color Method**: Algorithm for choosing colors
  - 0 = diversity (default, best overall)
  - 1 = blend-diversity (smooth gradients)
  - 2 = median-cut (traditional)
  - Setting Key: `gifsicle/colorMethod`
  - CLI: `--color-method=METHOD`

#### Dithering
- **Enable Dithering**: Apply when reducing colors
  - Setting Key: `gifsicle/enableDithering` (default: true)
  - CLI: `-f` or `--dither`
  - Improves gradient appearance

#### Additional Options
- **Crop Transparency**: Remove transparent borders
  - Setting Key: `gifsicle/cropTransparency` (default: true)
  - CLI: `--crop-transparency`
  
- **Interlace**: Progressive loading
  - Setting Key: `gifsicle/interlace` (default: false)
  - CLI: `-i` or `--interlace`
  
- **Threads**: Multi-threaded processing
  - Range: 1-16 (default: 4)
  - Setting Key: `gifsicle/threads`
  - CLI: `-j` or `--threads=N`

### gifsicle Command Construction

The worker builds commands like:

**Lossless Level 3:**
```bash
gifsicle -O3 --crop-transparency -j4 -o output.gif input.gif
```

**Lossy Level 3 (lossy=80):**
```bash
gifsicle -O3 --lossy=80 --crop-transparency -j4 -o output.gif input.gif
```

**Lossy + Color Reduction:**
```bash
gifsicle -O3 --lossy=80 --colors=256 --color-method=diversity -f --crop-transparency -j4 -o output.gif input.gif
```

**Why gifsicle?**
- Purpose-built for GIF optimization
- Both lossless and lossy compression
- Animation-aware optimization
- Multi-threading support
- Mature, proven, reliable (since 1995)
- Color palette reduction capabilities

See `worker/gifsicleworker.cpp` for full implementation details.

## Development Workflow

### Setting Up Development Environment

1. **Install Qt:**
   ```bash
   sudo apt-get install qt5-default qtcreator
   ```

2. **Install Dependencies:**
   ```bash
   sudo apt-get install jpegoptim pngquant
   ```

3. **Clone & Build:**
   ```bash
   git clone <repository-url>
   cd PixelBatch/src
   qmake PixelBatch.pro
   make
   ```

4. **Run:**
   ```bash
   ./pixelbatch
   ```

### Project Organization

- **Headers (.h):** Class declarations, interfaces
- **Implementation (.cpp):** Class definitions
- **UI Files (.ui):** Qt Designer XML layouts
- **Resources (.qrc):** Icons, images, other resources

### Coding Conventions

- **Naming:**
  - Classes: `PascalCase`
  - Methods: `camelCase`
  - Member variables: `m_camelCase`
  - Constants: `UPPER_SNAKE_CASE`
- **Qt Conventions:**
  - Use Qt types (`QString`, `QList`, etc.)
  - Connect signals/slots for communication
  - Use Q_OBJECT macro for classes with signals/slots
  - Parent-child memory management

## Key Design Patterns

### 1. Singleton Pattern
- **Classes:** `Settings`, `ImageWorkerFactory`
- **Purpose:** Ensure single instance with global access
- **Implementation:**
  ```cpp
  static Settings& instance() {
      static Settings instance;
      return instance;
  }
  ```

### 2. Factory Pattern
- **Classes:** `ImageWorkerFactory`, `ImageFormatPrefWidgetFactory`
- **Purpose:** Create objects without specifying exact class
- **Usage:** Create workers based on image file extension

### 3. Observer Pattern (Qt Signals/Slots)
- **Usage:** Throughout application for component communication
- **Example:**
  ```cpp
  connect(worker, &ImageWorker::optimizationFinished,
          taskWidget, &TaskWidget::onOptimizationFinished);
  ```

### 4. Template Method (ImageWorker)
- **Pattern:** Base class defines algorithm structure
- **Implementation:** Subclasses implement `optimize()` method

### 5. Delegate Pattern (ElidedItemDelegate)
- **Purpose:** Custom rendering for table cells
- **Usage:** Elide long filenames in TaskWidget

## Key Workflows

### Image Processing Flow

```
User Adds Image
    ↓
FileHandler validates file
    ↓
TaskWidget creates ImageTask (status: Pending)
    ↓
User clicks "Process Images"
    ↓
TaskWidget queues tasks (status: Queued)
    ↓
ImageWorkerFactory creates appropriate worker
    ↓
Worker executes optimization (status: Processing)
    ↓
Worker emits optimizationFinished signal
    ↓
TaskWidget updates task (status: Completed/Error)
    ↓
ImageStats calculates savings
    ↓
UI displays results
```

### Concurrency Model

- **Thread Pool:** Qt's `QThreadPool` (global instance)
- **Max Concurrent Tasks:** Configurable in settings
- **Queue Management:** TaskWidget maintains queue
- **Thread Safety:** Workers communicate via signals (thread-safe)

## Testing

Currently, the project does not include automated tests. Areas for testing:

- Unit tests for ImageStats calculations
- Integration tests for worker execution
- UI tests for TaskWidget operations
- Mock tests for external process calls

## Contributing

### Adding a New Image Optimizer

1. **Create Worker Class:**
   ```cpp
   // worker/newoptimworker.h
   class NewOptimWorker : public ImageWorker {
       Q_OBJECT
   public:
       void optimize(ImageTask *task) override;
   };
   ```

2. **Register in Factory:**
   ```cpp
   // worker/imageworkerfactory.cpp
   ImageWorkerFactory::createImageOptimizers() {
       // Add new optimizer metadata
   }
   ```

3. **Add Preference Widget (Optional):**
   ```cpp
   // OptimizerPrefWidgets/newoptimprefwidget.h
   class NewOptimPrefWidget : public ImageOptimizerPrefWidget { ... };
   ```

4. **Update .pro File:**
   ```qmake
   SOURCES += worker/newoptimworker.cpp
   HEADERS += worker/newoptimworker.h
   ```

### Code Review Checklist

- [ ] Follows Qt coding conventions
- [ ] Uses signals/slots for component communication
- [ ] Properly manages memory (Qt parent-child or smart pointers)
- [ ] Includes Q_OBJECT macro if using signals/slots
- [ ] Updates .pro file with new files
- [ ] Handles errors gracefully
- [ ] Provides user feedback for long operations

## Common Development Tasks

### Adding a New Setting

1. Add constant to `constants.h/cpp`
2. Add getter/setter to `settings.h/cpp`
3. Add UI control to `preferenceswidget.ui`
4. Connect UI to settings in `preferenceswidget.cpp`

### Adding a New Menu Action

1. Define action in `pixelbatch.cpp::initMenuBar()`
2. Connect to appropriate slot
3. Implement handler method

### Modifying UI Layout

1. Open `.ui` file in Qt Designer
2. Modify layout visually
3. Save and rebuild project

## Resources

- **Qt Documentation:** https://doc.qt.io/
- **Qt Designer Manual:** https://doc.qt.io/qt-5/qtdesigner-manual.html
- **C++17 Reference:** https://en.cppreference.com/w/cpp/17

## License

This project is licensed under the GNU General Public License v3.0. See [LICENSE](LICENSE) file for details.

---

**Maintainer:** [Keshav Bhatt]  
**Last Updated:** November 6, 2025

