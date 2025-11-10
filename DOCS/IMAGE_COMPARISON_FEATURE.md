- **Synchronized Display**: Both images zoom together

**Zoom Methods:**
- `zoomIn()`: Multiply current zoom by 1.25
- `zoomOut()`: Divide current zoom by 1.25
- `zoomReset()`: Set zoom to 1.0 (100%)
- `zoomFit()`: Calculate optimal zoom to fit viewport (max 100%)

# Image Comparison Feature

## Overview

The Image Comparison feature in PixelBatch allows users to visually compare original images with their optimized versions. This helps verify that the optimization process maintained acceptable image quality while reducing file size.

## Accessing the Feature

1. **Process images** in the Task Widget
2. **Select a completed task** from the list (row must be highlighted)
3. **Click "Compare Before/After"** button in the Task Action panel

The comparison dialog will open showing both the original and optimized images.

## Dialog Layout

The Image Comparison dialog consists of several components arranged vertically:

2. Use "Fit" to see the whole image first
3. Zoom in to 200-300% to inspect compression artifacts
4. Scroll to various parts of the image
5. Check edges, gradients, and text details
6. Look for JPEG blocking or PNG color banding
7. Reset to 100% to verify at actual size
│                                                     │
│  [Comparison View - Side-by-Side OR Slider]        │
1. Start at 100% zoom (if needed)
2. Move slider slowly across the image
3. Watch for compression artifacts at the split line
4. Check fine details and textures
5. Verify color accuracy
6. Examine critical areas thoroughly
7. Pay attention to areas with gradients or subtle details
├─────────────────────────────────────────────────────┤
│  [Close Button]                                     │
└─────────────────────────────────────────────────────┘
```

## Viewing Modes

### 1. Side-by-Side View (Default)

The side-by-side view displays the original and optimized images next to each other in a split panel.

**Features:**
- **Splitter**: Adjustable divider between the two images
- **Independent Scrolling**: Each image can be scrolled independently (when sync is disabled)
- **Synchronized Scrolling**: Optional feature to scroll both images together (enabled by default)
- **Zoom Controls**: Interactive zoom functionality to inspect details
- **Labels**: Clear headers indicating "Original" and "Optimized"

**Synchronized Scrolling:**
- Checkbox at the top: "Synchronize scroll bars"
- **Checked** (default): Scrolling one image automatically scrolls the other proportionally
- **Unchecked**: Images can be scrolled independently
- Works for both horizontal and vertical scrolling
- Handles images of different dimensions by using proportional scaling

**Zoom Functionality:**
- **Zoom In (+)**: Increase magnification by 25% (up to 500%)
- **Zoom Out (−)**: Decrease magnification by 25% (down to 10%)
- **100%**: Reset to original size (1:1 pixel ratio)
- **Fit**: Scale image to fit within the viewport (max 100%)
- **Range**: 10% to 500% zoom levels
- **Scroll Position Preservation**: Maintains relative scroll position when zooming
- **Synchronized Zoom**: Both images zoom together
- **Smooth Scaling**: Uses Qt's SmoothTransformation for quality

**Benefits:**
- Easy to see overall composition differences
- Good for comparing different areas of the image
- Natural way to view two images simultaneously
- Zoom in to inspect compression artifacts and fine details
- Zoom out to see the whole picture at once

### 2. Slider View

The slider view overlays both images and provides an interactive slider to reveal the difference.

**Features:**
- **Interactive Slider**: Horizontal slider (0-100) controls the reveal position
- **Vertical Split Line**: Yellow line (3px wide) shows the exact division point
- **Left Side**: Shows original image
- **Right Side**: Shows optimized image
- **Smooth Transition**: Drag slider to see real-time comparison
- **Zoom Controls**: Same zoom functionality as side-by-side view

**Zoom Functionality:**
- **Zoom In (+)**: Increase magnification by 25% (up to 500%)
- **Zoom Out (−)**: Decrease magnification by 25% (down to 10%)
- **100%**: Reset to original size (1:1 pixel ratio)
- **Fit**: Scale image to fit within the viewport (max 100%)
- **Real-time Composite**: Slider composite updates automatically with zoom
- **Range**: 10% to 500% zoom levels

**Benefits:**
- Pixel-perfect comparison at the exact same position
- Easy to spot subtle compression artifacts
- Excellent for detailed quality inspection
- Zoom in to examine fine details at the split boundary
- Zoom out to see overall composition

## View Mode Persistence

The comparison dialog **remembers your preferred viewing mode** across sessions:

- **Preference Storage**: Uses QSettings to save the last used mode
- **Setting Key**: `ImageComparison/viewMode`
- **Values**: `"sidebyside"` (default) or `"slider"`
- **Automatic Loading**: Next time you open the comparison dialog, it opens in your last used mode

**Implementation:**
```cpp
// Saving preference
QSettings settings;
settings.setValue("ImageComparison/viewMode", 
                  m_isSideBySideView ? "sidebyside" : "slider");

