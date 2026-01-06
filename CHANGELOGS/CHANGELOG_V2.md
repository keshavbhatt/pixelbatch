# PixelBatch v2.0 - Changelog

## 🎉 Major Features

### Per-Image Custom Settings
Configure unique optimization settings for each image instead of applying global settings to everything.

- **Custom Output Directory**: Set a different output folder for specific images
- **Custom Output Prefix**: Define custom filename prefixes per image
- **Custom Optimization Settings**: Apply different quality, compression, and metadata settings to individual images
  - Each image can have its own JPEG quality, PNG compression level, GIF optimization, or SVG settings
  - Settings are isolated - changes in the detail panel don't affect global settings
  - Visual indicator shows when an image has custom settings
  - "Apply to This Image" button saves current settings for the selected image only
  - "Reset to Global" button removes custom settings and reverts to global defaults
  - Custom settings persist across re-processing

**Use Cases:**
- Apply maximum quality to product photos while using aggressive compression for thumbnails
- Set different metadata stripping rules for different image types
- Optimize images with varying quality requirements in a single batch

### Multi-Selection with Checkboxes
Perform batch operations on specific images using checkboxes for flexible selection.

- **Smart Checkbox Column**: Hidden by default, toggle with `Ctrl+E` or via Selection menu
- **Batch Operations**:
  - Process only selected pending images
  - Re-process selected completed or error images
  - Remove multiple images at once with confirmation
- **Selection Management**:
  - Check All (`Ctrl+Shift+A`)
  - Uncheck All (`Ctrl+Shift+D`)
  - Checkboxes automatically hidden when not in use
- **Clean UI**: No clutter - checkboxes only appear when you need them
- **Menu Integration**: Dedicated "Selection" menu with all batch operations

**Keyboard Shortcuts:**
- `Ctrl+E` - Toggle checkbox visibility
- `Ctrl+Shift+A` - Check all items
- `Ctrl+Shift+D` - Uncheck all items
- `Ctrl+Shift+P` - Process checked items
- `Ctrl+Shift+R` - Remove checked items

## ✨ UI/UX Improvements

### Auto-Scroll to Processing Items
- Table automatically scrolls to show currently processing images
- Keep track of optimization progress even with hundreds of images in the list
- Item centered in viewport for best visibility
- No more losing sight of what's being optimized

### Completion Notification
- Dialog notification shown when all images finish processing
- Summary includes counts of successful, failed, and total images
- Only appears when there are actually tasks to process
- Helps you know when a long batch is complete

### Enhanced About Dialog
- **Icon-Enhanced Buttons**: Beautiful, intuitive icons for all actions
  - ❤️ Heart icon for "Support Project"
  - 🐛 Bug icon for "Report Issue"
  - 🐙 GitHub icon for "Source Code"
  - 🌐 Globe icon for "Website"
  - 💻 Terminal icon for "Debug Info"
- **Smooth Resize Animation**: Fixed glitchy behavior when toggling debug info
- **ESC Key Support**: Close dialog quickly with Escape key
- **Consistent Layout**: Proper size calculations prevent UI jumps

### Improved Status Messages
- **Consistent Feedback**: All image addition methods show the same format
  - Drag & drop shows count of added images
  - "Add Images" button shows count of added images
  - Menu action shows count of added images
- **Total Image Count**: Status bar displays total images and summary statistics
- **Clear Operation Results**: Every action provides immediate feedback

## 🔧 Bug Fixes

### Output Folder Settings Fix
- **No Restart Required**: Output folder and prefix changes apply immediately
- **Per-Task Generation**: Output paths regenerated on each processing attempt
- **Settings Synchronization**: Custom and global output settings work seamlessly together
- **Backward Compatibility**: Existing tasks automatically use updated paths

### About Dialog Resize Fix
- Eliminated resize glitch when toggling debug information
- Smooth expansion and contraction animations
- Proper layout invalidation before size adjustments
- Consistent behavior for both show and hide operations

### Settings Isolation Fix
- Changes in detail panel optimizer settings no longer affect global settings
- Auto-save disabled for detail panel widgets
- Preferences dialog continues to save globally as expected
- Proper separation between per-task and global configuration

