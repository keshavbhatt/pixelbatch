# Command-Line Argument Handling

## Overview
Implemented command-line argument handling to process image files and folders passed to PixelBatch via desktop integration, command line, or file manager context menu.

## Desktop Entry Specification

### Exec Field Explained
```ini
Exec=pixelbatch %F
```

**Field Code `%F`:**
- **Meaning**: Multiple file paths
- **Type**: List of files/URIs
- **Usage**: When user selects multiple files and opens with PixelBatch
- **Examples**:
  - Single file: `pixelbatch /home/user/photo.jpg`
  - Multiple files: `pixelbatch /home/user/photo1.jpg /home/user/photo2.png`
  - Folder: `pixelbatch /home/user/Photos/`

### Alternative Field Codes

| Code | Meaning | Use Case |
|------|---------|----------|
| `%f` | Single file path | Only one file at a time |
| `%F` | **Multiple file paths** | Multiple files (our choice) |
| `%u` | Single URL | Web resources |
| `%U` | Multiple URLs | Multiple web resources |

We use `%F` because PixelBatch is designed for **batch processing** of multiple images.

## Implementation

### 1. Command-Line Parser (main.cpp)

```cpp
QCommandLineParser parser;
parser.setApplicationDescription("Batch Image Optimization Tool");
parser.addHelpOption();           // --help or -h
parser.addVersionOption();        // --version or -v
parser.addPositionalArgument("files", "Image files or folders to optimize", "[files...]");
parser.process(a);
```

**Features:**
- Standard `--help` and `--version` flags
- Positional arguments for file paths
- Proper help text

### 2. File and Folder Processing

```cpp
for (const QString &arg : args) {
  QFileInfo fileInfo(arg);
  
  if (fileInfo.isFile()) {
    // Add single file directly
    filesToAdd.append(fileInfo.absoluteFilePath());
  } else if (fileInfo.isDir()) {
    // Scan directory for image files
    QDir dir(fileInfo.absoluteFilePath());
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG"
                << "*.png" << "*.PNG"
                << "*.gif" << "*.GIF"
                << "*.svg" << "*.SVG";
    
    QFileInfoList files = dir.entryInfoList(nameFilters, QDir::Files);
    for (const QFileInfo &file : files) {
      filesToAdd.append(file.absoluteFilePath());
    }
  }
}
```

**Supported Formats:**
- JPEG: `.jpg`, `.jpeg` (case-insensitive)
- PNG: `.png` (case-insensitive)
- GIF: `.gif` (case-insensitive)
- SVG: `.svg` (case-insensitive)

**Folder Handling:**
- Recursively finds all image files
- Non-recursive (only direct children)
- Filters by supported extensions

### 3. Delayed Processing

```cpp
QTimer::singleShot(100, [&w, args]() {
  // Process files after UI initialization
});
```

**Why Delayed?**
- Ensures main window is fully initialized
- Prevents UI lag during startup
- Allows proper widget state setup

### 4. Integration Method

```cpp
void PixelBatch::addFileFromCommandLine(const QString &filePath) {
  if (m_taskWidget) {
    m_taskWidget->addFileToTable(filePath);
  }
}
```

**Features:**
- Safety check for widget existence
- Delegates to TaskWidget
- Uses existing addFileToTable method

## Usage Scenarios

### 1. File Manager Integration

**Right-click context menu:**
```
photo1.jpg
photo2.png
photo3.gif
  → Right-click → Open With → PixelBatch
```

**Result:**
- PixelBatch launches
- All three files added to task list
- Ready to optimize

### 2. Command-Line Usage

**Single file:**
```bash
pixelbatch /home/user/photo.jpg
```

**Multiple files:**
```bash
pixelbatch photo1.jpg photo2.png photo3.gif
```

**Folder:**
```bash
pixelbatch /home/user/Photos/
```
- Adds all `.jpg`, `.jpeg`, `.png`, `.gif`, `.svg` files from the folder

**Mixed:**
```bash
pixelbatch photo1.jpg /home/user/Photos/ photo2.png
```
- Adds photo1.jpg
- Adds all images from /home/user/Photos/
- Adds photo2.png

### 3. Drag and Drop from Desktop

**Drag files to application icon:**
- Desktop environment passes file paths as arguments
- PixelBatch opens with files loaded

### 4. Help and Version

**Display help:**
```bash
pixelbatch --help
```

**Output:**
```
Usage: pixelbatch [options] [files...]
Batch Image Optimization Tool

Options:
  -h, --help     Displays help on commandline options.
  --help-all     Displays help including Qt specific options.
  -v, --version  Displays version information.

Arguments:
  files          Image files or folders to optimize
```

**Display version:**
```bash
pixelbatch --version
```

**Output:**
```
pixelbatch 1.0.0
```

## Desktop Integration Examples

### GNOME Files (Nautilus)

**Setup:**
1. Right-click image file
2. Properties → Open With → Add Application
3. Select PixelBatch
4. Set as default or use "Open With" submenu

