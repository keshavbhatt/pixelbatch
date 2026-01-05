#ifndef IMAGEOPTIMIZERPREFWIDGET_H
#define IMAGEOPTIMIZERPREFWIDGET_H

#include <QVariantMap>
#include <QWidget>

class ImageOptimizerPrefWidget : public QWidget {
  Q_OBJECT

public:
  explicit ImageOptimizerPrefWidget(QWidget *parent = nullptr);
  virtual ~ImageOptimizerPrefWidget() {}
  virtual void loadSettings() = 0;

  // Load settings from a custom QVariantMap instead of global QSettings
  virtual void loadCustomSettings(const QVariantMap &settings) { Q_UNUSED(settings); }

  // Get current settings as QVariantMap (for saving custom settings)
  virtual QVariantMap getCurrentSettings() const { return QVariantMap(); }

  // Disable auto-save to global QSettings (used in detail panel)
  virtual void setAutoSaveEnabled(bool enabled) { Q_UNUSED(enabled); }
};

#endif // IMAGEOPTIMIZERPREFWIDGET_H
