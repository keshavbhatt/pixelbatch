#include "svgoprefwidget.h"
#include "ui_svgoprefwidget.h"

SvgoPrefWidget::SvgoPrefWidget(QWidget *parent)
    : ImageOptimizerPrefWidget(parent), ui(new Ui::SvgoPrefWidget),
      m_settings(Settings::instance()) {
  ui->setupUi(this);

  // Load settings
  loadSettings();

  // Connect signals for auto-save
  connect(ui->precisionSlider, &QSlider::valueChanged,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->multipassCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->prettyPrintCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->indentSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          this, &SvgoPrefWidget::saveSettings);

  connect(ui->removeCommentsCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->removeMetadataCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->removeTitleCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->removeDescCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->removeEditorsDataCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);

  connect(ui->removeHiddenCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->removeEmptyCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->mergePathsCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->convertShapesCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);

  connect(ui->removeDimensionsCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->cleanupIdsCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);
  connect(ui->inlineStylesCheckBox, &QCheckBox::toggled,
          this, &SvgoPrefWidget::saveSettings);

  // Connect UI update signals
  connect(ui->prettyPrintCheckBox, &QCheckBox::toggled,
          ui->prettyPrintWidget, &QWidget::setEnabled);

  connect(ui->precisionSlider, &QSlider::valueChanged,
          this, &SvgoPrefWidget::updatePrecisionLabel);

  // Initial UI state
  ui->prettyPrintWidget->setEnabled(ui->prettyPrintCheckBox->isChecked());
  updatePrecisionLabel(ui->precisionSlider->value());
}

SvgoPrefWidget::~SvgoPrefWidget() { delete ui; }

void SvgoPrefWidget::loadSettings() {
  const QSettings &settings = m_settings.getSettings();

  // Precision
  ui->precisionSlider->setValue(
      settings.value("svgo/precision", 3).toInt());

  // Optimization options
  ui->multipassCheckBox->setChecked(
      settings.value("svgo/multipass", true).toBool());
  ui->prettyPrintCheckBox->setChecked(
      settings.value("svgo/prettyPrint", false).toBool());
  ui->indentSpinBox->setValue(
      settings.value("svgo/indent", 2).toInt());

  // Cleanup options
  ui->removeCommentsCheckBox->setChecked(
      settings.value("svgo/removeComments", true).toBool());
  ui->removeMetadataCheckBox->setChecked(
      settings.value("svgo/removeMetadata", true).toBool());
  ui->removeTitleCheckBox->setChecked(
      settings.value("svgo/removeTitle", false).toBool());
  ui->removeDescCheckBox->setChecked(
      settings.value("svgo/removeDesc", false).toBool());
  ui->removeEditorsDataCheckBox->setChecked(
      settings.value("svgo/removeEditorsData", true).toBool());

  // Size optimization
  ui->removeHiddenCheckBox->setChecked(
      settings.value("svgo/removeHidden", true).toBool());
  ui->removeEmptyCheckBox->setChecked(
      settings.value("svgo/removeEmpty", true).toBool());
  ui->mergePathsCheckBox->setChecked(
      settings.value("svgo/mergePaths", true).toBool());
  ui->convertShapesCheckBox->setChecked(
      settings.value("svgo/convertShapes", true).toBool());

  // Advanced options
  ui->removeDimensionsCheckBox->setChecked(
      settings.value("svgo/removeDimensions", false).toBool());
  ui->cleanupIdsCheckBox->setChecked(
      settings.value("svgo/cleanupIds", true).toBool());
  ui->inlineStylesCheckBox->setChecked(
      settings.value("svgo/inlineStyles", false).toBool());
}

void SvgoPrefWidget::saveSettings() {
  // Don't save to global settings if auto-save is disabled (used in detail panel)
  if (!m_autoSaveEnabled) {
    return;
  }

  QSettings &settings = const_cast<QSettings&>(m_settings.getSettings());

  // Precision
  settings.setValue("svgo/precision", ui->precisionSlider->value());

  // Optimization options
  settings.setValue("svgo/multipass", ui->multipassCheckBox->isChecked());
  settings.setValue("svgo/prettyPrint", ui->prettyPrintCheckBox->isChecked());
  settings.setValue("svgo/indent", ui->indentSpinBox->value());

  // Cleanup options
  settings.setValue("svgo/removeComments", ui->removeCommentsCheckBox->isChecked());
  settings.setValue("svgo/removeMetadata", ui->removeMetadataCheckBox->isChecked());
  settings.setValue("svgo/removeTitle", ui->removeTitleCheckBox->isChecked());
  settings.setValue("svgo/removeDesc", ui->removeDescCheckBox->isChecked());
  settings.setValue("svgo/removeEditorsData", ui->removeEditorsDataCheckBox->isChecked());

  // Size optimization
  settings.setValue("svgo/removeHidden", ui->removeHiddenCheckBox->isChecked());
  settings.setValue("svgo/removeEmpty", ui->removeEmptyCheckBox->isChecked());
  settings.setValue("svgo/mergePaths", ui->mergePathsCheckBox->isChecked());
  settings.setValue("svgo/convertShapes", ui->convertShapesCheckBox->isChecked());

  // Advanced options
  settings.setValue("svgo/removeDimensions", ui->removeDimensionsCheckBox->isChecked());
  settings.setValue("svgo/cleanupIds", ui->cleanupIdsCheckBox->isChecked());
  settings.setValue("svgo/inlineStyles", ui->inlineStylesCheckBox->isChecked());

  settings.sync();
}

void SvgoPrefWidget::updatePrecisionLabel(int value) {
  QString label = QString("%1 digit%2").arg(value).arg(value == 1 ? "" : "s");

  if (value <= 2) {
    label += " (Maximum compression)";
  } else if (value <= 4) {
    label += " (Recommended)";
  } else {
    label += " (High precision)";
  }

  ui->precisionValueLabel->setText(label);
}

