# Distribution Files Migration - ColorWall to PixelBatch

## Overview
Migrated all distribution files from ColorWall branding to PixelBatch, including desktop files, metainfo, and icons.

## Files Created

### Desktop Entry
**File:** `dist/linux/com.ktechpit.pixelbatch.desktop`

```ini
[Desktop Entry]
Name=PixelBatch
GenericName=Image Batch Optimizer
Comment=Batch image optimization tool for JPEG, PNG, GIF, and SVG files
Icon=com.ktechpit.pixelbatch
Exec=pixelbatch %F
Terminal=false
Type=Application
Categories=Graphics;Utility;Qt;
Keywords=Image;Optimization;Batch;JPEG;PNG;GIF;SVG;Compress;Optimizer;
StartupWMClass=pixelbatch
StartupNotify=true
MimeType=image/jpeg;image/png;image/gif;image/svg+xml;
```

**Features:**
- Proper application name and description
- Icon reference for system integration
- MIME type associations for image files
- Correct executable name
- Appropriate categories for app launchers

### AppStream Metadata
**File:** `dist/linux/com.ktechpit.pixelbatch.metainfo.xml`

**Key Elements:**
- Application ID: `com.ktechpit.pixelbatch`
- License: GPL-3.0-or-later
- Comprehensive feature list
- Keywords for searchability
- Release information
- URLs for homepage and bug tracker

### Application Icons

#### Scalable SVG Icon
**File:** `dist/linux/hicolor/scalable/apps/com.ktechpit.pixelbatch.svg`