// Loading preference
QString viewMode = settings.value("ImageComparison/viewMode", "sidebyside").toString();
```

## Image Information Section

Located at the **bottom** of the dialog, this section displays file statistics in a compact format:

**Displayed Information:**
- **Original Size**: File size with byte count
- **Optimized Size**: File size with byte count  
- **Saved Space**: Reduction in bytes and percentage

**Example:**
```
Original: 2.5 MB (2621440 bytes)  |  Optimized: 892.3 KB (914124 bytes)  |  Saved: 1.6 MB (65.13%)
```

**Design Characteristics:**
- **Minimal Height**: Uses `QSizePolicy::Maximum` to take only required space
- **Horizontal Layout**: Single line with pipe separators
- **Word Wrap Enabled**: Adapts to narrow windows
- **Compact Margins**: 6px margins for tight spacing

## Technical Implementation

### Class Structure

```cpp
class ImageComparisonWidget : public QDialog
```

**Key Member Variables:**
- `m_originalPixmap`, `m_optimizedPixmap`: Loaded images (original size)
- `m_originalPixmapScaled`, `m_optimizedPixmapScaled`: Zoomed versions
- `m_leftScroll`, `m_rightScroll`: Scroll areas for side-by-side view
- `m_imageLabel`: Label for slider view composite image
- `m_syncScrollBars`: Boolean flag for scroll synchronization
- `m_updatingScrollBars`: Prevents infinite scroll loops
- `m_isSideBySideView`: Tracks current view mode
- `m_zoomFactor`: Current zoom level (0.1 to 5.0)

### Image Loading

Images are automatically scaled if too large:

```cpp
int maxDim = 2000;  // Maximum dimension in pixels
if (width > maxDim || height > maxDim) {
    pixmap = pixmap.scaled(maxDim, maxDim, 
                          Qt::KeepAspectRatio, 
                          Qt::SmoothTransformation);
}
```

**Rationale:**
- Prevents memory issues with very large images
- Maintains aspect ratio
- Uses smooth transformation for quality
- 2000px provides sufficient detail for comparison

### Scroll Synchronization Algorithm

The scroll synchronization uses **proportional syncing** to handle images of different dimensions:

```cpp
// Calculate ratio based on scroll position
double ratio = (double)sourceValue / sourceMaximum;

// Apply ratio to target scroll bar
targetScrollBar->setValue((int)(ratio * targetMaximum));
```

**Key Features:**
- Works with different image sizes
- Handles both horizontal and vertical scrolling
- Uses `m_updatingScrollBars` flag to prevent recursive updates
- Bidirectional synchronization (left↔right)

### Slider View Composition

The slider view creates a composite image in real-time:

```cpp
QPixmap composite(originalPixmap.size());
QPainter painter(&composite);

// Calculate split position (0-100%)
int splitX = (originalPixmap.width() * sliderValue) / 100;

// Draw original (full image)
painter.drawPixmap(0, 0, originalPixmap);

// Draw optimized (right portion only)
QRect sourceRect(splitX, 0, width - splitX, height);
QRect targetRect(splitX, 0, width - splitX, height);
painter.drawPixmap(targetRect, optimizedPixmap, sourceRect);

// Draw yellow divider line
painter.setPen(QPen(Qt::yellow, 3));
painter.drawLine(splitX, 0, splitX, height);
```

**Performance:**
- Composite created on every slider change
- Efficient QPainter operations
- Line drawn last to ensure visibility

### Zoom Implementation

The zoom feature uses scaled pixmaps with intelligent scroll position management:

```cpp
void ImageComparisonWidget::applyZoom(double factor) {
  // Clamp zoom factor between 10% and 500%
  factor = qBound(0.1, factor, 5.0);
  
  m_zoomFactor = factor;
  updateZoom();
}

