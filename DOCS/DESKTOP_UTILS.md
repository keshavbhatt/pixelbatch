# Desktop Integration Utilities - DesktopUtils

## Overview
Centralized utility class for opening URLs, files, and folders with proper snap confinement support. Uses xdg-open as primary method with QDesktopServices as fallback to ensure functionality in snap environments.

## Problem

In snap confinement, `QDesktopServices::openUrl()` often fails for:
- `file://` URLs
- Opening folders in file manager
- Selecting files in file manager
- Opening local files with default applications

This is due to snap's security restrictions and sandboxing.

## Solution

Created **DesktopUtils** - a centralized utility class that:
1. **Tries xdg-open first** (works reliably in snap)
2. **Falls back to QDesktopServices** (for non-snap environments)
3. **Provides specialized methods** for common operations

## Implementation

### DesktopUtils Class

**File:** `src/desktoputils.h`

```cpp
class DesktopUtils {
public:
    // Open URL (web links, file:// URLs, etc.)
    static bool openUrl(const QString &url);
    
    // Open file in default application
    static bool openFile(const QString &filePath);
    
    // Open folder and select file in file manager
    static bool openFolderAndSelectFile(const QString &filePath);
    
    // Open folder in file manager
    static bool openFolder(const QString &folderPath);

private:
    static bool openWithXdgOpen(const QString &target);
    static bool openWithQDesktopServices(const QString &url);
};
```

## Methods

### 1. openUrl(const QString &url)