**Usage:**
- Select multiple images
- Right-click → Open With → PixelBatch
- All files loaded automatically

### KDE Dolphin

**Setup:**
1. Settings → Configure Dolphin → Services
2. Add service for image files
3. Point to PixelBatch desktop file

**Usage:**
- Select images → Actions → Optimize with PixelBatch

### Thunar (XFCE)

**Custom Action:**
1. Edit → Configure Custom Actions
2. Create action: "Optimize with PixelBatch"
3. Command: `pixelbatch %F`
4. Appearance Conditions: Image files

**Usage:**
- Select images → Right-click → Optimize with PixelBatch

## Technical Details

### Absolute Paths

All file paths are converted to absolute paths:
```cpp
fileInfo.absoluteFilePath()
```

**Benefits:**
- Works regardless of current working directory
- No relative path issues
- Consistent behavior

### Case-Insensitive Extensions

```cpp
nameFilters << "*.jpg" << "*.JPG"
             << "*.png" << "*.PNG"
             << "*.gif" << "*.GIF"
             << "*.svg" << "*.SVG";
```

**Handles:**
- photo.jpg ✓
- photo.JPG ✓
- photo.Jpg ✓
- photo.jPg ✓

### Error Handling

**Invalid paths:**
- Silently ignored (QFileInfo handles gracefully)
- Only valid files/folders processed

**Non-image files:**
- Filtered out by TaskWidget's validation
- Only supported formats accepted

## User Experience

### Visual Feedback

**Before:**
- User launches PixelBatch
- Manually adds files via dialog or drag-drop

**After:**
- User right-clicks images → Open with PixelBatch
- Application opens with files **already loaded**
- Immediate "Process Images" ready to click

### Workflow Example

1. **Select images in file manager**
   - photo1.jpg, photo2.png, photo3.gif

2. **Right-click → Open with PixelBatch**
   - PixelBatch launches

3. **Files automatically added**
   - 3 files in task list
   - Status: Pending

4. **Click "Process Images"**
   - All files optimized

5. **Done!**
   - No manual file selection needed

## Testing

### Test Cases

**1. Single file:**
```bash
pixelbatch test.jpg
```
Expected: 1 file in task list

**2. Multiple files:**
```bash
pixelbatch test1.jpg test2.png test3.gif
```
Expected: 3 files in task list

**3. Folder:**
```bash
pixelbatch /path/to/images/
```
Expected: All images from folder in task list

**4. Mixed:**
```bash
pixelbatch single.jpg /path/to/folder/ another.png
```
Expected: single.jpg + all folder images + another.png

**5. Invalid paths:**
```bash
pixelbatch nonexistent.jpg
```
Expected: Gracefully ignored, empty task list

**6. Non-image files:**
```bash
pixelbatch document.txt
```
Expected: Filtered out by TaskWidget

**7. Help:**
```bash
pixelbatch --help
```
Expected: Help text displayed, app doesn't launch

**8. Version:**
```bash
pixelbatch --version
```
Expected: Version displayed, app doesn't launch

## Limitations

### Current Implementation

1. **Non-Recursive Folder Scan**
   - Only scans direct children of folder
   - Doesn't search subdirectories
   - Can be enhanced if needed

2. **No Progress Indication During Scan**
   - Large folders processed silently
   - UI might appear frozen briefly
   - Could add progress dialog

3. **No File Count Limit**
   - Attempts to load all files
   - Very large folders might cause delay
   - Could add warning for >1000 files

## Future Enhancements

Potential improvements:

- [ ] Recursive folder scanning with `--recursive` flag
- [ ] Progress dialog for large folder scans
- [ ] File count warning/confirmation
- [ ] Pattern matching (e.g., `*.jpg` glob support)
- [ ] Exclude patterns (e.g., `--exclude="*-thumbnail.jpg"`)
- [ ] Pipe support (`cat files.txt | xargs pixelbatch`)
- [ ] Automatic processing with `--auto-process` flag
- [ ] Output directory specification via CLI

## Security Considerations

### Path Validation

- Uses Qt's QFileInfo for safe path handling
- Absolute paths prevent directory traversal
- No shell execution of paths

### File Type Validation

- Extension-based filtering
- TaskWidget performs additional validation
- Only supported formats processed

## Conclusion

Command-line argument handling is now fully implemented:

✅ **Desktop Integration** - Works with file manager "Open With"  
✅ **Batch Support** - Multiple files via `%F` field code  
✅ **Folder Support** - Automatically scans directories  
✅ **Help/Version** - Standard command-line flags  
✅ **Safe Handling** - Proper path validation  
✅ **User Friendly** - Delayed processing for smooth UI  

Users can now:
1. Right-click images → Open with PixelBatch
2. Drag images to PixelBatch icon
3. Run from command line with file arguments
4. Process entire folders with one command

This significantly improves the user experience by eliminating manual file selection when launching from the desktop or file manager!

---

**Status:** ✅ Complete  
**Date:** November 6, 2025  
**Version:** 1.0.0

