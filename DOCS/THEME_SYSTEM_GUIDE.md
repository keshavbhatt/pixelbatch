# Theme and Style System - Comprehensive Guide

## Overview
PixelBatch features a flexible theming system that allows users to customize the application's appearance or use their system's default theme and style.

## Features

### 1. **Custom Themes (Qt Creator Inspired)**
- **Light Theme**: Professional light color scheme based on Qt Creator's light theme
- **Dark Theme**: Professional dark color scheme based on Qt Creator's dark theme
- **Signature Qt Blue**: Uses Qt Creator's `#007ACC` highlight color in both themes
- **Soft Text Colors**: Dark theme uses `rgb(224, 224, 224)` instead of pure white for reduced eye strain
- **Disabled State Support**: Proper visual feedback for disabled controls
- **Link Styling**: Blue hyperlinks, purple visited links (standard web convention)

### 2. **Widget Styles**
- Dynamically detects all available Qt styles on your system
- Common styles include: Fusion, Windows, WindowsVista, GTK+, etc.
- Default: Fusion (cross-platform modern design)

### 3. **System Theme Integration**
- **Use System Theme** checkbox to override custom settings
- Automatically uses your operating system's theme and style
- Perfect for users who want native OS appearance

### 4. **Persistent Settings**
- All preferences saved automatically
- Theme/style restored on application startup
- Settings survive across sessions

---

## User Guide

### Accessing Theme Settings

1. Open **Edit → Preferences** (or Settings)
2. Navigate to **Appearance** section
3. Configure your preferred theme options

### Using System Theme (Recommended for OS Integration)

**To use your system's default appearance:**

1. Check **"Use System Theme and Style"**
2. Theme and Style dropdowns become disabled
3. Application immediately adopts system appearance
4. Setting persists across restarts

**When to use:**
- ✅ You want PixelBatch to match your desktop environment
- ✅ You're on KDE/GNOME and want native look
- ✅ You prefer your OS-wide theme settings
- ✅ You want consistent appearance across all apps

### Using Custom Theme

**To customize the application appearance:**

1. Uncheck **"Use System Theme and Style"**
2. Theme and Style dropdowns become enabled
3. Select your preferred **Color Theme** (Light or Dark)
4. Select your preferred **Widget Style**
5. Changes apply immediately

**When to use:**
- ✅ You want Dark theme regardless of system settings
- ✅ You prefer a specific widget style
- ✅ You want the app to look different from other apps
- ✅ You're testing different visual styles

### Changing Color Theme

1. Ensure **"Use System Theme"** is unchecked
2. Open **Color Theme** dropdown
3. Select:
   - **Light**: Bright backgrounds, dark text
   - **Dark**: Dark backgrounds, light text
4. Theme applies instantly

### Changing Widget Style

1. Ensure **"Use System Theme"** is unchecked
2. Open **Widget Style** dropdown
3. Select from available styles (varies by platform)
4. Style applies instantly

---

## Theme Details

### Light Theme (Qt Creator Inspired)

**Color Palette:**

| Element | Color Name | Hex Code | RGB | Usage |
|---------|-----------|----------|-----|-------|
| Window Background | Light Gray | `#EFEFEF` | `rgb(239, 239, 239)` | Main window, dialogs |
| Base (Input Fields) | White | `#FFFFFF` | `rgb(255, 255, 255)` | Text boxes, tables |
| Text | Black | `#000000` | `rgb(0, 0, 0)` | All text |
| Button Background | Light Gray | `#EFEFEF` | `rgb(239, 239, 239)` | Buttons |
| Button Text | Black | `#000000` | `rgb(0, 0, 0)` | Button labels |
| Alternate Base | Very Light Gray | `#F7F7F7` | `rgb(247, 247, 247)` | Table alternating rows |
| Highlight | Qt Blue | `#007ACC` | `rgb(0, 122, 204)` | Selected items |
| Highlighted Text | White | `#FFFFFF` | `rgb(255, 255, 255)` | Text on selection |
| Link | Deep Blue | `#0055FF` | `rgb(0, 85, 255)` | Hyperlinks |
| Link Visited | Purple | `#8250B4` | `rgb(130, 80, 180)` | Visited links |
| Tooltip Base | Light Yellow | `#FFFFDC` | `rgb(255, 255, 220)` | Tooltip background |
| Tooltip Text | Black | `#000000` | `rgb(0, 0, 0)` | Tooltip text |
| Disabled Text | Medium Gray | — | `rgb(150, 150, 150)` | Disabled controls |
| Disabled Base | Very Light Gray | — | `rgb(245, 245, 245)` | Disabled inputs |
| Disabled Button | Light Gray | — | `rgb(230, 230, 230)` | Disabled buttons |

