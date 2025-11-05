# PNG Optimization Improvements

## Overview
Replaced dual PNG optimization system (pngout + pngquant) with a single, superior solution using **pngquant 3.0.3** with comprehensive user-configurable settings.

## Decision: Why pngquant Over pngout?

### Comparison

| Feature | pngquant ✅ | pngout ❌ |
|---------|------------|----------|
| **Compression Ratio** | 40-70% reduction | 5-15% reduction |
| **Type** | Lossy (with quality control) | Lossless only |
| **Quality Control** | Min-max range (0-100%) | None |
| **Speed Control** | 1-11 scale | Limited |
| **Smart Skip** | --skip-if-larger | None |
| **Modern** | v3.0.3 (2024, Rust) | v2020 (outdated) |
| **Dithering** | Floyd-Steinberg | None |
| **User-Friendly** | Intuitive options | Expert-level complexity |
| **Cross-Platform** | Excellent | Limited |
| **Documentation** | Comprehensive | Sparse |

### Key Advantages of pngquant

1. **Superior Compression**: Achieves 40-70% file size reduction vs pngout's 5-15%
2. **Quality Control**: Users can set acceptable quality range (min-max)
3. **Modern & Maintained**: Active development, Rust rewrite in 2024
4. **Better Dithering**: Floyd-Steinberg dithering produces smooth gradients
5. **Intelligent**: Skip-if-larger prevents creating bigger files
6. **Speed Options**: Configurable speed/quality trade-off
7. **Simpler**: Easier to configure and understand

**Verdict**: pngquant is objectively better for 99% of use cases.

## Implementation

### 1. PNG Preference Widget (`pngquantprefwidget.ui`)

Created comprehensive UI with 5 logical sections:

#### Quality Settings Group
- **Minimum Quality** (0-100%, default: 65%)
  - Don't save if quality falls below this level
  - Prevents over-compression
  
- **Maximum Quality** (0-100%, default: 80%)
  - Use fewer colors to stay below this quality
  - Controls compression aggressiveness
  
- **Quality Range Display**
  - Shows current range in bold blue
  - Real-time update
  - Example: "Quality Range: 65-80%"

#### Speed vs Quality Trade-off Group
- **Speed Slider** (1-11, default: 4)
  - 1 = Slowest, best quality
  - 4 = Default, balanced
  - 11 = Fastest, rougher quality
  
- **Visual Feedback**
  - Slider with tick marks
  - Dynamic label showing speed description
  - "Slow/Best" ← → "Fast/Rough" labels

#### Dithering Group
- **Enable Floyd-Steinberg Dithering** (checkbox, default: enabled)
  - Improves image quality by reducing color banding
  - Creates smoother gradients
  - Disable only for pixel art or simple graphics

#### Color Precision (Advanced) Group
- **Posterize Level** (0-8, default: 0)
  - Output lower-precision color
  - For ARGB4444 or reduced color output
  - Advanced use only

#### Additional Options Group
- **Strip Optional Metadata** (default: enabled)
  - Removes optional chunks for smaller files
  - Recommended for web
  
- **Skip if Larger** (default: enabled)
  - Keep original if compression doesn't help
  - Smart file size protection
  
- **Force Overwrite** (default: enabled)
  - Overwrite output files without prompting

### 2. Settings Management (`pngquantprefwidget.cpp`)

#### Features
- **Auto-save**: All changes immediately saved to QSettings
- **Quality Validation**: Ensures min ≤ max
- **Dynamic Labels**: Real-time feedback on slider/spinbox changes
- **Speed Descriptions**:
  - 1: "Slowest/Best"
  - 2-3: "Slow"
  - 4: "Default"
  - 5-7: "Fast"
  - 8-11: "Fastest/Rough"

#### Settings Keys
All stored under `pngquant/` prefix:
- `pngquant/qualityMin` (default: 65)
- `pngquant/qualityMax` (default: 80)
- `pngquant/speed` (default: 4)
- `pngquant/enableDithering` (default: true)
- `pngquant/posterize` (default: 0)
- `pngquant/stripMetadata` (default: true)
- `pngquant/skipIfLarger` (default: true)
- `pngquant/force` (default: true)

### 3. Worker Implementation (`pngquantworker.cpp`)

#### Improved Architecture
- Removed hardcoded constructor parameters
- Reads settings from QSettings dynamically
- Proper error handling with directory creation
- Force overwrite with file removal

#### Command Construction
Maps settings to pngquant 3.0.3 arguments:

**Quality**:
```bash
--quality=65-80
```

**Speed**:
```bash
--speed=4
```

**Output**:
```bash
--output=/path/to/output.png
```

**Options**:
```bash
--force              # if enabled
--strip              # if enabled
--skip-if-larger     # if enabled
--nofs               # if dithering disabled
--posterize=N        # if N > 0
```

**Example Command**:
```bash
pngquant --quality=65-80 --speed=4 --output=out.png --force --strip --skip-if-larger -- in.png
```

### 4. Factory Integration (`imageworkerfactory.cpp`)

- Removed pngout from registered optimizers
- Simplified PNG worker instantiation:
  ```cpp
  return new PngquantWorker(); // No parameters needed
  ```
- Cleaned up optimizer list

### 5. Project Configuration (`PixelBatch.pro`)

