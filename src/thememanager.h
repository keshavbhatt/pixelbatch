#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyleFactory>

class ThemeManager {
public:
  static void applyTheme(const QString &theme, bool useSystemTheme = false) {
    if (useSystemTheme) {
      // Use system's default palette
      qApp->setPalette(QPalette());
      return;
    }

    if (theme == "Dark") {
      // Qt Creator Dark Theme - Professional dark color scheme
      QPalette darkPalette;

      // Background colors
      darkPalette.setColor(QPalette::Window,
                           QColor(45, 45, 48)); // Main window background
      darkPalette.setColor(QPalette::Base,
                           QColor(30, 30, 30)); // Input fields, text areas
      darkPalette.setColor(QPalette::AlternateBase,
                           QColor(42, 42, 45)); // Alternating rows
      darkPalette.setColor(QPalette::Button, QColor(60, 60, 63)); // Buttons

      // Text colors
      darkPalette.setColor(QPalette::WindowText,
                           QColor(224, 224, 224)); // General text
      darkPalette.setColor(QPalette::Text, QColor(224, 224, 224)); // Input text
      darkPalette.setColor(QPalette::ButtonText,
                           QColor(224, 224, 224)); // Button text
      darkPalette.setColor(QPalette::BrightText,
                           QColor(255, 128, 128)); // Bright/error text

      // Tooltip
      darkPalette.setColor(QPalette::ToolTipBase,
                           QColor(80, 80, 80)); // Tooltip background
      darkPalette.setColor(QPalette::ToolTipText,
                           QColor(224, 224, 224)); // Tooltip text

      // Selection/Highlight
      darkPalette.setColor(
          QPalette::Highlight,
          QColor(0, 122, 204)); // Selection background (Qt Creator blue)
      darkPalette.setColor(QPalette::HighlightedText,
                           Qt::white); // Selected text

      // Links
      darkPalette.setColor(QPalette::Link, QColor(76, 169, 255)); // Hyperlinks
      darkPalette.setColor(QPalette::LinkVisited,
                           QColor(153, 153, 255)); // Visited links

      // Disabled state
      darkPalette.setColor(QPalette::Disabled, QPalette::WindowText,
                           QColor(128, 128, 128));
      darkPalette.setColor(QPalette::Disabled, QPalette::Text,
                           QColor(128, 128, 128));
      darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText,
                           QColor(128, 128, 128));
      darkPalette.setColor(QPalette::Disabled, QPalette::Base,
                           QColor(40, 40, 40));
      darkPalette.setColor(QPalette::Disabled, QPalette::Button,
                           QColor(50, 50, 50));

      qApp->setPalette(darkPalette);
    } else {
      // Qt Creator Light Theme - Clean professional light scheme
      QPalette lightPalette;

      // Background colors
      lightPalette.setColor(QPalette::Window,
                            QColor(239, 239, 239)); // Main window background
      lightPalette.setColor(QPalette::Base,
                            Qt::white); // Input fields, text areas
      lightPalette.setColor(QPalette::AlternateBase,
                            QColor(247, 247, 247)); // Alternating rows
      lightPalette.setColor(QPalette::Button, QColor(239, 239, 239)); // Buttons

      // Text colors
      lightPalette.setColor(QPalette::WindowText, Qt::black); // General text
      lightPalette.setColor(QPalette::Text, Qt::black);       // Input text
      lightPalette.setColor(QPalette::ButtonText, Qt::black); // Button text
      lightPalette.setColor(QPalette::BrightText,
                            QColor(255, 0, 0)); // Bright/error text

      // Tooltip
      lightPalette.setColor(QPalette::ToolTipBase,
                            QColor(255, 255, 220)); // Tooltip background
      lightPalette.setColor(QPalette::ToolTipText, Qt::black); // Tooltip text

      // Selection/Highlight
      lightPalette.setColor(
          QPalette::Highlight,
          QColor(0, 122, 204)); // Selection background (Qt Creator blue)
      lightPalette.setColor(QPalette::HighlightedText,
                            Qt::white); // Selected text

      // Links
      lightPalette.setColor(QPalette::Link, QColor(0, 85, 255)); // Hyperlinks
      lightPalette.setColor(QPalette::LinkVisited,
                            QColor(130, 80, 180)); // Visited links

      // Disabled state
      lightPalette.setColor(QPalette::Disabled, QPalette::WindowText,
                            QColor(150, 150, 150));
      lightPalette.setColor(QPalette::Disabled, QPalette::Text,
                            QColor(150, 150, 150));
      lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText,
                            QColor(150, 150, 150));
      lightPalette.setColor(QPalette::Disabled, QPalette::Base,
                            QColor(245, 245, 245));
      lightPalette.setColor(QPalette::Disabled, QPalette::Button,
                            QColor(230, 230, 230));

      qApp->setPalette(lightPalette);
    }
  }

  static void applyStyle(const QString &style, bool useSystemTheme = false) {
    if (useSystemTheme) {
      // Don't change style when using system theme
      return;
    }
    QApplication::setStyle(QStyleFactory::create(style));
  }

private:
  ThemeManager() = default; // Prevent instantiation
};

#endif // THEMEMANAGER_H
