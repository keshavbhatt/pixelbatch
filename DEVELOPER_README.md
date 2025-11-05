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
- **Columns:**
  1. File name (with elision)
  2. Status (Pending/Queued/Processing/Completed/Error)
  3. Size Before
  4. Size After
  5. Savings (bytes and percentage)

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
- jpegoptim
- pngquant
- pngout

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