void ImageComparisonWidget::updateZoom() {
  // Calculate scaled size
  QSize scaledSize(originalSize.width() * m_zoomFactor,
                   originalSize.height() * m_zoomFactor);
  
  // Scale pixmaps with smooth transformation
  m_originalPixmapScaled = m_originalPixmap.scaled(
      scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  
  // Store scroll position as ratio before update
  double scrollRatio = currentScrollPos / maxScrollPos;
  
  // Update display
  m_leftLabel->setPixmap(m_originalPixmapScaled);
  
  // Restore scroll position proportionally
  QTimer::singleShot(10, [this, scrollRatio]() {
    scrollBar->setValue(scrollRatio * newMaxScrollPos);
  });
}
```

**Key Features:**
- **Zoom Levels**: 10%, 25%, 50%, 75%, 100%, 125%, 150%, 200%, 300%, 400%, 500%
- **Incremental Zoom**: Each step multiplies/divides by 1.25 (25% change)
- **Scroll Position**: Maintains relative position when zooming (side-by-side view)
- **Delayed Restoration**: Uses QTimer to ensure layout update before scroll adjustment
- **Smooth Scaling**: Qt::SmoothTransformation ensures quality at all zoom levels
- **Dual Mode Support**: Works in both side-by-side and slider views
- **Real-time Composite**: Slider view regenerates composite image with zoomed pixmaps
- **Synchronized Display**: Both images zoom together in all modes

**Implementation Notes:**
- In **side-by-side view**: Scaled pixmaps are displayed directly in labels
- In **slider view**: Composite image is regenerated using scaled pixmaps when slider moves or zoom changes
- The `updateComparison()` method checks zoom factor and uses scaled pixmaps when `m_zoomFactor != 1.0`
- Both views share the same zoom controls and zoom factor value

## User Workflow

### Basic Comparison

1. User adds images to PixelBatch
2. User configures optimization settings
3. User processes images
4. User selects a completed task
5. User clicks "Compare Before/After"
6. Dialog opens in last-used view mode
7. User examines the comparison
8. User switches views if desired (toggle button)
9. User closes dialog when satisfied

### Quality Verification

**Side-by-Side Method:**
1. Enable scroll synchronization
2. Zoom in on specific areas (if needed)
3. Scroll to various parts of the image
4. Look for overall quality differences
5. Check edges, gradients, and details

**Slider Method:**
1. Move slider slowly across the image
2. Watch for compression artifacts
3. Check fine details and textures
4. Verify color accuracy
5. Examine critical areas thoroughly

## Integration Points

### TaskWidget Integration

```cpp
void TaskWidget::compareImagesForSelectedTask()
```

**Validation Checks:**
1. Ensure a task is selected
2. Verify task status is `Completed`
3. Check original file exists
4. Check optimized file exists
5. Show appropriate error messages if any check fails

**Dialog Creation:**
```cpp
ImageComparisonWidget *dialog = new ImageComparisonWidget(
    originalPath, optimizedPath, this);
dialog->setAttribute(Qt::WA_DeleteOnClose);
dialog->exec();  // Modal dialog
```

### TaskActionWidget Integration

**UI Button:**
- Label: "Compare Before/After"
- Icon: `image-edit-line.png`
- Position: Grid layout, row 0, column 2
- Enabled: Only when a task is selected

**Signal Connection:**
```cpp
connect(ui->compareImagesPb, &QPushButton::clicked, 
        m_taskWidget, &TaskWidget::compareImagesForSelectedTask);
```

## Error Handling

### File Not Found

If the original or optimized image file doesn't exist:

```cpp
QMessageBox::warning(this, tr("File Not Found"),
    tr("The [original/optimized] image file does not exist.\n"
       "It may have been moved or deleted."));
```

### Image Not Processed

If user tries to compare before processing:

```cpp
QMessageBox::information(this, tr("Image Not Processed"),
    tr("Please process the image first before comparing.\n"
       "The optimized image does not exist yet."));
```

### Failed Image Loading

If images fail to load:

```cpp
qWarning() << "Failed to load one or both images";
// Dialog continues but may show empty/broken images
```

## Keyboard Shortcuts

The dialog responds to standard Qt shortcuts and zoom controls:

**Dialog Controls:**
- **Escape**: Close dialog (same as clicking Close button)
- **Enter/Return**: Close dialog (default button behavior)

**Zoom Controls (Both Views):**
- **Ctrl++** or **Ctrl+=**: Zoom in (via tooltip hint)
- **Ctrl+-**: Zoom out (via tooltip hint)
- **Ctrl+0**: Reset to 100% (via tooltip hint)

*Note: Keyboard shortcuts are currently implemented via button tooltips. Full keyboard event handling could be added in future updates. Zoom functionality works in both side-by-side and slider views.*

## Future Enhancement Possibilities

### Potential Features

1. **Difference Highlighting**: Overlay showing pixel differences
2. **Metadata Comparison**: EXIF data, color space, etc.
3. **Histogram Display**: Visual representation of color distribution
4. **Export Comparison**: Save side-by-side or slider view as image
5. **Full Keyboard Shortcuts**: Arrow keys for slider, space for toggle, keyboard-based zoom
6. **Full Screen Mode**: Maximize comparison area
7. **Multiple Image Support**: Compare more than two images
8. **Difference Metrics**: PSNR, SSIM, perceptual quality scores
9. **Region Selection**: Compare specific areas only
10. **Mouse Wheel Zoom**: Zoom with mouse wheel for convenience

### Performance Optimizations

1. **Lazy Loading**: Load images only when needed
2. **Caching**: Cache scaled versions to avoid recomputation
3. **Background Loading**: Load images asynchronously
4. **Progressive Loading**: Show low-res preview first

## Code Files

### Main Implementation Files

- **Header**: `src/imagecomparisonwidget.h`
- **Implementation**: `src/imagecomparisonwidget.cpp`

### Integration Files

- **TaskWidget**: `src/taskwidget.h`, `src/taskwidget.cpp`
- **TaskActionWidget**: `src/taskactionwidget.h`, `src/taskactionwidget.cpp`
- **UI File**: `src/taskactionwidget.ui`

### Build Configuration

- **Project File**: `src/PixelBatch.pro`
  - Added to `SOURCES` and `HEADERS` sections

## Dependencies

### Qt Modules

- **QtWidgets**: Core UI components
- **QtGui**: Image handling (QPixmap, QPainter)
- **QtCore**: Settings persistence (QSettings)

### Qt Classes Used

- `QDialog`: Base class for the comparison widget
- `QPixmap`: Image storage and display
- `QPainter`: Slider view composition
- `QScrollArea`: Scrollable image containers
- `QSplitter`: Resizable divider in side-by-side view
- `QSlider`: Interactive control for slider view
- `QCheckBox`: Scroll synchronization toggle
- `QSettings`: View mode persistence
- `QLabel`: Image display and text labels
- `QGroupBox`: Info section container
- `QVBoxLayout`, `QHBoxLayout`: Layout management

## Settings Storage

### QSettings Configuration

**Organization**: Inherited from application settings
**Application**: PixelBatch

**Storage Location** (Linux):
```
~/.config/PixelBatch/PixelBatch.conf
```

**Setting Structure:**
```ini
[ImageComparison]
viewMode=sidebyside  # or "slider"
```

## Best Practices

### For Users

1. **Process First**: Always process images before comparing
2. **Use Both Views**: Each view excels at different things
3. **Enable Sync**: Keep scroll synchronization on for easier comparison
4. **Check Critical Areas**: Focus on important parts of your images
5. **Adjust Splitter**: In side-by-side view, resize panels as needed

### For Developers

1. **Memory Management**: Be mindful of large image handling
2. **Thread Safety**: Image loading could be moved to background thread
3. **Error Handling**: Always validate file existence before loading
4. **User Feedback**: Provide clear error messages
5. **Performance**: Consider lazy loading for very large images

## Testing Considerations

### Test Scenarios

1. **Different Image Sizes**: Small, medium, large, very large
2. **Different Formats**: JPEG, PNG, GIF, SVG
3. **Different Aspect Ratios**: Portrait, landscape, square
4. **Edge Cases**: 
   - Very thin images
   - Very wide images
   - Monochrome images
   - Transparent images (PNG)
5. **Error Conditions**:
   - Missing files
   - Corrupted images
   - Insufficient permissions

### Performance Testing

1. **Large Images**: Test with 8K+ resolution images
2. **Multiple Opens**: Open dialog multiple times in succession
3. **View Switching**: Toggle between views rapidly
4. **Scroll Performance**: Test with large images in side-by-side view
5. **Memory Leaks**: Verify proper cleanup on dialog close

## Conclusion

The Image Comparison feature provides a professional and user-friendly way to verify optimization results. With two viewing modes, scroll synchronization, and persistent preferences, it offers flexibility for different comparison needs while maintaining a clean and intuitive interface.

