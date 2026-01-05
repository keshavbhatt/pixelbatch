#include "jpegoptimprefwidget.h"
#include "ui_jpegoptimprefwidget.h"

JpegOptimPrefWidget::JpegOptimPrefWidget(QWidget *parent)
    : ImageOptimizerPrefWidget(parent), ui(new Ui::JpegOptimPrefWidget),
      m_settings(Settings::instance()) {
  ui->setupUi(this);

  // Load settings
  loadSettings();

  // Connect signals for auto-save
  connect(ui->maxQualitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &JpegOptimPrefWidget::saveSettings);
  connect(ui->targetSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &JpegOptimPrefWidget::saveSettings);
  connect(ui->thresholdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &JpegOptimPrefWidget::saveSettings);
  connect(ui->metadataComboBox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &JpegOptimPrefWidget::saveSettings);
  connect(ui->autoModeRadio, &QRadioButton::toggled, this,
          &JpegOptimPrefWidget::saveSettings);
  connect(ui->progressiveRadio, &QRadioButton::toggled, this,
          &JpegOptimPrefWidget::saveSettings);
  connect(ui->normalRadio, &QRadioButton::toggled, this,
          &JpegOptimPrefWidget::saveSettings);
  connect(ui->forceCheckBox, &QCheckBox::toggled, this,
          &JpegOptimPrefWidget::saveSettings);
  connect(ui->preserveTimesCheckBox, &QCheckBox::toggled, this,
          &JpegOptimPrefWidget::saveSettings);
  connect(ui->retryCheckBox, &QCheckBox::toggled, this,
          &JpegOptimPrefWidget::saveSettings);
  connect(ui->workersSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &JpegOptimPrefWidget::saveSettings);

  // Update quality label when value changes
  connect(ui->maxQualitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &JpegOptimPrefWidget::updateQualityLabel);

  // Initial label update
  updateQualityLabel(ui->maxQualitySpinBox->value());
}

JpegOptimPrefWidget::~JpegOptimPrefWidget() { delete ui; }

void JpegOptimPrefWidget::loadSettings() {
  const QSettings &settings = m_settings.getSettings();

  // Quality settings
  ui->maxQualitySpinBox->setValue(
      settings.value("jpegoptim/maxQuality", 100).toInt());
  ui->targetSizeSpinBox->setValue(
      settings.value("jpegoptim/targetSize", 0).toInt());
  ui->thresholdSpinBox->setValue(
      settings.value("jpegoptim/compressionThreshold", 0).toInt());

  // Metadata handling
  ui->metadataComboBox->setCurrentIndex(
      settings.value("jpegoptim/metadataMode", 0).toInt());

  // Output format
  int outputMode = settings.value("jpegoptim/outputMode", 0).toInt();
  if (outputMode == 0) {
    ui->autoModeRadio->setChecked(true);
  } else if (outputMode == 1) {
    ui->progressiveRadio->setChecked(true);
  } else {
    ui->normalRadio->setChecked(true);
  }

  // Additional options
  ui->forceCheckBox->setChecked(
      settings.value("jpegoptim/force", false).toBool());
  ui->preserveTimesCheckBox->setChecked(
      settings.value("jpegoptim/preserveTimes", true).toBool());
  ui->retryCheckBox->setChecked(
      settings.value("jpegoptim/retry", false).toBool());

  // Performance
  ui->workersSpinBox->setValue(
      settings.value("jpegoptim/maxWorkers", 1).toInt());
}

void JpegOptimPrefWidget::saveSettings() {
  // Don't save to global settings if auto-save is disabled (used in detail panel)
  if (!m_autoSaveEnabled) {
    return;
  }

  QSettings &settings = const_cast<QSettings&>(m_settings.getSettings());

  // Quality settings
  settings.setValue("jpegoptim/maxQuality", ui->maxQualitySpinBox->value());
  settings.setValue("jpegoptim/targetSize", ui->targetSizeSpinBox->value());
  settings.setValue("jpegoptim/compressionThreshold",
                    ui->thresholdSpinBox->value());

  // Metadata handling
  settings.setValue("jpegoptim/metadataMode",
                    ui->metadataComboBox->currentIndex());

  // Output format
  int outputMode = 0;
  if (ui->progressiveRadio->isChecked()) {
    outputMode = 1;
  } else if (ui->normalRadio->isChecked()) {
    outputMode = 2;
  }
  settings.setValue("jpegoptim/outputMode", outputMode);

  // Additional options
  settings.setValue("jpegoptim/force", ui->forceCheckBox->isChecked());
  settings.setValue("jpegoptim/preserveTimes",
                    ui->preserveTimesCheckBox->isChecked());
  settings.setValue("jpegoptim/retry", ui->retryCheckBox->isChecked());

  // Performance
  settings.setValue("jpegoptim/maxWorkers", ui->workersSpinBox->value());

  settings.sync();
}

