# JPEG Optimization Improvements

## Overview
Updated jpegoptim worker and preference widget to support jpegoptim v1.5.6 with comprehensive user-configurable options.

## Changes Made

### 1. UI/Preference Widget (`OptimizerPrefWidgets/jpegoptimprefwidget.ui`)
Created a comprehensive preference interface with the following sections:

#### Quality Settings Group
- **Maximum Quality** (0-100 spinner)
  - Shows quality level label (Lossless/High/Good/Medium/Low)
  - Default: 100 (lossless)
  
- **Target Size** (0-99999 KB spinner)
  - Allows optimizing to specific file size
  - Default: 0 (disabled)
  
- **Compression Threshold** (0-100% spinner)
  - Skip optimization if savings below threshold
  - Default: 0 (always optimize)

#### Metadata Handling Group
- **Dropdown menu** with 5 options:
  1. Keep All Metadata (default)
  2. Strip All Metadata
  3. Keep EXIF Only
  4. Keep ICC Profile Only
  5. Keep EXIF + ICC Profile
- Includes helpful hint label about metadata impact

#### Output Format Group
- **Radio buttons** for JPEG format:
  1. Auto Mode (recommended, default)
  2. Force Progressive JPEG
  3. Force Baseline (Normal) JPEG

#### Additional Options Group
- **Force optimization** checkbox
- **Preserve file timestamps** checkbox (default: checked)
- **Retry optimization** checkbox

#### Performance Group
- **Parallel Threads** (1-16 spinner)
  - Controls jpegoptim internal parallelism
  - Default: 1

### 2. Preference Widget Logic (`jpegoptimprefwidget.cpp/.h`)

#### New Methods
- `loadSettings()`: Loads all settings from QSettings
- `saveSettings()`: Saves all settings to QSettings with auto-save
- `updateQualityLabel(int)`: Updates quality description label dynamically

#### Signal Connections
- All UI controls connected to `saveSettings()` for immediate persistence
- Quality spinner connected to label updater for user feedback

#### Settings Keys
All settings stored under `jpegoptim/` prefix:
- `jpegoptim/maxQuality`
- `jpegoptim/targetSize`
- `jpegoptim/compressionThreshold`
- `jpegoptim/metadataMode`
- `jpegoptim/outputMode`
- `jpegoptim/force`
- `jpegoptim/preserveTimes`
- `jpegoptim/retry`
- `jpegoptim/maxWorkers`

### 3. Worker Implementation (`worker/jpegoptimworker.cpp`)

#### Improved Argument Building
Correctly maps settings to jpegoptim v1.5.6 arguments:

**Quality:**
- `--max=N` when maxQuality < 100
- `--size=N` when targetSize > 0
- `--threshold=N` when compressionThreshold > 0

**Metadata:**
- Mode 0: `--strip-none`
- Mode 1: `--strip-all`
- Mode 2: `--strip-all --keep-exif`
- Mode 3: `--strip-all --keep-icc`
- Mode 4: `--strip-all --keep-exif --keep-icc`

**Output Format:**
- Mode 0: `--auto-mode`
- Mode 1: `--all-progressive`
- Mode 2: `--all-normal`

**Options:**
- `--force` when enabled
- `--preserve` when enabled
- `--retry` when enabled
- `--workers=N` when N > 1

#### File Handling
- Creates destination directory if needed
- Removes existing destination file
- Copies source to destination (jpegoptim operates in-place)
- Proper error handling with user feedback

#### Debug Output
- Logs full jpegoptim command for troubleshooting

### 4. Documentation Updates

#### DEVELOPER_README.md
- Added comprehensive JPEG Optimization Settings section
- Documented all settings with keys, defaults, and CLI mappings
- Explained command construction process

#### README.md
- Added detailed JPEG Optimization subsection
- User-friendly explanations of each setting
- Practical recommendations and use cases
- Metadata impact information

#### Code Comments
- Enhanced jpegoptimworker.cpp header comment
- Complete settings mapping reference
- Clear explanation of modes and options

## Vital Options Implemented

Based on jpegoptim v1.5.6 help, we selected these vital user options:

✅ **Quality Control**: --max, --size, --threshold  
✅ **Metadata**: --strip-all, --keep-exif, --keep-icc  
✅ **Output Format**: --auto-mode, --all-progressive, --all-normal  
✅ **File Handling**: --preserve, --force  
✅ **Performance**: --workers  
✅ **Optimization**: --retry  

### Options Not Exposed (Expert/Rare Use)
- `--noaction` (dry run - not needed in GUI)
- `--csv` (output format - handled by app)
- `--totals` (summary - app provides this)
- `--verbose` (debugging - use --quiet instead)
- `--overwrite` (handled by app logic)
- `--preserve-perms` (Linux default behavior)
- `--stdout/--stdin` (not applicable for GUI)
- `--files-stdin/--files-from` (GUI handles file selection)
- `--nofix` (better to attempt all files)
- `--save-extra` (edge case)
- Individual metadata flags (--strip-exif, --strip-iptc, etc.) - covered by combo options

## Testing Recommendations

1. **Test Quality Levels**: Try 100, 90, 75, 50 on sample images
2. **Test Metadata Modes**: Verify EXIF/ICC preservation
3. **Test Target Size**: Set specific KB targets
4. **Test Threshold**: Set to 5% and verify skipping
5. **Test Output Modes**: Compare progressive vs baseline
6. **Test Workers**: Try 1, 2, 4 threads
7. **Test Retry**: Verify multiple optimization passes
8. **Test Preserve Times**: Verify timestamp preservation

## Benefits

### For Users
- **Full Control**: Access to all important jpegoptim features
- **Easy to Use**: Logical grouping and clear labels
- **Safe Defaults**: Lossless, preserve timestamps, keep metadata
- **Helpful Guidance**: Tooltips and hint labels
- **Immediate Feedback**: Quality labels update in real-time

### For Developers
- **Well Documented**: Comprehensive comments and README
- **Maintainable**: Clear settings mapping
- **Extensible**: Easy to add new options
- **Debuggable**: Command logging for troubleshooting

## Version Compatibility

Tested against: **jpegoptim v1.5.6**  
Minimum required: **jpegoptim v1.5.0** (for --workers support)

## Future Enhancements

Potential additions:
- [ ] Presets (Web Optimized, Archival Quality, Maximum Compression)
- [ ] Preview mode to compare before/after
- [ ] Batch-specific settings override
- [ ] Advanced metadata options (keep specific EXIF tags)
- [ ] Arithmetic coding support (--all-arith)
- [ ] Huffman coding option (--all-huffman)

---

**Author**: PixelBatch Development Team  
**Date**: November 6, 2025  
**Version**: 1.0

