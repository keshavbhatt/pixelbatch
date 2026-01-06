#include "gifsicleprefwidget.h"
#include "ui_gifsicleprefwidget.h"

GifsiclePrefWidget::GifsiclePrefWidget(QWidget *parent)
    : ImageOptimizerPrefWidget(parent), ui(new Ui::GifsiclePrefWidget),
      m_settings(Settings::instance()) {
  ui->setupUi(this);

  // Load settings
  loadSettings();

  // Connect signals for auto-save
  connect(ui->level1Radio, &QRadioButton::toggled,
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->level2Radio, &QRadioButton::toggled,
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->level3Radio, &QRadioButton::toggled,
          this, &GifsiclePrefWidget::saveSettings);

  connect(ui->losslessRadio, &QRadioButton::toggled,
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->lossyRadio, &QRadioButton::toggled,
          this, &GifsiclePrefWidget::saveSettings);

  connect(ui->lossyLevelSlider, &QSlider::valueChanged,
          this, &GifsiclePrefWidget::saveSettings);

  connect(ui->reduceColorsCheckBox, &QCheckBox::toggled,
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->colorCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->colorMethodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &GifsiclePrefWidget::saveSettings);

  connect(ui->ditheringCheckBox, &QCheckBox::toggled,
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->cropTransparencyCheckBox, &QCheckBox::toggled,
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->interlaceCheckBox, &QCheckBox::toggled,
          this, &GifsiclePrefWidget::saveSettings);
  connect(ui->threadsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &GifsiclePrefWidget::saveSettings);

  // Connect UI update signals
  connect(ui->losslessRadio, &QRadioButton::toggled, this, [this](bool checked) {
    updateLossyControls(checked ? 0 : 1);
  });
  connect(ui->lossyRadio, &QRadioButton::toggled, this, [this](bool checked) {
    updateLossyControls(checked ? 1 : 0);
  });

  connect(ui->reduceColorsCheckBox, &QCheckBox::toggled,
          this, &GifsiclePrefWidget::updateColorReductionControls);

  connect(ui->lossyLevelSlider, &QSlider::valueChanged,
          this, &GifsiclePrefWidget::updateLossyLevelLabel);

  // Initial UI state
  updateLossyControls(ui->losslessRadio->isChecked() ? 0 : 1);
  updateColorReductionControls(ui->reduceColorsCheckBox->isChecked());
  updateLossyLevelLabel(ui->lossyLevelSlider->value());
}

GifsiclePrefWidget::~GifsiclePrefWidget() { delete ui; }

void GifsiclePrefWidget::loadSettings() {
  const QSettings &settings = m_settings.getSettings();

  // Optimization level
  int level = settings.value("gifsicle/optimizationLevel", 2).toInt();
  if (level == 1) {
    ui->level1Radio->setChecked(true);
  } else if (level == 3) {
    ui->level3Radio->setChecked(true);
  } else {
    ui->level2Radio->setChecked(true);
  }

  // Compression type
  int compressionType = settings.value("gifsicle/compressionType", 0).toInt();
  if (compressionType == 1) {
    ui->lossyRadio->setChecked(true);
  } else {
    ui->losslessRadio->setChecked(true);
  }

  // Lossy level
  ui->lossyLevelSlider->setValue(
      settings.value("gifsicle/lossyLevel", 80).toInt());

  // Color reduction
  ui->reduceColorsCheckBox->setChecked(
      settings.value("gifsicle/reduceColors", false).toBool());
  ui->colorCountSpinBox->setValue(
      settings.value("gifsicle/colorCount", 256).toInt());
  ui->colorMethodComboBox->setCurrentIndex(
      settings.value("gifsicle/colorMethod", 0).toInt());

  // Dithering
  ui->ditheringCheckBox->setChecked(
      settings.value("gifsicle/enableDithering", true).toBool());

  // Additional options
  ui->cropTransparencyCheckBox->setChecked(
      settings.value("gifsicle/cropTransparency", true).toBool());
  ui->interlaceCheckBox->setChecked(
      settings.value("gifsicle/interlace", false).toBool());

  // Performance
  ui->threadsSpinBox->setValue(
      settings.value("gifsicle/threads", 4).toInt());
}

void GifsiclePrefWidget::saveSettings() {
  // Don't save to global settings if auto-save is disabled (used in detail panel)
  if (!m_autoSaveEnabled) {
    return;
  }

  QSettings &settings = const_cast<QSettings&>(m_settings.getSettings());

  // Optimization level
  int level = 2;
  if (ui->level1Radio->isChecked()) {
    level = 1;
  } else if (ui->level3Radio->isChecked()) {
    level = 3;
  }
  settings.setValue("gifsicle/optimizationLevel", level);

  // Compression type
  int compressionType = ui->lossyRadio->isChecked() ? 1 : 0;
  settings.setValue("gifsicle/compressionType", compressionType);

  // Lossy level
  settings.setValue("gifsicle/lossyLevel", ui->lossyLevelSlider->value());

  // Color reduction
  settings.setValue("gifsicle/reduceColors", ui->reduceColorsCheckBox->isChecked());
  settings.setValue("gifsicle/colorCount", ui->colorCountSpinBox->value());
  settings.setValue("gifsicle/colorMethod", ui->colorMethodComboBox->currentIndex());

  // Dithering
  settings.setValue("gifsicle/enableDithering", ui->ditheringCheckBox->isChecked());

  // Additional options
  settings.setValue("gifsicle/cropTransparency", ui->cropTransparencyCheckBox->isChecked());
  settings.setValue("gifsicle/interlace", ui->interlaceCheckBox->isChecked());

  // Performance
  settings.setValue("gifsicle/threads", ui->threadsSpinBox->value());

  settings.sync();
}