**Purpose:** Open any URL (web, file://, etc.)

**Example:**
```cpp
DesktopUtils::openUrl("https://github.com/pixelbatch");
DesktopUtils::openUrl("file:///home/user/image.jpg");
```

**Behavior:**
1. Try xdg-open with URL
2. Fallback to QDesktopServices::openUrl()

### 2. openFile(const QString &filePath)

**Purpose:** Open file with default application

**Example:**
```cpp
DesktopUtils::openFile("/home/user/photo.jpg");
// Opens photo.jpg in default image viewer
```

**Behavior:**
1. Validate file exists
2. Convert to absolute path
3. Try xdg-open
4. Fallback to QDesktopServices with file:// URL

### 3. openFolderAndSelectFile(const QString &filePath)

**Purpose:** Open file manager and select/highlight the file

**Example:**
```cpp
DesktopUtils::openFolderAndSelectFile("/home/user/optimized_photo.jpg");
// Opens /home/user/ folder with optimized_photo.jpg selected
```

**Behavior:**
1. Validate file exists
2. **Try D-Bus call to org.freedesktop.FileManager1** (works on GNOME, KDE, etc.)
3. Fallback to opening folder with xdg-open
4. Final fallback to QDesktopServices

**D-Bus Method:**
```bash
dbus-send --session \
  --dest=org.freedesktop.FileManager1 \
  /org/freedesktop/FileManager1 \
  org.freedesktop.FileManager1.ShowItems \
  array:string:"file:///path/to/file" \
  string:""
```

### 4. openFolder(const QString &folderPath)

**Purpose:** Open folder in file manager

**Example:**
```cpp
DesktopUtils::openFolder("/home/user/Photos");
// Opens Photos folder in file manager
```

**Behavior:**
1. Validate folder exists
2. Try xdg-open
3. Fallback to QDesktopServices with file:// URL

## Snap Confinement Support

### Why xdg-open Works Better

**xdg-open advantages in snap:**
- Recognized by snap's desktop interface
- Properly escapes snap confinement for file operations
- Automatically uses host system's file manager
- Supports all desktop environments

**QDesktopServices limitations in snap:**
- May fail to open file:// URLs
- Restricted by snap's file system isolation
- Doesn't always escape to host file manager

### Snap Plugs Required

In `snapcraft.yaml`, these plugs enable desktop integration:

```yaml
plugs:
  - desktop
  - desktop-legacy
  - home
  - removable-media
```

## Code Migration

### Before (Direct QDesktopServices)

```cpp
// Opening URL
QDesktopServices::openUrl(QUrl("https://example.com"));

// Opening file
QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));

// Opening folder
QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
```

**Issues:**
- May fail in snap
- No fallback mechanism
- Doesn't select files in file manager

### After (Centralized DesktopUtils)

```cpp
// Opening URL
DesktopUtils::openUrl("https://example.com");

// Opening file
DesktopUtils::openFile(filePath);

// Opening folder and selecting file
DesktopUtils::openFolderAndSelectFile(filePath);

// Opening folder
DesktopUtils::openFolder(folderPath);
```

**Benefits:**
- Works in snap and native environments
- Automatic fallback
- File selection support
- Centralized logic

## Usage in PixelBatch

### About Dialog

**File:** `src/about.cpp`

**Before:**
```cpp
QDesktopServices::openUrl(QUrl(donateLink));
```

**After:**
```cpp
DesktopUtils::openUrl(donateLink);
```

**Used in:**
- `onDonateClicked()`
- `onReportIssueClicked()`
- `onSourceCodeClicked()`
- `onWebsiteClicked()`

### Main Window

**File:** `src/pixelbatch.cpp`

**Before:**
```cpp
QDesktopServices::openUrl(QUrl("https://github.com/..."));
```

**After:**
```cpp
DesktopUtils::openUrl("https://github.com/...");
```

**Used in:**
- `reportIssue()` - Opens GitHub issues
- `donate()` - Opens GitHub sponsors

### TaskWidget

**File:** `src/taskwidget.cpp`

#### Opening File in File Manager

**Before:**
```cpp
QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
// Just opens folder, doesn't select file
```

**After:**
```cpp
DesktopUtils::openFolderAndSelectFile(fileInfo.absoluteFilePath());
// Opens folder AND selects the file
```

**Improvement:** File is now highlighted in file manager!

#### Opening Image in Viewer

**Before:**
```cpp
if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
    QMessageBox::warning(...);
}
```

**After:**
```cpp
if (!DesktopUtils::openFile(filePath)) {
    QMessageBox::warning(...);
}
```

**Used in:**
- `openOptimizedImageInFileManagerForSelectedTask()` - Opens and selects optimized image
- `openOptimizedImageInImageViewerForSelectedTask()` - Opens optimized image in viewer
- `openOriginalImageInImageViewerForSelectedTask()` - Opens original image in viewer

## Testing

### Test Cases

**1. Opening Web URLs**
```cpp
DesktopUtils::openUrl("https://github.com");
```
Expected: Browser opens to GitHub

**2. Opening File in Viewer**
```cpp
DesktopUtils::openFile("/home/user/photo.jpg");
```
Expected: Default image viewer opens photo.jpg

**3. Opening Folder**
```cpp
DesktopUtils::openFolder("/home/user/Photos");
```
Expected: File manager opens Photos folder

**4. Opening Folder and Selecting File**
```cpp
DesktopUtils::openFolderAndSelectFile("/home/user/photo.jpg");
```
Expected: File manager opens with photo.jpg selected/highlighted

### Testing in Snap

```bash
# Install snap
sudo snap install pixelbatch_1.0.0_amd64.snap --dangerous

# Test opening URLs (from About dialog)
# Click "Website", "Report Issue", "Source Code"

# Test opening files (from TaskWidget)
# Add images, process, then:
# - Right-click → "Open Optimized Image"
# - Right-click → "Show in File Manager"
# - Right-click → "Open Original Image"
```

### Testing Native (Non-Snap)

```bash
# Build and run natively
qmake
make
./pixelbatch

# Test same operations as above
```

## Error Handling

### File Not Found

```cpp
if (!fileInfo.exists()) {
    qWarning() << "File does not exist:" << filePath;
    return false;
}
```

### xdg-open Not Available

```cpp
if (!checkProcess.waitForFinished(1000) || checkProcess.exitCode() != 0) {
    qDebug() << "xdg-open not found";
    return false;  // Will fallback to QDesktopServices
}
```

### Both Methods Failed

```cpp
if (!DesktopUtils::openFile(filePath)) {
    QMessageBox::warning(this, tr("Error Opening File"),
                         tr("Failed to open the file..."));
}
```

## Debugging

### Enable Debug Output

```cpp
qDebug() << "Attempting to open:" << target;
qDebug() << "xdg-open not found";
qWarning() << "QDesktopServices::openUrl failed for:" << url;
```

### Check xdg-open Availability

```bash
which xdg-open
# Should output: /usr/bin/xdg-open
```

### Test xdg-open Manually

```bash
xdg-open https://github.com
xdg-open /home/user/photo.jpg
xdg-open /home/user/Photos/
```

## Platform-Specific Behavior

### Linux (Most Common)

**xdg-open delegates to:**
- GNOME: gio open / nautilus
- KDE: kde-open / dolphin
- XFCE: exo-open / thunar
- Other DEs: falls back to file manager

### File Manager File Selection

**Works on:**
- ✅ GNOME (Nautilus) - via D-Bus
- ✅ KDE (Dolphin) - via D-Bus
- ✅ Cinnamon (Nemo) - via D-Bus
- ⚠️ XFCE (Thunar) - Opens folder only
- ⚠️ LXDE (PCManFM) - Opens folder only

**Fallback:** If D-Bus fails, opens folder without selection

## Benefits

### 1. Snap Compatibility

✅ Works reliably in snap confinement  
✅ Properly escapes sandbox for file operations  
✅ Uses host system's default applications  

### 2. Centralized Logic

✅ Single source of truth  
✅ Easy to update/maintain  
✅ Consistent behavior across application  
✅ No code duplication  

### 3. Better UX

✅ File selection in file manager  
✅ Automatic fallback mechanisms  
✅ Works on all desktop environments  
✅ Proper error handling  

### 4. Future-Proof

✅ Easy to add new methods  
✅ Can add platform-specific optimizations  
✅ Testable in isolation  

## Future Enhancements

Potential improvements:

- [ ] Add macOS support (open command)
- [ ] Add Windows support (start command)
- [ ] Add file manager detection (for better selection support)
- [ ] Add async operations with callbacks
- [ ] Add timeout configuration
- [ ] Add retry mechanisms
- [ ] Add custom error messages per operation

## Comparison: Before vs After

### Opening URL

**Before:**
```cpp
QDesktopServices::openUrl(QUrl(url));
```
- May fail in snap
- No fallback
- 1 method call

**After:**
```cpp
DesktopUtils::openUrl(url);
```
- Works in snap and native
- Automatic fallback
- Same 1 method call

### Opening File

**Before:**
```cpp
if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
    // Error handling
}
```
- May fail in snap
- Verbose QUrl conversion
- No validation

**After:**
```cpp
if (!DesktopUtils::openFile(filePath)) {
    // Error handling
}
```
- Works in snap and native
- Simple API
- Built-in validation

### Showing File in Folder

**Before:**
```cpp
// Open folder only, no file selection
QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
```
- No file selection
- May fail in snap

**After:**
```cpp
// Opens folder AND selects file
DesktopUtils::openFolderAndSelectFile(filePath);
```
- Selects file when possible
- Works in snap and native
- Better UX

## Files Modified

### Created:
- ✅ `src/desktoputils.h` - Header file
- ✅ `src/desktoputils.cpp` - Implementation

### Modified:
- ✅ `src/about.cpp` - Uses DesktopUtils for all URL opening
- ✅ `src/pixelbatch.cpp` - Uses DesktopUtils for GitHub links
- ✅ `src/taskwidget.cpp` - Uses DesktopUtils for file/folder operations
- ✅ `src/PixelBatch.pro` - Added desktoputils to build

## Conclusion

The DesktopUtils class provides:

✅ **Snap Compatibility** - Works reliably in snap environments  
✅ **Centralized Logic** - No code duplication  
✅ **Better UX** - File selection in file manager  
✅ **Automatic Fallback** - xdg-open → QDesktopServices  
✅ **Easy Maintenance** - Single point of update  
✅ **Consistent Behavior** - Same API everywhere  

All desktop integration operations in PixelBatch now use DesktopUtils for robust, snap-friendly file and URL handling!

---

**Status:** ✅ Complete  
**Date:** November 6, 2025  
**Version:** 1.0.0

