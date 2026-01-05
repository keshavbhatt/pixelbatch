# Per-Task Optimization Settings

## Overview
Implemented per-task custom optimization settings, allowing each image to have its own optimizer configuration. Falls back to global settings if no custom settings are specified.

## Problem Solved
Previously, all optimization settings were global and applied to every image. Users couldn't customize settings for individual images. Any changes in the preferences widget affected all current and future tasks.

## Solution Architecture

### 1. ImageTask Data Structure (`imagetask.h`)

**Added Members:**
```cpp
QVariantMap customOptimizerSettings;  // Custom optimization settings for this task
```

**Added Helper Method:**
```cpp
bool hasCustomOptimizerSettings() const { return !customOptimizerSettings.isEmpty(); }
```

**Storage Format:**
Settings are stored as `QVariantMap` with keys matching QSettings keys:
```cpp
{
  "jpegoptim/maxQuality": 85,
  "jpegoptim/metadataMode": 1,
  "pngquant/qualityMin": 70,
  "pngquant/qualityMax": 90
}
```

### 2. ImageWorker Base Class (`worker/ImageWorker.h`)

**Added Method:**
```cpp
void setCustomSettings(const QVariantMap &settings);
```

**Added Member:**
```cpp
QVariantMap m_customSettings;
```

**Added Helper Method:**
```cpp
QVariant getSetting(const QString &key, const QVariant &defaultValue = QVariant()) const {
  // 1. Check custom settings first
  if (m_customSettings.contains(key)) {
    return m_customSettings.value(key);
  }
  // 2. Fall back to global QSettings
  const QSettings &settings = Settings::instance().getSettings();
  return settings.value(key, defaultValue);
}
```

**Setting Resolution Priority:**
1. Task-specific custom settings (if set)
2. Global QSettings (default)
3. Hard-coded default value (fallback)

### 3. Worker Implementations Updated

All workers updated to use `getSetting()` instead of direct `QSettings` access:

#### jpegoptimworker.cpp
```cpp
// Before
const QSettings &settings = Settings::instance().getSettings();
int maxQuality = settings.value("jpegoptim/maxQuality", 100).toInt();

// After
int maxQuality = getSetting("jpegoptim/maxQuality", 100).toInt();
```

#### pngquantworker.cpp
```cpp
// Before
const QSettings &settings = Settings::instance().getSettings();
int qualityMin = settings.value("pngquant/qualityMin", 65).toInt();

// After
int qualityMin = getSetting("pngquant/qualityMin", 65).toInt();
```

#### gifsicleworker.cpp
```cpp
// Before
int optimizationLevel = settings.value("gifsicle/optimizationLevel", 2).toInt();

// After
int optimizationLevel = getSetting("gifsicle/optimizationLevel", 2).toInt();
```

#### svgoworker.cpp
```cpp
// Before
int precision = settings.value("svgo/precision", 3).toInt();

// After
int precision = getSetting("svgo/precision", 3).toInt();
```

### 4. TaskWidget Integration (`taskwidget.h` & `taskwidget.cpp`)

**Added Methods:**
```cpp
void setTaskCustomOptimizerSettings(ImageTask *task, const QVariantMap &settings);
void clearTaskCustomOptimizerSettings(ImageTask *task);
```

**Processing Integration:**
In `processNextBatch()`, before calling `worker->optimize()`:
```cpp
// Apply task-specific settings if available
if (imageTask->hasCustomOptimizerSettings()) {
  worker->setCustomSettings(imageTask->customOptimizerSettings);
  qDebug() << "Applied custom settings to worker for" << imageTask->imagePath;
}
```

## Usage Flow

### Setting Custom Optimization Settings for a Task

```cpp
// Example: Set custom JPEG quality for a specific task
ImageTask *task = getSelectedImageTask();
QVariantMap customSettings;
customSettings["jpegoptim/maxQuality"] = 85;
customSettings["jpegoptim/metadataMode"] = 1;
setTaskCustomOptimizerSettings(task, customSettings);
```

### Processing with Custom Settings

1. Task is queued for processing
2. Worker is created for the task
3. If task has custom settings:
   - `worker->setCustomSettings(task->customOptimizerSettings)` is called
   - Worker stores custom settings in `m_customSettings`
4. Worker calls `getSetting()` for each parameter
5. `getSetting()` checks custom settings first, then falls back to global settings
6. Image is optimized with the resolved settings

### Clearing Custom Settings

```cpp
ImageTask *task = getSelectedImageTask();
clearTaskCustomOptimizerSettings(task);
// Task will now use global settings on next processing
```

## Integration with ImageDetailPanel

The ImageDetailPanel can now:

1. **Load Current Settings:**
   - If task has custom settings: Load from `task->customOptimizerSettings`
   - If no custom settings: Load from global QSettings
   