**Best For:**
- Bright environments
- Daytime use
- High ambient light
- Users who prefer traditional UI
- Professional work environments
- Qt developers (familiar Qt Creator look)

### Dark Theme (Qt Creator Inspired)

**Color Palette:**

| Element | Color Name | Hex Code | RGB | Usage |
|---------|-----------|----------|-----|-------|
| Window Background | Professional Dark | `#2D2D30` | `rgb(45, 45, 48)` | Main window, dialogs |
| Base (Input Fields) | Editor Dark | `#1E1E1E` | `rgb(30, 30, 30)` | Text boxes, tables |
| Text | Soft White | `#E0E0E0` | `rgb(224, 224, 224)` | All text |
| Button Background | Button Gray | `#3C3C3F` | `rgb(60, 60, 63)` | Buttons |
| Button Text | Soft White | `#E0E0E0` | `rgb(224, 224, 224)` | Button labels |
| Alternate Base | Alt Dark | `#2A2A2D` | `rgb(42, 42, 45)` | Table alternating rows |
| Highlight | Qt Blue | `#007ACC` | `rgb(0, 122, 204)` | Selected items |
| Highlighted Text | White | `#FFFFFF` | `rgb(255, 255, 255)` | Text on selection |
| Link | Bright Blue | `#4CA9FF` | `rgb(76, 169, 255)` | Hyperlinks |
| Link Visited | Purple Blue | `#9999FF` | `rgb(153, 153, 255)` | Visited links |
| Bright Text | Salmon | `#FF8080` | `rgb(255, 128, 128)` | Error/warning text |
| Tooltip Base | Dark Tooltip | `#505050` | `rgb(80, 80, 80)` | Tooltip background |
| Tooltip Text | Soft White | `#E0E0E0` | `rgb(224, 224, 224)` | Tooltip text |
| Disabled Text | Medium Gray | — | `rgb(128, 128, 128)` | Disabled controls |
| Disabled Base | Darker Gray | — | `rgb(40, 40, 40)` | Disabled inputs |
| Disabled Button | Dark Gray | — | `rgb(50, 50, 50)` | Disabled buttons |

**Best For:**
- Low-light environments
- Nighttime use
- Reduced eye strain (softer white text)
- Users sensitive to bright screens
- Extended work sessions
- Qt developers (familiar Qt Creator look)
- OLED screens (power saving)

---

## Widget Styles Guide

### Available Styles (Platform-Dependent)

#### **Fusion** (Default, Cross-Platform)
- Modern, flat design
- Consistent across all platforms
- Clean, professional appearance
- Recommended for most users

#### **Windows** (Windows Only)
- Native Windows look
- Matches Windows 7/8/10 classic style
- Familiar to Windows users

#### **WindowsVista** (Windows Vista+)
- Modern Windows appearance
- Aero-inspired design
- Best on Windows 10/11

#### **GTK+** (Linux Only)
- Native GNOME/GTK appearance
- Integrates with GTK themes
- Best for GNOME desktop users

#### **Macintosh** (macOS Only)
- Native macOS appearance
- Aqua-inspired design
- Best for Mac users

#### **Plastique** (Cross-Platform)
- Polished, modern look
- Similar to Fusion but more detailed

#### **Cleanlooks** (Cross-Platform)
- Minimalist, clean design
- Simple and unobtrusive

**Note:** Available styles depend on your platform and Qt installation.

---

## Settings Storage

### Location

Settings are stored in platform-specific locations:

**Linux:**
```
~/.config/PixelBatch/PixelBatch.conf
```

**Windows:**
```
HKEY_CURRENT_USER\Software\PixelBatch
```

**macOS:**
```
~/Library/Preferences/com.pixelbatch.PixelBatch.plist
```

### Format (Linux Example)

```ini
[appearance]
theme=Dark
style=Fusion
useSystemTheme=false
```

