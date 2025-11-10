# TaskWidget Split Layout Redesign

## Overview

The TaskWidget has been redesigned with a split layout that provides a much better user experience. The new design shows a task list on the left and a detailed image panel on the right, allowing users to see image previews, information, and optimization settings all in one view.

## New Layout Structure

```
┌─────────────────────────────────────────────────────────────┐
│  Menu Bar & Toolbar                                         │
├───────────────────────────┬─────────────────────────────────┤
│                           │                                 │
│   Task List (Left Panel)  │  Image Detail Panel (Right)     │
│   ┌─────────────────────┐ │  ┌───────────────────────────┐ │
│   │ File    | Status    │ │  │   Image Preview           │ │
│   │---------|-----------|│ │  │   (Thumbnail)             │ │
│   │ img1.jpg| Pending   │ │  │                           │ │
│   │ img2.png| Completed │◄┼──│   [Selected Image]        │ │
│   │ img3.gif| Pending   │ │  │                           │ │
│   │ ...     | ...       │ │  └───────────────────────────┘ │
│   └─────────────────────┘ │                                 │
│                           │  Image Information:             │
│                           │  • Filename: image.jpg          │
│                           │  • Size: 2.5 MB                 │
│                           │  • Dimensions: 1920×1080        │
│                           │  • Format: JPEG                 │
│                           │                                 │
│                           │  Optimization Settings:         │
│                           │  ┌──────────────────────────┐   │
│                           │  │ [Format-Specific Widget] │   │
│                           │  │ Quality: 75              │   │
│                           │  │ ☑ Auto Mode            │   │
│                           │  │ Strip: None             │   │
│                           │  └──────────────────────────┘   │
│                           │                                 │
│                           │  Actions:                       │
│                           │  [Remove from List]             │
│                           │  [Open Original]                │
│                           │  [Open Optimized]               │
│                           │  [Open in File Manager]         │
│                           │  [Compare Before/After]         │
│                           │                                 │
├───────────────────────────┴─────────────────────────────────┤
│  Status Bar: [Add Images] [Process Images]                 │
└─────────────────────────────────────────────────────────────┘
```

## New Components

### 1. ImageDetailPanel Widget

**Location:** `src/imagedetailpanel.h` and `src/imagedetailpanel.cpp`

**Purpose:** Displays detailed information about the selected image task.

**Sections:**

#### Image Preview
- Shows a thumbnail preview of the selected image
- Automatically scales large images to 400x400 max
- Scrollable if needed
- Shows placeholder text when no image is selected

#### Image Information
- **Filename:** Full filename with word wrap and text selection
- **Size:** File size in B, KB, or MB
- **Dimensions:** Width × Height in pixels
- **Format:** Image format (JPEG, PNG, GIF, SVG)

#### Optimization Settings
- Dynamically loads the appropriate optimizer widget based on image format
- Shows current settings for:
  - JPEG: jpegoptim settings (quality, strip options, etc.)
  - PNG: pngquant settings (quality, speed, etc.)
  - GIF: gifsicle settings (optimization level, colors, etc.)
  - SVG: svgo settings (precision, plugins, etc.)
- Settings are displayed for viewing (future: allow per-image overrides)

#### Action Buttons
- **Remove from List:** Remove the selected image from the task list
- **Open Original:** Open original image in default image viewer
- **Open Optimized:** Open optimized image (enabled after processing)
- **Open in File Manager:** Open optimized image location in file manager
- **Compare Before/After:** Open the image comparison dialog

**Key Features:**
- Buttons are dynamically enabled/disabled based on task status
- Connects to existing TaskWidget actions via signals
- Responsive to selection changes in the task list

### 2. Split Layout with QSplitter

**Implementation:** Modified in `src/pixelbatch.cpp`

**Features:**
- **Resizable divider:** User can drag to adjust panel sizes
- **Proportions:** 70% task list, 30% detail panel by default
- **Minimum widths:** Task list expands, detail panel 350-450px
- **Persistent layout:** Splitter position can be saved (future enhancement)

## Code Architecture

### Signal/Slot Connections

#### TaskWidget → ImageDetailPanel
```cpp
// When selection changes, update detail panel
connect(m_taskWidget, &TaskWidget::selectedImageTaskChanged,
        m_imageDetailPanel, &ImageDetailPanel::setImageTask);
```

#### ImageDetailPanel → TaskWidget
```cpp
// Action buttons forward to TaskWidget methods
connect(m_imageDetailPanel, &ImageDetailPanel::removeRequested, 
        m_taskWidget, &TaskWidget::removeSelectedRow);

connect(m_imageDetailPanel, &ImageDetailPanel::openOriginalRequested,
        m_taskWidget, &TaskWidget::openOriginalImageInImageViewerForSelectedTask);

// ... and so on for other actions
```

### New Methods

#### TaskWidget
- `ImageTask* getSelectedImageTask() const` - Returns currently selected task
- Signal: `selectedImageTaskChanged(ImageTask*)` - Emitted on selection change