2. **Save Settings:**
   ```cpp
   // When user changes settings in the panel:
   QVariantMap settings;
   settings["jpegoptim/maxQuality"] = spinBox->value();
   settings["jpegoptim/metadataMode"] = comboBox->currentIndex();
   m_taskWidget->setTaskCustomOptimizerSettings(selectedTask, settings);
   ```

3. **Indicate Custom Settings:**
   - Show visual indicator (e.g., badge, color) when task has custom settings
   - Provide "Reset to Global" button to clear custom settings

## Benefits

✅ **Per-Task Customization** - Each image can have unique optimization settings
✅ **Non-Destructive** - Doesn't affect global settings or other tasks
✅ **Flexible Fallback** - Uses global settings when no custom settings specified
✅ **Re-processing Support** - Custom settings persist across re-processing
✅ **Type-Safe** - QVariantMap handles different data types automatically
✅ **Backward Compatible** - Existing code continues to work with global settings

## Example Scenarios

### Scenario 1: High-Quality Product Photos
```cpp
// User wants maximum quality for product photos
QVariantMap productSettings;
productSettings["jpegoptim/maxQuality"] = 95;
productSettings["jpegoptim/metadataMode"] = 2;  // Keep all metadata
setTaskCustomOptimizerSettings(productTask, productSettings);
```

### Scenario 2: Web Thumbnails
```cpp
// User wants aggressive compression for thumbnails
QVariantMap thumbnailSettings;
thumbnailSettings["jpegoptim/maxQuality"] = 70;
thumbnailSettings["jpegoptim/targetSize"] = 50;  // 50 KB max
setTaskCustomOptimizerSettings(thumbnailTask, thumbnailSettings);
```

### Scenario 3: Batch with Mixed Requirements
```cpp
// Process 100 images, but 5 need special treatment
for (ImageTask *task : allTasks) {
  if (needsHighQuality(task)) {
    QVariantMap highQuality;
    highQuality["jpegoptim/maxQuality"] = 98;
    setTaskCustomOptimizerSettings(task, highQuality);
  }
  // Other 95 tasks use global settings automatically
}
```

## Data Flow Diagram

```
┌─────────────────┐
│  ImageTask      │
│ customSettings  │◄─── User sets via ImageDetailPanel
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ processNextBatch│
│                 │
│ if hasCustom... │
│   setCustom...  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  ImageWorker    │
│ m_customSettings│
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  getSetting()   │
│                 │
│ 1. Check custom │
│ 2. Check global │
│ 3. Use default  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Build CLI args  │
│ Execute process │
└─────────────────┘
```

## Files Modified

1. **imagetask.h**
   - Added `QVariantMap customOptimizerSettings`
   - Added `hasCustomOptimizerSettings()` helper
   - Added `QVariantMap` include

2. **worker/ImageWorker.h**
   - Added `setCustomSettings()` method
   - Added `m_customSettings` member
   - Added `getSetting()` helper method
   - Added `QVariantMap` and `settings.h` includes

3. **worker/jpegoptimworker.cpp**
   - Replaced all `settings.value()` with `getSetting()`

4. **worker/pngquantworker.cpp**
   - Replaced all `settings.value()` with `getSetting()`

5. **worker/gifsicleworker.cpp**
   - Replaced all `settings.value()` with `getSetting()`

6. **worker/svgoworker.cpp**
   - Replaced all `settings.value()` with `getSetting()`

7. **taskwidget.h**
   - Added `setTaskCustomOptimizerSettings()` method
   - Added `clearTaskCustomOptimizerSettings()` method

8. **taskwidget.cpp**
   - Implemented set/clear methods
   - Added custom settings application in `processNextBatch()`

## Testing Recommendations

1. **Basic Custom Settings:**
   - Set custom quality for one image
   - Process it
   - Verify it uses custom settings
   - Verify other images use global settings

2. **Fallback Behavior:**
   - Set custom settings for task
   - Clear custom settings
   - Process task
   - Verify it uses global settings

3. **Re-processing:**
   - Set custom settings
   - Process task
   - Re-process task
   - Verify custom settings persist

4. **Mixed Settings:**
   - Set custom settings for some tasks
   - Leave others with global settings
   - Process all
   - Verify each uses correct settings

5. **Setting Overrides:**
   - Set partial custom settings (only quality, not metadata)
   - Process task
   - Verify custom quality used, global metadata used

## Future Enhancements (Optional)

- Visual indicator in table showing tasks with custom settings
- "Copy Settings" feature to duplicate settings between tasks
- Settings templates/presets (High Quality, Web Optimized, etc.)
- Import/export custom settings for reuse
- Show settings diff in detail panel (custom vs global)