### Settings Keys

| Setting | Key | Default | Type |
|---------|-----|---------|------|
| Color Theme | `appearance/theme` | `"Light"` | String |
| Widget Style | `appearance/style` | `"Fusion"` | String |
| Use System Theme | `appearance/useSystemTheme` | `false` | Boolean |

---

## Technical Implementation

### Architecture

**ThemeManager** (Static Utility Class)
- Centralized theme application logic
- No instances needed (all static methods)
- Single source of truth for palettes

**Settings** (Singleton)
- Persistent storage via QSettings
- Getter/setter methods for all theme settings

**PreferencesWidget**
- User interface for theme configuration
- Real-time preview
- Auto-save on changes

**PixelBatch** (Main Window)
- Applies theme on startup
- Coordinates theme changes

### Theme Application Flow

**On Application Startup:**
```
main() 
→ PixelBatch constructor
→ setupUi()
→ applySavedThemeAndStyle()
→ Check useSystemTheme setting
→ If true: Apply QPalette() (system default)
→ If false: Apply custom theme + style
→ UI rendered with selected appearance
```

**When User Changes Theme:**
```
User selects theme
→ onThemeChanged(index)
→ Save to QSettings
→ ThemeManager::applyTheme(theme, useSystemTheme)
→ Create QPalette with colors
→ qApp->setPalette(palette)
→ UI updates immediately
```

**When User Toggles System Theme:**
```
User checks "Use System Theme"
→ onUseSystemThemeToggled(true)
→ Save to QSettings
→ Disable theme/style controls
→ ThemeManager::applyTheme("", true)
→ Apply QPalette() (resets to system)
→ UI updates immediately
```

### Code Example

**Applying a Theme:**
```cpp
// Use custom dark theme
ThemeManager::applyTheme("Dark", false);

// Use custom light theme
ThemeManager::applyTheme("Light", false);

// Use system theme
ThemeManager::applyTheme("", true);
```

**Applying a Style:**
```cpp
// Use Fusion style
ThemeManager::applyStyle("Fusion", false);

// Use system style
ThemeManager::applyStyle("", true);
```

**Complete Theme Switch:**
```cpp
bool useSystem = m_settings.getUseSystemTheme();

if (useSystem) {
    ThemeManager::applyTheme("", true);
    ThemeManager::applyStyle("", true);
} else {
    QString theme = m_settings.getTheme();
    QString style = m_settings.getStyle();
    ThemeManager::applyTheme(theme, false);
    ThemeManager::applyStyle(style, false);
}
```

---

## Use Cases & Recommendations

### Scenario 1: Developer on Linux (GNOME)
**Recommendation:** Use System Theme
- Matches GNOME's Adwaita theme
- Consistent with other apps
- Respects system dark mode preference

**Settings:**
- ✅ Use System Theme and Style

### Scenario 2: Late-Night Work
**Recommendation:** Custom Dark Theme
- Reduces eye strain
- Works regardless of system theme
- Consistent experience

**Settings:**
- ☐ Use System Theme and Style
- Color Theme: Dark
- Widget Style: Fusion

### Scenario 3: Professional Environment
**Recommendation:** Custom Light Theme
- Traditional, professional appearance
- High contrast for visibility
- Platform-consistent with Fusion style

**Settings:**
- ☐ Use System Theme and Style
- Color Theme: Light
- Widget Style: Fusion

### Scenario 4: Windows User
**Recommendation:** Windows Style with Light Theme
- Native Windows appearance
- Familiar to Windows users
- Integrates with Windows 10/11

**Settings:**
- ☐ Use System Theme and Style
- Color Theme: Light
- Widget Style: WindowsVista

### Scenario 5: macOS User
**Recommendation:** Use System Theme
- Native macOS Aqua appearance
- Respects macOS light/dark mode
- Perfect integration

**Settings:**
- ✅ Use System Theme and Style

---

## Troubleshooting

### Issue: Theme doesn't persist after restart
**Solution:**
- Check QSettings file permissions
- Verify settings file exists at correct location
- Check for errors in terminal/console

### Issue: Dark theme text is unreadable
**Solution:**
- Some custom widgets may not fully respect theme
- Try different widget style (Fusion recommended)
- Report issue if specific widget is problematic