void GifsiclePrefWidget::updateLossyControls(int compressionType) {
  // compressionType: 0 = lossless, 1 = lossy
  ui->lossyControlsWidget->setEnabled(compressionType == 1);
}

void GifsiclePrefWidget::updateColorReductionControls(bool enabled) {
  ui->colorControlsWidget->setEnabled(enabled);
}

void GifsiclePrefWidget::updateLossyLevelLabel(int value) {
  QString label;
  if (value <= 40) {
    label = QString("%1 (Low compression)").arg(value);
  } else if (value <= 80) {
    label = QString("%1 (Moderate)").arg(value);
  } else if (value <= 120) {
    label = QString("%1 (High)").arg(value);
  } else {
    label = QString("%1 (Very high)").arg(value);
  }
  ui->lossyLevelValueLabel->setText(label);
}

void GifsiclePrefWidget::loadCustomSettings(const QVariantMap &settings) {
  // Block signals to prevent auto-save while loading
  ui->level1Radio->blockSignals(true);
  ui->level2Radio->blockSignals(true);
  ui->level3Radio->blockSignals(true);
  ui->losslessRadio->blockSignals(true);
  ui->lossyRadio->blockSignals(true);
  ui->lossyLevelSlider->blockSignals(true);
  ui->reduceColorsCheckBox->blockSignals(true);
  ui->colorCountSpinBox->blockSignals(true);
  ui->colorMethodComboBox->blockSignals(true);
  ui->ditheringCheckBox->blockSignals(true);
  ui->cropTransparencyCheckBox->blockSignals(true);
  ui->interlaceCheckBox->blockSignals(true);
  ui->threadsSpinBox->blockSignals(true);

  // Load optimization level
  int level = settings.value("gifsicle/optimizationLevel", 2).toInt();
  if (level == 1) ui->level1Radio->setChecked(true);
  else if (level == 3) ui->level3Radio->setChecked(true);
  else ui->level2Radio->setChecked(true);

  // Load compression type
  int compressionType = settings.value("gifsicle/compressionType", 0).toInt();
  if (compressionType == 1) ui->lossyRadio->setChecked(true);
  else ui->losslessRadio->setChecked(true);

  // Load lossy level
  ui->lossyLevelSlider->setValue(settings.value("gifsicle/lossyLevel", 80).toInt());

  // Load color reduction
  ui->reduceColorsCheckBox->setChecked(settings.value("gifsicle/reduceColors", false).toBool());
  ui->colorCountSpinBox->setValue(settings.value("gifsicle/colorCount", 256).toInt());
  ui->colorMethodComboBox->setCurrentIndex(settings.value("gifsicle/colorMethod", 0).toInt());

  // Load dithering
  ui->ditheringCheckBox->setChecked(settings.value("gifsicle/enableDithering", true).toBool());

  // Load additional options
  ui->cropTransparencyCheckBox->setChecked(settings.value("gifsicle/cropTransparency", true).toBool());
  ui->interlaceCheckBox->setChecked(settings.value("gifsicle/interlace", false).toBool());

  // Load performance
  ui->threadsSpinBox->setValue(settings.value("gifsicle/threads", 4).toInt());

  // Update controls state
  updateLossyControls(compressionType);
  updateColorReductionControls(ui->reduceColorsCheckBox->isChecked());
  updateLossyLevelLabel(ui->lossyLevelSlider->value());

  // Unblock signals
  ui->level1Radio->blockSignals(false);
  ui->level2Radio->blockSignals(false);
  ui->level3Radio->blockSignals(false);
  ui->losslessRadio->blockSignals(false);
  ui->lossyRadio->blockSignals(false);
  ui->lossyLevelSlider->blockSignals(false);
  ui->reduceColorsCheckBox->blockSignals(false);
  ui->colorCountSpinBox->blockSignals(false);
  ui->colorMethodComboBox->blockSignals(false);
  ui->ditheringCheckBox->blockSignals(false);
  ui->cropTransparencyCheckBox->blockSignals(false);
  ui->interlaceCheckBox->blockSignals(false);
  ui->threadsSpinBox->blockSignals(false);
}

QVariantMap GifsiclePrefWidget::getCurrentSettings() const {
  QVariantMap settings;

  // Optimization level
  int level = 2;
  if (ui->level1Radio->isChecked()) level = 1;
  else if (ui->level3Radio->isChecked()) level = 3;
  settings["gifsicle/optimizationLevel"] = level;

  // Compression type
  settings["gifsicle/compressionType"] = ui->lossyRadio->isChecked() ? 1 : 0;

  // Lossy level
  settings["gifsicle/lossyLevel"] = ui->lossyLevelSlider->value();

  // Color reduction
  settings["gifsicle/reduceColors"] = ui->reduceColorsCheckBox->isChecked();
  settings["gifsicle/colorCount"] = ui->colorCountSpinBox->value();
  settings["gifsicle/colorMethod"] = ui->colorMethodComboBox->currentIndex();

  // Dithering
  settings["gifsicle/enableDithering"] = ui->ditheringCheckBox->isChecked();

  // Additional options
  settings["gifsicle/cropTransparency"] = ui->cropTransparencyCheckBox->isChecked();
  settings["gifsicle/interlace"] = ui->interlaceCheckBox->isChecked();

  // Performance
  settings["gifsicle/threads"] = ui->threadsSpinBox->value();

  return settings;
}

void GifsiclePrefWidget::setAutoSaveEnabled(bool enabled) {
  m_autoSaveEnabled = enabled;
}