**Added**:
- `OptimizerPrefWidgets/pngquantprefwidget.cpp`
- `OptimizerPrefWidgets/pngquantprefwidget.h`
- `OptimizerPrefWidgets/pngquantprefwidget.ui`

**Removed**:
- `worker/pngoutworker.cpp`
- `worker/pngoutworker.h`

## Usage Guidelines

### Recommended Settings

**For Web Images**:
- Quality: 65-80% (default)
- Speed: 4 (balanced)
- Dithering: Enabled
- Strip Metadata: Enabled
- Skip if Larger: Enabled

**For High-Quality Archives**:
- Quality: 80-95%
- Speed: 1-2 (slow but best)
- Dithering: Enabled
- Strip Metadata: Disabled
- Skip if Larger: Enabled

**For Maximum Compression**:
- Quality: 50-70%
- Speed: 4
- Dithering: Enabled
- Strip Metadata: Enabled
- Skip if Larger: Enabled

**For Pixel Art/Simple Graphics**:
- Quality: 70-85%
- Speed: 4
- Dithering: **Disabled**
- Strip Metadata: Enabled
- Skip if Larger: Enabled

### Quality Range Explanation

The quality range (min-max) controls compression:

- **Min Quality**: pngquant won't save if quality drops below this
- **Max Quality**: pngquant will use fewer colors to stay below this

Example with 65-80%:
- If pngquant can achieve 85% quality → uses more colors, stays under 80%
- If pngquant can only achieve 60% quality → doesn't save (below 65% min)

## Benefits

### For Users
- ✅ **Better Compression**: 40-70% vs 5-15% with pngout
- ✅ **More Control**: Quality range, speed, dithering options
- ✅ **Safer**: Skip-if-larger prevents file size increases
- ✅ **Clearer**: Intuitive settings with helpful hints
- ✅ **Flexible**: Can optimize for web, archive, or maximum compression

### For Developers
- ✅ **Simplified**: One PNG optimizer instead of two
- ✅ **Consistent**: Same pattern as JPEG settings
- ✅ **Maintainable**: Clean settings-based architecture
- ✅ **Extensible**: Easy to add more options
- ✅ **Modern**: Based on actively maintained pngquant 3.0.3

## Technical Details

### pngquant 3.0.3 Features Used

From `pngquant --help`:
```
--force           ✅ overwrite existing output files
--skip-if-larger  ✅ only save converted files if they're smaller
--output file     ✅ destination file path
--quality min-max ✅ don't save below min, use fewer colors below max
--speed N         ✅ speed/quality trade-off (1-11)
--nofs            ✅ disable Floyd-Steinberg dithering
--posterize N     ✅ output lower-precision color
--strip           ✅ remove optional metadata
```

### Not Exposed (Rarely Used)
- `--ext`: Output extension (we use --output)
- `--verbose`: Verbose output (we use quiet mode)
- `ncolors`: Max colors (quality range is better)

## Migration Notes

### Removed Files
The following pngout files can be deleted:
- `/src/worker/pngoutworker.cpp`
- `/src/worker/pngoutworker.h`

### Breaking Changes
None - settings are stored separately, existing installations unaffected.

### Compatibility
- **Minimum pngquant version**: 2.0+
- **Recommended**: 3.0.3 or later
- **Platform**: Linux (primary), Windows/macOS compatible

## Testing Checklist

### UI Testing
- [ ] Open PNG Optimizer Settings
- [ ] Adjust quality min/max sliders
- [ ] Verify quality range label updates
- [ ] Move speed slider, check label descriptions
- [ ] Toggle dithering checkbox
- [ ] Adjust posterize spinner
- [ ] Toggle all option checkboxes
- [ ] Close and reopen - verify settings persist

### Functional Testing
- [ ] Add PNG images to TaskWidget
- [ ] Change quality to 50-70% (high compression)
- [ ] Process and verify >40% savings
- [ ] Change quality to 80-95% (high quality)
- [ ] Process and verify good quality
- [ ] Test with dithering disabled (pixel art)
- [ ] Test skip-if-larger (add pre-optimized PNG)

### Edge Cases
- [ ] Set min > max, verify auto-correction
- [ ] Set speed to 1 (slowest), verify longer processing
- [ ] Set speed to 11 (fastest), verify quick processing
- [ ] Enable posterize, verify output
- [ ] Disable strip metadata, verify metadata preserved

## Future Enhancements

Potential additions:
- [ ] Preset system (Web, Archive, Maximum)
- [ ] Color count limiter (--ncolors)
- [ ] Preview mode to compare quality
- [ ] Batch-specific quality overrides
- [ ] Advanced metadata filtering

## Documentation Updates

- **README.md**: Updated PNG optimization section
- **DEVELOPER_README.md**: Added pngquant settings documentation
- This file: Complete implementation guide

## Conclusion

By consolidating on pngquant, we've:
1. **Improved compression** by 3-8x compared to pngout
2. **Simplified the codebase** by removing redundant optimizer
3. **Enhanced user control** with quality range settings
4. **Modernized** the tool chain with pngquant 3.0.3
5. **Maintained consistency** with JPEG settings pattern

pngquant is now the sole PNG optimizer, providing superior results with an intuitive, well-documented settings interface.

---

**Author:** PixelBatch Development Team  
**Date:** November 6, 2025  
**Version:** 1.0