void SvgoPrefWidget::loadCustomSettings(const QVariantMap &settings) {
  // Block signals to prevent auto-save while loading
  ui->precisionSlider->blockSignals(true);
  ui->multipassCheckBox->blockSignals(true);
  ui->prettyPrintCheckBox->blockSignals(true);
  ui->indentSpinBox->blockSignals(true);
  ui->removeCommentsCheckBox->blockSignals(true);
  ui->removeMetadataCheckBox->blockSignals(true);
  ui->removeTitleCheckBox->blockSignals(true);
  ui->removeDescCheckBox->blockSignals(true);
  ui->removeEditorsDataCheckBox->blockSignals(true);
  ui->removeHiddenCheckBox->blockSignals(true);
  ui->removeEmptyCheckBox->blockSignals(true);
  ui->mergePathsCheckBox->blockSignals(true);
  ui->convertShapesCheckBox->blockSignals(true);
  ui->removeDimensionsCheckBox->blockSignals(true);
  ui->cleanupIdsCheckBox->blockSignals(true);
  ui->inlineStylesCheckBox->blockSignals(true);

  // Load from custom settings map
  ui->precisionSlider->setValue(settings.value("svgo/precision", 3).toInt());
  ui->multipassCheckBox->setChecked(settings.value("svgo/multipass", true).toBool());
  ui->prettyPrintCheckBox->setChecked(settings.value("svgo/prettyPrint", false).toBool());
  ui->indentSpinBox->setValue(settings.value("svgo/indent", 2).toInt());
  ui->removeCommentsCheckBox->setChecked(settings.value("svgo/removeComments", true).toBool());
  ui->removeMetadataCheckBox->setChecked(settings.value("svgo/removeMetadata", true).toBool());
  ui->removeTitleCheckBox->setChecked(settings.value("svgo/removeTitle", false).toBool());
  ui->removeDescCheckBox->setChecked(settings.value("svgo/removeDesc", false).toBool());
  ui->removeEditorsDataCheckBox->setChecked(settings.value("svgo/removeEditorsData", true).toBool());
  ui->removeHiddenCheckBox->setChecked(settings.value("svgo/removeHidden", true).toBool());
  ui->removeEmptyCheckBox->setChecked(settings.value("svgo/removeEmpty", true).toBool());
  ui->mergePathsCheckBox->setChecked(settings.value("svgo/mergePaths", true).toBool());
  ui->convertShapesCheckBox->setChecked(settings.value("svgo/convertShapes", true).toBool());
  ui->removeDimensionsCheckBox->setChecked(settings.value("svgo/removeDimensions", false).toBool());
  ui->cleanupIdsCheckBox->setChecked(settings.value("svgo/cleanupIds", true).toBool());
  ui->inlineStylesCheckBox->setChecked(settings.value("svgo/inlineStyles", false).toBool());

  // Update precision label
  updatePrecisionLabel(ui->precisionSlider->value());

  // Unblock signals
  ui->precisionSlider->blockSignals(false);
  ui->multipassCheckBox->blockSignals(false);
  ui->prettyPrintCheckBox->blockSignals(false);
  ui->indentSpinBox->blockSignals(false);
  ui->removeCommentsCheckBox->blockSignals(false);
  ui->removeMetadataCheckBox->blockSignals(false);
  ui->removeTitleCheckBox->blockSignals(false);
  ui->removeDescCheckBox->blockSignals(false);
  ui->removeEditorsDataCheckBox->blockSignals(false);
  ui->removeHiddenCheckBox->blockSignals(false);
  ui->removeEmptyCheckBox->blockSignals(false);
  ui->mergePathsCheckBox->blockSignals(false);
  ui->convertShapesCheckBox->blockSignals(false);
  ui->removeDimensionsCheckBox->blockSignals(false);
  ui->cleanupIdsCheckBox->blockSignals(false);
  ui->inlineStylesCheckBox->blockSignals(false);
}

QVariantMap SvgoPrefWidget::getCurrentSettings() const {
  QVariantMap settings;

  settings["svgo/precision"] = ui->precisionSlider->value();
  settings["svgo/multipass"] = ui->multipassCheckBox->isChecked();
  settings["svgo/prettyPrint"] = ui->prettyPrintCheckBox->isChecked();
  settings["svgo/indent"] = ui->indentSpinBox->value();
  settings["svgo/removeComments"] = ui->removeCommentsCheckBox->isChecked();
  settings["svgo/removeMetadata"] = ui->removeMetadataCheckBox->isChecked();
  settings["svgo/removeTitle"] = ui->removeTitleCheckBox->isChecked();
  settings["svgo/removeDesc"] = ui->removeDescCheckBox->isChecked();
  settings["svgo/removeEditorsData"] = ui->removeEditorsDataCheckBox->isChecked();
  settings["svgo/removeHidden"] = ui->removeHiddenCheckBox->isChecked();
  settings["svgo/removeEmpty"] = ui->removeEmptyCheckBox->isChecked();
  settings["svgo/mergePaths"] = ui->mergePathsCheckBox->isChecked();
  settings["svgo/convertShapes"] = ui->convertShapesCheckBox->isChecked();
  settings["svgo/removeDimensions"] = ui->removeDimensionsCheckBox->isChecked();
  settings["svgo/cleanupIds"] = ui->cleanupIdsCheckBox->isChecked();
  settings["svgo/inlineStyles"] = ui->inlineStylesCheckBox->isChecked();

  return settings;
}

void SvgoPrefWidget::setAutoSaveEnabled(bool enabled) {
  m_autoSaveEnabled = enabled;
}