void JpegOptimPrefWidget::updateQualityLabel(int quality) {
  if (quality == 100) {
    ui->maxQualityValueLabel->setText("(Lossless)");
  } else if (quality >= 90) {
    ui->maxQualityValueLabel->setText("(High Quality)");
  } else if (quality >= 75) {
    ui->maxQualityValueLabel->setText("(Good Quality)");
  } else if (quality >= 50) {
    ui->maxQualityValueLabel->setText("(Medium Quality)");
  } else {
    ui->maxQualityValueLabel->setText("(Low Quality)");
  }
}

void JpegOptimPrefWidget::loadCustomSettings(const QVariantMap &settings) {
  // Block signals to prevent auto-save while loading
  ui->maxQualitySpinBox->blockSignals(true);
  ui->targetSizeSpinBox->blockSignals(true);
  ui->thresholdSpinBox->blockSignals(true);
  ui->metadataComboBox->blockSignals(true);
  ui->autoModeRadio->blockSignals(true);
  ui->progressiveRadio->blockSignals(true);
  ui->normalRadio->blockSignals(true);
  ui->forceCheckBox->blockSignals(true);
  ui->preserveTimesCheckBox->blockSignals(true);
  ui->retryCheckBox->blockSignals(true);
  ui->workersSpinBox->blockSignals(true);

  // Load from custom settings map
  ui->maxQualitySpinBox->setValue(settings.value("jpegoptim/maxQuality", 100).toInt());
  ui->targetSizeSpinBox->setValue(settings.value("jpegoptim/targetSize", 0).toInt());
  ui->thresholdSpinBox->setValue(settings.value("jpegoptim/compressionThreshold", 0).toInt());
  ui->metadataComboBox->setCurrentIndex(settings.value("jpegoptim/metadataMode", 0).toInt());

  int outputMode = settings.value("jpegoptim/outputMode", 0).toInt();
  if (outputMode == 0) ui->autoModeRadio->setChecked(true);
  else if (outputMode == 1) ui->progressiveRadio->setChecked(true);
  else ui->normalRadio->setChecked(true);

  ui->forceCheckBox->setChecked(settings.value("jpegoptim/force", false).toBool());
  ui->preserveTimesCheckBox->setChecked(settings.value("jpegoptim/preserveTimes", true).toBool());
  ui->retryCheckBox->setChecked(settings.value("jpegoptim/retry", false).toBool());
  ui->workersSpinBox->setValue(settings.value("jpegoptim/maxWorkers", 1).toInt());

  // Update quality label
  updateQualityLabel(ui->maxQualitySpinBox->value());

  // Unblock signals
  ui->maxQualitySpinBox->blockSignals(false);
  ui->targetSizeSpinBox->blockSignals(false);
  ui->thresholdSpinBox->blockSignals(false);
  ui->metadataComboBox->blockSignals(false);
  ui->autoModeRadio->blockSignals(false);
  ui->progressiveRadio->blockSignals(false);
  ui->normalRadio->blockSignals(false);
  ui->forceCheckBox->blockSignals(false);
  ui->preserveTimesCheckBox->blockSignals(false);
  ui->retryCheckBox->blockSignals(false);
  ui->workersSpinBox->blockSignals(false);
}

QVariantMap JpegOptimPrefWidget::getCurrentSettings() const {
  QVariantMap settings;

  settings["jpegoptim/maxQuality"] = ui->maxQualitySpinBox->value();
  settings["jpegoptim/targetSize"] = ui->targetSizeSpinBox->value();
  settings["jpegoptim/compressionThreshold"] = ui->thresholdSpinBox->value();
  settings["jpegoptim/metadataMode"] = ui->metadataComboBox->currentIndex();

  int outputMode = 0;
  if (ui->progressiveRadio->isChecked()) outputMode = 1;
  else if (ui->normalRadio->isChecked()) outputMode = 2;
  settings["jpegoptim/outputMode"] = outputMode;

  settings["jpegoptim/force"] = ui->forceCheckBox->isChecked();
  settings["jpegoptim/preserveTimes"] = ui->preserveTimesCheckBox->isChecked();
  settings["jpegoptim/retry"] = ui->retryCheckBox->isChecked();
  settings["jpegoptim/maxWorkers"] = ui->workersSpinBox->value();

  return settings;
}

void JpegOptimPrefWidget::setAutoSaveEnabled(bool enabled) {
  m_autoSaveEnabled = enabled;
}
