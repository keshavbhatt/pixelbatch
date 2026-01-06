# PixelBatch v2.0 — Changelog

## 🚀 Major Features

### Per-Image Custom Settings
Configure optimization settings per image instead of using only global defaults.

- Per-image output folder and filename prefix
- Per-image optimizer settings (JPEG, PNG, GIF, SVG)
- Settings are fully isolated from global preferences
- Visual indicator for images with custom settings
- Apply current settings to selected image only
- Reset image back to global defaults
- Custom settings persist across re-processing

**Use cases:** mix high-quality assets and aggressively optimized thumbnails in one batch.

---

### Multi-Selection with Checkboxes
Select specific images and perform batch actions.

- Toggleable checkbox column (`Ctrl+E`)
- Process, re-process, or remove only selected images
- Check all / uncheck all shortcuts
- Dedicated **Selection** menu for batch actions
- Checkboxes stay hidden when not needed

**Shortcuts**
- `Ctrl+E` Toggle checkboxes
- `Ctrl+Shift+A` Check all
- `Ctrl+Shift+D` Uncheck all
- `Ctrl+Shift+P` Process selected
- `Ctrl+Shift+R` Remove selected

---

## ✨ UI / UX Improvements

- Auto-scroll to currently processing image
- Completion dialog with success / failure summary
- Improved About dialog with icons, smooth resize, and ESC support
- Consistent status messages when adding images
- Status bar shows total image count and summary

### Optimizer Preferences
- Minimum dialog size (500×400)
- Window size saved per optimizer
- One-click **Restore Defaults** button per optimizer (with confirmation)

---

## 🐞 Bug Fixes

- Output folder and prefix changes apply immediately (no restart)
- Fixed settings leakage between per-image and global configuration
- Fixed UI glitches when resetting custom settings
- Fixed optimizer dialog sizing and persistence
- Fixed About dialog resize flicker

---

**Release Date:** January 2026
