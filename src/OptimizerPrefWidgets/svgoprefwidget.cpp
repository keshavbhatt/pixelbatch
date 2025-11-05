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

