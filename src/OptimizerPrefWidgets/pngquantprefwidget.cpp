#include "pngquantprefwidget.h"
#include "ui_pngquantprefwidget.h"

PngQuantPrefWidget::PngQuantPrefWidget(QWidget *parent)
    : ImageOptimizerPrefWidget(parent), ui(new Ui::PngQuantPrefWidget),
      m_settings(Settings::instance()) {
  ui->setupUi(this);

  // Load settings
  loadSettings();

  // Connect signals for auto-save
  connect(ui->qualityMinSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &PngQuantPrefWidget::saveSettings);
  connect(ui->qualityMaxSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &PngQuantPrefWidget::saveSettings);
  connect(ui->speedSlider, &QSlider::valueChanged,
          this, &PngQuantPrefWidget::saveSettings);
  connect(ui->ditheringCheckBox, &QCheckBox::toggled,
          this, &PngQuantPrefWidget::saveSettings);
  connect(ui->posterizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &PngQuantPrefWidget::saveSettings);
  connect(ui->stripMetadataCheckBox, &QCheckBox::toggled,
          this, &PngQuantPrefWidget::saveSettings);
  connect(ui->skipIfLargerCheckBox, &QCheckBox::toggled,
          this, &PngQuantPrefWidget::saveSettings);
  connect(ui->forceCheckBox, &QCheckBox::toggled,
          this, &PngQuantPrefWidget::saveSettings);

  // Update labels when values change
  connect(ui->qualityMinSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &PngQuantPrefWidget::updateQualityRangeLabel);
  connect(ui->qualityMaxSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &PngQuantPrefWidget::updateQualityRangeLabel);
  connect(ui->speedSlider, &QSlider::valueChanged, this, [this](int value) {
    QString label;
    if (value == 1) {
      label = "1 (Slowest/Best)";
    } else if (value <= 3) {
      label = QString("%1 (Slow)").arg(value);
    } else if (value == 4) {
      label = "4 (Default)";
    } else if (value <= 7) {
      label = QString("%1 (Fast)").arg(value);
    } else {
      label = QString("%1 (Fastest/Rough)").arg(value);
    }
    ui->speedValueLabel->setText(label);
  });

  // Initial label updates
  updateQualityRangeLabel();
  ui->speedValueLabel->setText("4 (Default)");
}

PngQuantPrefWidget::~PngQuantPrefWidget() { delete ui; }

void PngQuantPrefWidget::loadSettings() {
  const QSettings &settings = m_settings.getSettings();

  // Quality settings
  ui->qualityMinSpinBox->setValue(
      settings.value("pngquant/qualityMin", 65).toInt());
  ui->qualityMaxSpinBox->setValue(
      settings.value("pngquant/qualityMax", 80).toInt());

  // Speed setting
  ui->speedSlider->setValue(
      settings.value("pngquant/speed", 4).toInt());

  // Dithering
  ui->ditheringCheckBox->setChecked(
      settings.value("pngquant/enableDithering", true).toBool());

  // Posterize
  ui->posterizeSpinBox->setValue(
      settings.value("pngquant/posterize", 0).toInt());

  // Additional options
  ui->stripMetadataCheckBox->setChecked(
      settings.value("pngquant/stripMetadata", true).toBool());
  ui->skipIfLargerCheckBox->setChecked(
      settings.value("pngquant/skipIfLarger", true).toBool());
  ui->forceCheckBox->setChecked(
      settings.value("pngquant/force", true).toBool());
}

void PngQuantPrefWidget::saveSettings() {
  // Don't save to global settings if auto-save is disabled (used in detail panel)
  if (!m_autoSaveEnabled) {
    return;
  }

  QSettings &settings = const_cast<QSettings&>(m_settings.getSettings());

  // Quality settings
  int minQuality = ui->qualityMinSpinBox->value();
  int maxQuality = ui->qualityMaxSpinBox->value();

  // Ensure min <= max
  if (minQuality > maxQuality) {
    minQuality = maxQuality;
    ui->qualityMinSpinBox->setValue(minQuality);
  }

  settings.setValue("pngquant/qualityMin", minQuality);
  settings.setValue("pngquant/qualityMax", maxQuality);

  // Speed setting
  settings.setValue("pngquant/speed", ui->speedSlider->value());

  // Dithering
  settings.setValue("pngquant/enableDithering", ui->ditheringCheckBox->isChecked());

  // Posterize
  settings.setValue("pngquant/posterize", ui->posterizeSpinBox->value());

  // Additional options
  settings.setValue("pngquant/stripMetadata", ui->stripMetadataCheckBox->isChecked());
  settings.setValue("pngquant/skipIfLarger", ui->skipIfLargerCheckBox->isChecked());
  settings.setValue("pngquant/force", ui->forceCheckBox->isChecked());

  settings.sync();
}

void PngQuantPrefWidget::updateQualityRangeLabel() {
  int minQuality = ui->qualityMinSpinBox->value();
  int maxQuality = ui->qualityMaxSpinBox->value();

  ui->qualityRangeLabel->setText(
      QString("Quality Range: %1-%2%").arg(minQuality).arg(maxQuality));
}