#### ImageDetailPanel
- `void setImageTask(ImageTask* task)` - Updates panel with task details
- `void clear()` - Clears all information and disables buttons
- `void updateImagePreview()` - Loads and displays image thumbnail
- `void updateImageInfo()` - Updates file information labels
- `void updateOptimizerSettings()` - Loads format-specific settings widget
- `void loadOptimizerWidget()` - Creates appropriate optimizer widget

## User Benefits

### ✅ Visual Confirmation
- See exactly what image you're working with
- No need to remember filenames

### ✅ Contextual Information
- All relevant info at a glance
- File size, dimensions, format immediately visible

### ✅ Settings Visibility
- See current optimization settings without opening preferences
- Understand what will happen to each image

### ✅ Quick Actions
- All common actions in one place
- No need to remember keyboard shortcuts or menu locations

### ✅ Better Workflow
- Less context switching
- More efficient for batch operations
- Clear visual feedback

## Technical Details

### Image Loading
```cpp
QImageReader reader(imagePath);
QSize imageSize = reader.size();

// Scale for preview if too large
if (imageSize.width() > 400 || imageSize.height() > 400) {
    imageSize.scale(400, 400, Qt::KeepAspectRatio);
    reader.setScaledSize(imageSize);
}

QImage image = reader.read();
m_previewLabel->setPixmap(QPixmap::fromImage(image));
```

**Performance Considerations:**
- Images scaled during read (efficient)
- Max size 400x400 prevents memory issues
- Asynchronous loading could be added for very large images

### Widget Management
```cpp
// Remove previous optimizer widget
if (m_currentOptimizerWidget) {
    m_optimizerSettingsLayout->removeWidget(m_currentOptimizerWidget);
    m_currentOptimizerWidget->deleteLater();
    m_currentOptimizerWidget = nullptr;
}

// Create new widget for selected image format
ImageFormatPrefWidget *prefWidget =
    ImageFormatPrefWidgetFactory::createWidget(imageType, this);
```

**Memory Management:**
- Old widgets properly deleted with `deleteLater()`
- Factory pattern for widget creation
- Clean separation of concerns

## UI/UX Improvements

### Minimum Window Size
Changed from `350x300` to `900x500` to accommodate split layout

### Panel Constraints
```cpp
m_imageDetailPanel->setMinimumWidth(350);
m_imageDetailPanel->setMaximumWidth(450);
```

### Dynamic Button States
Buttons automatically enable/disable based on:
- Whether an image is selected
- Whether the task is completed
- Whether files exist

## Future Enhancements

### 1. Per-Image Settings Override
Allow users to change settings for individual images:
```cpp
// Add "Apply to This Image" button
// Store per-image settings in ImageTask
// Use override settings during processing
```

### 2. Batch Settings Application
```cpp
// Add "Apply to All [FORMAT]" button
// Update all matching format images
// Persist as new defaults
```

### 3. Image Preview Zoom
```cpp
// Add zoom controls to preview
// Mouse wheel zoom
// Fit/100%/Fill options
```

### 4. Quick Compare
```cpp
// Add small before/after preview
// Show side-by-side in detail panel
// Click to open full comparison
```

### 5. Processing Progress
```cpp
// Show progress in detail panel
// Live preview of optimization
// Cancel button for selected image
```

### 6. Drag & Drop Reordering
```cpp
// Drag images to reorder
// Priority processing
// Group by format
```

## Migration Notes

### Old TaskActionWidget
- Kept for backward compatibility but hidden
- Could be removed in future version
- All functionality moved to ImageDetailPanel

### Layout Changes
- Removed from `pixelbatch.ui` layouts
- Now constructed programmatically
- More flexibility for future changes

## Testing Considerations

### Test Cases

1. **Selection Changes**
   - Select different images
   - Verify detail panel updates
   - Check button states

2. **Image Formats**
   - Test JPEG, PNG, GIF, SVG
   - Verify correct optimizer widget loads
   - Check settings display

3. **Task States**
   - Pending tasks
   - Processing tasks
   - Completed tasks
   - Failed tasks

4. **Actions**
   - Test all action buttons
   - Verify they work with detail panel
   - Check error handling

5. **Edge Cases**
   - No selection (panel should clear)
   - Missing image files
   - Corrupted images
   - Very large images

6. **Splitter**
   - Resize panels
   - Minimum/maximum constraints
   - Window resize behavior

## Known Issues

None currently identified.

## Dependencies

### New Files
- `src/imagedetailpanel.h`
- `src/imagedetailpanel.cpp`

### Modified Files
- `src/pixelbatch.h` - Added ImageDetailPanel member
- `src/pixelbatch.cpp` - Added split layout setup
- `src/taskwidget.h` - Added getSelectedImageTask() and signal
- `src/taskwidget.cpp` - Implemented selection tracking
- `src/PixelBatch.pro` - Added new files to build

### No Breaking Changes
All existing functionality preserved, layout enhanced.