**Design:**
- Blue gradient background (#007ACC to #005A9E - Qt Creator blue)
- Stack of 3 images representing batch processing
- Downward arrow indicating compression/optimization
- Percentage symbol showing size reduction
- 256x256 base size, fully scalable

#### Symbolic Icon
**File:** `dist/linux/hicolor/symbolic/apps/com.ktechpit.pixelbatch-symbolic.svg`

**Design:**
- Monochrome design using `currentColor`
- Simplified image stack (3 layers)
- Downward arrow for optimization
- 16x16 optimized for small sizes
- Used in system panels and trays

#### PNG Icons (Generated)
All generated from scalable SVG using `rsvg-convert`:

- `hicolor/16x16/apps/com.ktechpit.pixelbatch.png`
- `hicolor/32x32/apps/com.ktechpit.pixelbatch.png`
- `hicolor/64x64/apps/com.ktechpit.pixelbatch.png`
- `hicolor/128x128/apps/com.ktechpit.pixelbatch.png`
- `hicolor/256x256/apps/com.ktechpit.pixelbatch.png`

#### Application Window Icon
**File:** `src/resources/icons/app/icon-64.png`

- 64x64 PNG for window title bar
- Embedded in resources.qrc
- Referenced in `pixelbatch.cpp` via `setWindowIcon()`

### Icon Generation Script
**File:** `dist/linux/generate-icons.sh`

**Features:**
- Automatically generates all PNG sizes from SVG
- Supports both rsvg-convert and inkscape
- Proper path resolution
- Creates 5 sizes: 16, 32, 64, 128, 256

**Usage:**
```bash
cd dist/linux
./generate-icons.sh
```

## Files Removed

Cleaned up all old ColorWall branding files:
- ❌ `dist/linux/com.ktechpit.colorwall.desktop`
- ❌ `dist/linux/com.ktechpit.colorwall.metainfo.xml`
- ❌ `dist/linux/hicolor/scalable/apps/com.ktechpit.colorwall.svg`
- ❌ `dist/linux/hicolor/symbolic/apps/com.ktechpit.colorwall-symbolic.svg`

## Project File Configuration

### Installation Paths (PixelBatch.pro)

All installation paths already configured for PixelBatch:

```qmake
# Binary installation
target.path = /usr/bin

# Icon installation (all sizes)
icon16.path = /usr/share/icons/hicolor/16x16/apps/
icon16.files = ../dist/linux/hicolor/16x16/apps/com.ktechpit.pixelbatch.png

icon32.path = /usr/share/icons/hicolor/32x32/apps/
icon32.files = ../dist/linux/hicolor/32x32/apps/com.ktechpit.pixelbatch.png

icon64.path = /usr/share/icons/hicolor/64x64/apps/
icon64.files = ../dist/linux/hicolor/64x64/apps/com.ktechpit.pixelbatch.png

icon128.path = /usr/share/icons/hicolor/128x128/apps/
icon128.files = ../dist/linux/hicolor/128x128/apps/com.ktechpit.pixelbatch.png

icon256.path = /usr/share/icons/hicolor/256x256/apps/
icon256.files = ../dist/linux/hicolor/256x256/apps/com.ktechpit.pixelbatch.png

iconscalable.path = /usr/share/icons/hicolor/scalable/apps/
iconscalable.files = ../dist/linux/hicolor/scalable/apps/com.ktechpit.pixelbatch.svg

iconsymbolic.path = /usr/share/icons/hicolor/symbolic/apps/
iconsymbolic.files = ../dist/linux/hicolor/symbolic/apps/com.ktechpit.pixelbatch-symbolic.svg

# AppStream metadata
appstream.path = /usr/share/metainfo/
appstream.files = ../dist/linux/com.ktechpit.pixelbatch.metainfo.xml

# Desktop file
desktop.path = /usr/share/applications/
desktop.files = ../dist/linux/com.ktechpit.pixelbatch.desktop

# Installation targets
unix:INSTALLS += target icon16 icon32 icon64 icon128 icon256
unix:INSTALLS += iconscalable iconsymbolic appstream desktop
```

### Resource File
**File:** `src/resources.qrc`

Added app icon resource:
```xml
<qresource prefix="/icons/app">
    <file>resources/icons/app/icon-64.png</file>
</qresource>
```

## Directory Structure

```
dist/
└── linux/
    ├── com.ktechpit.pixelbatch.desktop
    ├── com.ktechpit.pixelbatch.metainfo.xml
    ├── generate-icons.sh
    └── hicolor/
        ├── 16x16/apps/
        │   └── com.ktechpit.pixelbatch.png
        ├── 32x32/apps/
        │   └── com.ktechpit.pixelbatch.png
        ├── 64x64/apps/
        │   └── com.ktechpit.pixelbatch.png
        ├── 128x128/apps/
        │   └── com.ktechpit.pixelbatch.png
        ├── 256x256/apps/
        │   └── com.ktechpit.pixelbatch.png
        ├── scalable/apps/
        │   └── com.ktechpit.pixelbatch.svg
        └── symbolic/apps/
            └── com.ktechpit.pixelbatch-symbolic.svg

src/
└── resources/
    └── icons/
        └── app/
            └── icon-64.png
```

## Installation

### Manual Installation
```bash
cd src
qmake
make
sudo make install
```

This will:
1. Install binary to `/usr/bin/pixelbatch`
2. Install all icons to `/usr/share/icons/hicolor/*/apps/`
3. Install desktop file to `/usr/share/applications/`
4. Install metainfo to `/usr/share/metainfo/`

### Post-Installation
After installation, update icon cache:
```bash
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
```

## Verification

### Check Desktop File
```bash
desktop-file-validate /usr/share/applications/com.ktechpit.pixelbatch.desktop
```

### Check AppStream Metadata
```bash
appstreamcli validate /usr/share/metainfo/com.ktechpit.pixelbatch.metainfo.xml
```

### Check Icons
```bash
ls -la /usr/share/icons/hicolor/*/apps/com.ktechpit.pixelbatch*
```

## Icon Design Rationale

### Color Scheme
- **Primary Blue:** `#007ACC` - Qt Creator's signature blue
- **Secondary Blue:** `#005A9E` - Darker shade for gradient
- **White:** `#FFFFFF` - For contrast and clarity

### Visual Elements
1. **Image Stack (3 layers):**
   - Represents batch processing
   - Shows multiple images
   - Depth through opacity

2. **Downward Arrow:**
   - Indicates compression
   - Shows size reduction
   - Universal optimization symbol

3. **Percentage Symbol:**
   - Represents file size reduction
   - Key feature visualization
   - Clear metric indicator

### Design Principles
- **Scalable:** SVG source ensures clarity at all sizes
- **Recognizable:** Distinct shape easy to identify in app launcher
- **Professional:** Clean, modern design matching Qt Creator
- **Symbolic:** Monochrome variant for system integration

## Dependencies for Icon Generation

**Required tools:**
- `rsvg-convert` (from librsvg package)
- OR `inkscape` (alternative)

**Arch Linux:**
```bash
sudo pacman -S librsvg
# or
sudo pacman -S inkscape
```

**Ubuntu/Debian:**
```bash
sudo apt install librsvg2-bin
# or
sudo apt install inkscape
```

## Regenerating Icons

If you modify the scalable SVG, regenerate PNG icons:

```bash
cd dist/linux
./generate-icons.sh
```

Then rebuild and reinstall the application.

## AppStream Keywords

The metainfo includes keywords in multiple languages for better discoverability:

**English:**
- Image, Optimization, Compress, JPEG, PNG, GIF, SVG
- Batch, Optimizer, jpegoptim, pngquant, gifsicle, svgo
- Lossless, Lossy, Graphics, Photo, Web

This ensures the app appears in searches for:
- Image optimization
- Batch processing
- Specific format names
- Optimizer tool names

## MIME Types

Desktop file associates with:
- `image/jpeg` - JPEG images
- `image/png` - PNG images
- `image/gif` - GIF images
- `image/svg+xml` - SVG vector graphics

This allows:
- "Open with" context menu integration
- Drag-and-drop from file managers
- Default application setting

## Categories

AppStream and desktop file categories:
- **Graphics** - Primary category (image processing)
- **Utility** - Secondary category (tool/utility)
- **Qt** - Technology indicator

Ensures proper placement in:
- Application launchers
- Software centers
- Menu systems

## Future Enhancements

Potential improvements:
- [ ] Add more icon sizes (512x512, 1024x1024)
- [ ] Create animated icon for progress indication
- [ ] Add theme variants (light/dark mode icons)
- [ ] Create Windows .ico file
- [ ] Create macOS .icns file
- [ ] Add screenshots to metainfo
- [ ] Translate desktop file strings
- [ ] Add more AppStream metadata

## Conclusion

All distribution files have been successfully migrated from ColorWall to PixelBatch:

✅ Desktop file created with proper application info  
✅ AppStream metainfo created with comprehensive metadata  
✅ Application icon designed and generated in all sizes  
✅ Symbolic icon created for system integration  
✅ Project file configured for installation  
✅ Resource file updated with app icon  
✅ Old ColorWall files removed  
✅ Icon generation script created for maintainability  

The application is now ready for packaging and distribution on Linux systems!

---

**Status:** ✅ Complete  
**Date:** November 6, 2025  
**Version:** 1.0.0