### Issue: Style doesn't change
**Solution:**
- Verify style name is correct (case-sensitive)
- Check if style is available: `QStyleFactory::keys()`
- Some styles may not be available on your platform

### Issue: System theme doesn't match OS
**Solution:**
- Restart the application
- Check if OS theme has changed since app started
- QPalette() reads system palette at app startup

### Issue: Checkbox is grayed out
**Solution:**
- This shouldn't happen - may indicate UI state issue
- Close and reopen preferences dialog
- Restart application if problem persists

---

## Keyboard Shortcuts

Currently, there are no keyboard shortcuts for theme switching. Themes are changed via the preferences dialog.

**Future Enhancement Ideas:**
- Ctrl+T: Toggle between Light/Dark
- Ctrl+Shift+T: Toggle System Theme
- F11: Dark mode (common convention)

---

## Accessibility Considerations

### Light Theme
- ✅ High contrast (black text on white background)
- ✅ Readable for users with visual impairments
- ✅ Works well with screen readers
- ✅ Standard color scheme

### Dark Theme
- ✅ Reduces eye strain for light-sensitive users
- ✅ Helpful for users with photophobia
- ⚠️ May have lower contrast in some areas
- ⚠️ Not ideal for color-blind users (blue links on dark background)

### System Theme
- ✅ Respects OS-level accessibility settings
- ✅ Inherits system high-contrast mode if enabled
- ✅ Best option for accessibility-focused users

**Recommendation:** Users with accessibility needs should use **System Theme** to ensure proper integration with OS accessibility features.

---

## Future Enhancements

### Planned Features
- [ ] High Contrast theme option
- [ ] Custom color picker for user-defined themes
- [ ] More theme presets (Solarized, Nord, etc.)
- [ ] Per-widget custom styling
- [ ] Theme import/export functionality
- [ ] Theme preview before applying
- [ ] Automatic theme switching based on time of day
- [ ] Respect system dark mode (auto-detect and switch)
- [ ] Theme-aware icons (light/dark variants)

### Community Requests
Users can request features at: [GitHub Issues](https://github.com/keshavbhatt/pixelbatch/issues)

---

## FAQ

**Q: Why does my system theme look different from my DE theme?**
A: Qt reads the palette at application startup. If you change your system theme while PixelBatch is running, you need to restart the app.

**Q: Can I create my own custom theme?**
A: Currently, only Light and Dark themes are available. Custom themes are planned for a future release.

**Q: Will theme switching affect performance?**
A: No. Theme changes are instantaneous and have no impact on image processing performance.

**Q: Does dark theme save battery on OLED screens?**
A: Yes, dark pixels on OLED screens consume less power, so dark theme can extend battery life on laptops with OLED displays.

**Q: Why is Fusion the default style?**
A: Fusion provides a consistent, modern appearance across all platforms (Windows, Linux, macOS) and is well-maintained by Qt.

**Q: Can I use system theme but custom colors?**
A: Currently, it's either/or. This feature (partial customization) is planned for the future.

**Q: Does the theme affect file output?**
A: No. The theme only affects the application's appearance. Image optimization is completely independent of UI theme.

---

## Credits

**Color Palette Design:**
- Light Theme: Based on Qt's standard palette
- Dark Theme: Inspired by popular dark themes (VSCode Dark+, Sublime Text)

**Implementation:**
- ThemeManager: Centralized theme logic
- Qt Framework: QPalette and QStyle system
- Community Feedback: User-requested features

---

## Version History

**v1.0.0 (November 2025)**
- Initial theme system implementation
- Light and Dark themes
- Widget style selection
- System theme integration
- Settings persistence

---

## Summary

PixelBatch's theme system provides:

✅ **Flexibility**: Choose custom or system theme  
✅ **Comfort**: Dark theme for low-light work  
✅ **Integration**: Native look with system theme  
✅ **Persistence**: Settings saved automatically  
✅ **Performance**: Instant theme switching  
✅ **Accessibility**: Works with system accessibility features  

**Default Recommendation:** Start with **System Theme** enabled for best OS integration, then customize if needed.

---

**Document Version:** 1.0  
**Last Updated:** November 6, 2025  
**Applies To:** PixelBatch v1.0.0+

For issues or suggestions, please visit: [PixelBatch GitHub](https://github.com/keshavbhatt/pixelbatch)