void PngQuantPrefWidget::loadCustomSettings(const QVariantMap &settings) {
  // Block signals to prevent auto-save while loading
  ui->qualityMinSpinBox->blockSignals(true);
  ui->qualityMaxSpinBox->blockSignals(true);
  ui->speedSlider->blockSignals(true);
  ui->ditheringCheckBox->blockSignals(true);
  ui->posterizeSpinBox->blockSignals(true);
  ui->stripMetadataCheckBox->blockSignals(true);
  ui->skipIfLargerCheckBox->blockSignals(true);
  ui->forceCheckBox->blockSignals(true);

  // Load from custom settings map
  ui->qualityMinSpinBox->setValue(settings.value("pngquant/qualityMin", 65).toInt());
  ui->qualityMaxSpinBox->setValue(settings.value("pngquant/qualityMax", 80).toInt());
  ui->speedSlider->setValue(settings.value("pngquant/speed", 4).toInt());
  ui->ditheringCheckBox->setChecked(settings.value("pngquant/enableDithering", true).toBool());
  ui->posterizeSpinBox->setValue(settings.value("pngquant/posterize", 0).toInt());
  ui->stripMetadataCheckBox->setChecked(settings.value("pngquant/stripMetadata", true).toBool());
  ui->skipIfLargerCheckBox->setChecked(settings.value("pngquant/skipIfLarger", true).toBool());
  ui->forceCheckBox->setChecked(settings.value("pngquant/force", true).toBool());

  // Update labels
  updateQualityRangeLabel();
  int speed = ui->speedSlider->value();
  QString label;
  if (speed == 1) {
    label = "1 (Slowest/Best)";
  } else if (speed <= 3) {
    label = QString("%1 (Slow)").arg(speed);
  } else if (speed == 4) {
    label = "4 (Default)";
  } else if (speed <= 7) {
    label = QString("%1 (Fast)").arg(speed);
  } else {
    label = QString("%1 (Fastest/Rough)").arg(speed);
  }
  ui->speedValueLabel->setText(label);

  // Unblock signals
  ui->qualityMinSpinBox->blockSignals(false);
  ui->qualityMaxSpinBox->blockSignals(false);
  ui->speedSlider->blockSignals(false);
  ui->ditheringCheckBox->blockSignals(false);
  ui->posterizeSpinBox->blockSignals(false);
  ui->stripMetadataCheckBox->blockSignals(false);
  ui->skipIfLargerCheckBox->blockSignals(false);
  ui->forceCheckBox->blockSignals(false);
}

QVariantMap PngQuantPrefWidget::getCurrentSettings() const {
  QVariantMap settings;

  settings["pngquant/qualityMin"] = ui->qualityMinSpinBox->value();
  settings["pngquant/qualityMax"] = ui->qualityMaxSpinBox->value();
  settings["pngquant/speed"] = ui->speedSlider->value();
  settings["pngquant/enableDithering"] = ui->ditheringCheckBox->isChecked();
  settings["pngquant/posterize"] = ui->posterizeSpinBox->value();
  settings["pngquant/stripMetadata"] = ui->stripMetadataCheckBox->isChecked();
  settings["pngquant/skipIfLarger"] = ui->skipIfLargerCheckBox->isChecked();
  settings["pngquant/force"] = ui->forceCheckBox->isChecked();

  return settings;
}

void PngQuantPrefWidget::setAutoSaveEnabled(bool enabled) {
  m_autoSaveEnabled = enabled;
}

void PngQuantPrefWidget::restoreDefaults() {
  // Block signals
  ui->qualityMinSpinBox->blockSignals(true);
  ui->qualityMaxSpinBox->blockSignals(true);
  ui->speedSlider->blockSignals(true);
  ui->ditheringCheckBox->blockSignals(true);
  ui->posterizeSpinBox->blockSignals(true);
  ui->stripMetadataCheckBox->blockSignals(true);
  ui->skipIfLargerCheckBox->blockSignals(true);
  ui->forceCheckBox->blockSignals(true);

  // Set default values
  ui->qualityMinSpinBox->setValue(65);
  ui->qualityMaxSpinBox->setValue(80);
  ui->speedSlider->setValue(4);
  ui->ditheringCheckBox->setChecked(true);
  ui->posterizeSpinBox->setValue(0);
  ui->stripMetadataCheckBox->setChecked(true);
  ui->skipIfLargerCheckBox->setChecked(true);
  ui->forceCheckBox->setChecked(true);

  // Update labels
  updateQualityRangeLabel();
  ui->speedValueLabel->setText("4 (Default)");

  // Unblock signals
  ui->qualityMinSpinBox->blockSignals(false);
  ui->qualityMaxSpinBox->blockSignals(false);
  ui->speedSlider->blockSignals(false);
  ui->ditheringCheckBox->blockSignals(false);
  ui->posterizeSpinBox->blockSignals(false);
  ui->stripMetadataCheckBox->blockSignals(false);
  ui->skipIfLargerCheckBox->blockSignals(false);
  ui->forceCheckBox->blockSignals(false);

  // Save defaults to QSettings
  saveSettings();
}
