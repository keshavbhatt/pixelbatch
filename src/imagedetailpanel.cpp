#include "imagedetailpanel.h"

#include "OptimizerPrefWidgets/imageformatprefwidgetfactory.h"
#include "worker/imageworkerfactory.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImageReader>

ImageDetailPanel::ImageDetailPanel(QWidget *parent)
    : QWidget(parent), m_currentTask(nullptr), m_settings(Settings::instance()),
      m_currentOptimizerWidget(nullptr) {
  setupUI();
}

ImageDetailPanel::~ImageDetailPanel() {}

void ImageDetailPanel::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(6, 6, 6, 6);
  mainLayout->setSpacing(8);

  // Image Preview Section
  QGroupBox *previewGroup = new QGroupBox(tr("Image Preview (original)"));
  QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);

  m_previewLabel = new QLabel();
  m_previewLabel->setAlignment(Qt::AlignCenter);
  m_previewLabel->setScaledContents(false);
  m_previewLabel->setText(tr("No image selected"));
  m_previewLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  previewLayout->addWidget(m_previewLabel, 0, Qt::AlignCenter);

  mainLayout->addWidget(previewGroup);

  // Image Information Section
  QGroupBox *infoGroup = new QGroupBox(tr("Image Information"));
  QGridLayout *infoLayout = new QGridLayout(infoGroup);
  infoLayout->setColumnStretch(1, 1);

  m_filenameLabel = new QLabel(tr("No file selected"));
  m_sizeLabel = new QLabel(tr("-"));
  m_dimensionsLabel = new QLabel(tr("-"));
  m_formatLabel = new QLabel(tr("-"));

  m_filenameLabel->setWordWrap(true);
  m_filenameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  infoLayout->addWidget(new QLabel(tr("<b>Filename:</b>")), 0, 0);
  infoLayout->addWidget(m_filenameLabel, 0, 1);

  infoLayout->addWidget(new QLabel(tr("<b>Size:</b>")), 1, 0);
  infoLayout->addWidget(m_sizeLabel, 1, 1);

  infoLayout->addWidget(new QLabel(tr("<b>Dimensions:</b>")), 2, 0);
  infoLayout->addWidget(m_dimensionsLabel, 2, 1);

  infoLayout->addWidget(new QLabel(tr("<b>Format:</b>")), 3, 0);
  infoLayout->addWidget(m_formatLabel, 3, 1);

  mainLayout->addWidget(infoGroup);

  // Output Settings Section
  QGroupBox *outputGroup = new QGroupBox(tr("Output Settings"));
  QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);

  m_useCustomOutputCheckBox = new QCheckBox(tr("Use custom output directory"));
  m_useCustomOutputCheckBox->setToolTip(tr("Override the global output directory setting for this image"));
  outputLayout->addWidget(m_useCustomOutputCheckBox);

  // Output directory
  QHBoxLayout *outputDirLayout = new QHBoxLayout();
  QLabel *outputDirLabel = new QLabel(tr("Output Directory:"));
  m_outputDirLineEdit = new QLineEdit();
  m_outputDirLineEdit->setReadOnly(true);
  m_outputDirLineEdit->setPlaceholderText(tr("Using global setting"));
  m_changeOutputDirButton = new QPushButton(tr("Browse..."));
  m_changeOutputDirButton->setEnabled(false);

  outputDirLayout->addWidget(outputDirLabel);
  outputDirLayout->addWidget(m_outputDirLineEdit, 1);
  outputDirLayout->addWidget(m_changeOutputDirButton);
  outputLayout->addLayout(outputDirLayout);

  // Output prefix
  QHBoxLayout *outputPrefixLayout = new QHBoxLayout();
  QLabel *outputPrefixLabel = new QLabel(tr("Output Prefix:"));
  m_outputPrefixLineEdit = new QLineEdit();
  m_outputPrefixLineEdit->setPlaceholderText(tr("Using global setting"));
  m_outputPrefixLineEdit->setEnabled(false);
  m_outputPrefixLineEdit->setToolTip(tr("Prefix to add to the optimized filename"));

  outputPrefixLayout->addWidget(outputPrefixLabel);
  outputPrefixLayout->addWidget(m_outputPrefixLineEdit, 1);
  outputLayout->addLayout(outputPrefixLayout);

  mainLayout->addWidget(outputGroup);

  // Connect signals
  connect(m_useCustomOutputCheckBox, &QCheckBox::toggled, this,
          &ImageDetailPanel::onUseCustomOutputToggled);
  connect(m_changeOutputDirButton, &QPushButton::clicked, this,
          &ImageDetailPanel::onChangeOutputDir);
  connect(m_outputPrefixLineEdit, &QLineEdit::textChanged, this,
          &ImageDetailPanel::onOutputPrefixChanged);

  // Optimizer Settings Section
  QGroupBox *settingsGroup = new QGroupBox(tr("Optimization Settings"));
  m_optimizerSettingsLayout = new QVBoxLayout(settingsGroup);

  // Add placeholder label initially
  QLabel *noSettingsLabel =
      new QLabel(tr("Select an image to view optimization settings"));
  noSettingsLabel->setAlignment(Qt::AlignCenter);
  noSettingsLabel->setWordWrap(true);
  m_optimizerSettingsLayout->addWidget(noSettingsLabel);


  mainLayout->addWidget(
      settingsGroup, 1); // Give stretch factor of 1 to take all available space

  setLayout(mainLayout);
}

void ImageDetailPanel::setImageTask(ImageTask *task) {
  m_currentTask = task;

  if (!task) {
    clear();
    return;
  }

  updateImagePreview();
  updateImageInfo();
  updateOutputSettings();
  updateOptimizerSettings();
}

void ImageDetailPanel::clear() {
  m_currentTask = nullptr;

  m_previewLabel->clear();
  m_previewLabel->setText(tr("No image selected"));

  m_filenameLabel->setText(tr("No file selected"));
  m_sizeLabel->setText(tr("-"));
  m_dimensionsLabel->setText(tr("-"));
  m_formatLabel->setText(tr("-"));

  // Clear output settings
  m_useCustomOutputCheckBox->setChecked(false);
  m_outputDirLineEdit->clear();
  m_outputPrefixLineEdit->clear();
  m_changeOutputDirButton->setEnabled(false);
  m_outputPrefixLineEdit->setEnabled(false);

  // Clear all widgets from the optimizer settings layout
  QLayoutItem *item;
  while ((item = m_optimizerSettingsLayout->takeAt(0)) != nullptr) {
    if (item->widget()) {
      item->widget()->deleteLater();
    }
    delete item;
  }
  m_currentOptimizerWidget = nullptr;

  // Add placeholder label back
  QLabel *noSettingsLabel =
      new QLabel(tr("Select an image to view optimization settings"));
  noSettingsLabel->setAlignment(Qt::AlignCenter);
  noSettingsLabel->setWordWrap(true);
  m_optimizerSettingsLayout->addWidget(noSettingsLabel);
}

void ImageDetailPanel::updateImagePreview() {
  if (!m_currentTask) {
    return;
  }

  QFileInfo fileInfo(m_currentTask->imagePath);
  if (!fileInfo.exists()) {
    m_previewLabel->setText(tr("Image file not found"));
    return;
  }

  // Load image
  QImageReader reader(m_currentTask->imagePath);
  if (!reader.canRead()) {
    m_previewLabel->setText(tr("Cannot read image"));
    return;
  }

  QImage image = reader.read();
  if (image.isNull()) {
    m_previewLabel->setText(tr("Failed to load image"));
    return;
  }

  // Convert to pixmap and scale only if larger than max preview size
  m_originalPixmap = QPixmap::fromImage(image);

  const int maxPreviewSize = 250;
  QPixmap displayPixmap;

  if (m_originalPixmap.width() > maxPreviewSize ||
      m_originalPixmap.height() > maxPreviewSize) {
    // Scale down large images
    displayPixmap =
        m_originalPixmap.scaled(maxPreviewSize, maxPreviewSize,
                                Qt::KeepAspectRatio, Qt::SmoothTransformation);
  } else {
    // Use original size for small images
    displayPixmap = m_originalPixmap;
  }

  m_previewLabel->setPixmap(displayPixmap);

  // Add note for GIF images (only shows first frame)
  QString extension = fileInfo.suffix().toLower();
  if (extension == "gif") {
    m_previewLabel->setToolTip(
        tr("Note: GIF preview shows only the first frame, not the animation"));
  } else {
    m_previewLabel->setToolTip("");
  }
}

void ImageDetailPanel::updateImageInfo() {
  if (!m_currentTask) {
    return;
  }

  QFileInfo fileInfo(m_currentTask->imagePath);

  // Filename
  m_filenameLabel->setText(fileInfo.fileName());

  // File size
  qint64 fileSize = fileInfo.size();
  QString sizeText;
  if (fileSize < 1024) {
    sizeText = QString("%1 B").arg(fileSize);
  } else if (fileSize < 1024 * 1024) {
    sizeText = QString("%1 KB").arg(fileSize / 1024.0, 0, 'f', 2);
  } else {
    sizeText = QString("%1 MB").arg(fileSize / (1024.0 * 1024.0), 0, 'f', 2);
  }
  m_sizeLabel->setText(sizeText);

  // Dimensions
  QImageReader reader(m_currentTask->imagePath);
  QSize imageSize = reader.size();
  if (imageSize.isValid()) {
    m_dimensionsLabel->setText(
        QString("%1 × %2").arg(imageSize.width()).arg(imageSize.height()));
  } else {
    m_dimensionsLabel->setText(tr("Unknown"));
  }

  // Format
  QString format = QString::fromLatin1(reader.format()).toUpper();
  QString extension = fileInfo.suffix().toLower();

  if (extension == "gif") {
    m_formatLabel->setText(format + tr(" (preview shows first frame only)"));
  } else {
    m_formatLabel->setText(format);
  }
}

void ImageDetailPanel::updateOptimizerSettings() {
  if (!m_currentTask) {
    return;
  }

  // Clear all widgets from the layout
  QLayoutItem *item;
  while ((item = m_optimizerSettingsLayout->takeAt(0)) != nullptr) {
    if (item->widget()) {
      item->widget()->deleteLater();
    }
    delete item;
  }
  m_currentOptimizerWidget = nullptr;

  loadOptimizerWidget();
}

void ImageDetailPanel::loadOptimizerWidget() {
  if (!m_currentTask) {
    return;
  }

  // Determine image type from file extension
  QFileInfo fileInfo(m_currentTask->imagePath);
  QString extension = fileInfo.suffix().toLower();
  ImageType imageType =
      ImageWorkerFactory::instance().getImageTypeByExtension(extension);

  if (imageType == ImageType::Unsupported) {
    QLabel *noOptimizerLabel =
        new QLabel(tr("No optimizer available for this image format"));
    noOptimizerLabel->setAlignment(Qt::AlignCenter);
    noOptimizerLabel->setWordWrap(true);
    m_currentOptimizerWidget = noOptimizerLabel;
    m_optimizerSettingsLayout->addWidget(m_currentOptimizerWidget);
    return;
  }

  // Get the optimizer for this image type
  ImageOptimizer optimizer =
      ImageWorkerFactory::instance().getOptimizerByImageType(imageType);

  // Create the preference widget using the factory (includes scroll area)
  QWidget *prefWidget =
      ImageFormatPrefWidgetFactory::instance().createPrefWidgetFor(optimizer, this);

  if (prefWidget) {

    m_currentOptimizerWidget = prefWidget;
    m_optimizerSettingsLayout->addWidget(m_currentOptimizerWidget);

    // Make the widget enabled for viewing settings
    m_currentOptimizerWidget->setEnabled(true);
  } else {
    QLabel *noOptimizerLabel =
        new QLabel(tr("No optimizer available for this image format"));
    noOptimizerLabel->setAlignment(Qt::AlignCenter);
    noOptimizerLabel->setWordWrap(true);
    m_currentOptimizerWidget = noOptimizerLabel;
    m_optimizerSettingsLayout->addWidget(m_currentOptimizerWidget);
  }
}

void ImageDetailPanel::updateOutputSettings() {
  if (!m_currentTask) {
    return;
  }

  // Block signals to avoid triggering updates while setting values
  m_useCustomOutputCheckBox->blockSignals(true);
  m_outputDirLineEdit->blockSignals(true);
  m_outputPrefixLineEdit->blockSignals(true);

  // Check if task has custom output settings
  bool hasCustomDir = m_currentTask->hasCustomOutputDir();
  bool hasCustomPrefix = m_currentTask->hasCustomOutputPrefix();

  m_useCustomOutputCheckBox->setChecked(hasCustomDir || hasCustomPrefix);

  // Update UI based on custom settings
  if (hasCustomDir) {
    m_outputDirLineEdit->setText(m_currentTask->customOutputDir);
  } else {
    m_outputDirLineEdit->setText(m_settings.getOptimizedPath());
    m_outputDirLineEdit->setPlaceholderText(tr("Using global setting: %1").arg(m_settings.getOptimizedPath()));
  }

  if (hasCustomPrefix) {
    m_outputPrefixLineEdit->setText(m_currentTask->customOutputPrefix);
  } else {
    m_outputPrefixLineEdit->setText(m_settings.getOutputFilePrefix());
    m_outputPrefixLineEdit->setPlaceholderText(tr("Using global setting: %1").arg(m_settings.getOutputFilePrefix()));
  }

  // Enable/disable controls based on checkbox state
  bool customEnabled = m_useCustomOutputCheckBox->isChecked();
  m_changeOutputDirButton->setEnabled(customEnabled);
  m_outputPrefixLineEdit->setEnabled(customEnabled);

  // Unblock signals
  m_useCustomOutputCheckBox->blockSignals(false);
  m_outputDirLineEdit->blockSignals(false);
  m_outputPrefixLineEdit->blockSignals(false);
}

void ImageDetailPanel::onUseCustomOutputToggled(bool checked) {
  if (!m_currentTask) {
    return;
  }

  m_changeOutputDirButton->setEnabled(checked);
  m_outputPrefixLineEdit->setEnabled(checked);

  if (checked) {
    // Enable custom output - set current values as custom if not already set
    if (!m_currentTask->hasCustomOutputDir()) {
      QString dir = m_settings.getOptimizedPath();
      m_currentTask->customOutputDir = dir;
      m_outputDirLineEdit->setText(dir);
      emit customOutputDirChanged(m_currentTask, dir);
    }
    if (!m_currentTask->hasCustomOutputPrefix()) {
      QString prefix = m_settings.getOutputFilePrefix();
      m_currentTask->customOutputPrefix = prefix;
      m_outputPrefixLineEdit->setText(prefix);
      emit customOutputPrefixChanged(m_currentTask, prefix);
    }
  } else {
    // Disable custom output - revert to global settings
    m_currentTask->customOutputDir = QString();
    m_currentTask->customOutputPrefix = QString();
    m_outputDirLineEdit->setText(m_settings.getOptimizedPath());
    m_outputDirLineEdit->setPlaceholderText(tr("Using global setting: %1").arg(m_settings.getOptimizedPath()));
    m_outputPrefixLineEdit->setText(m_settings.getOutputFilePrefix());
    m_outputPrefixLineEdit->setPlaceholderText(tr("Using global setting: %1").arg(m_settings.getOutputFilePrefix()));
    emit customOutputDirCleared(m_currentTask);
    emit customOutputPrefixCleared(m_currentTask);
  }
}

void ImageDetailPanel::onChangeOutputDir() {
  if (!m_currentTask) {
    return;
  }

  QString currentDir = m_currentTask->hasCustomOutputDir()
                       ? m_currentTask->customOutputDir
                       : m_settings.getOptimizedPath();

  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Select Output Directory"), currentDir,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!dir.isEmpty()) {
    // Ensure directory ends with separator
    if (!dir.endsWith(QDir::separator())) {
      dir += QDir::separator();
    }

    m_currentTask->customOutputDir = dir;
    m_outputDirLineEdit->setText(dir);
    emit customOutputDirChanged(m_currentTask, dir);
  }
}

void ImageDetailPanel::onOutputPrefixChanged(const QString &text) {
  if (!m_currentTask) {
    return;
  }

  // Only update if custom output is enabled
  if (m_useCustomOutputCheckBox->isChecked()) {
    m_currentTask->customOutputPrefix = text;
    emit customOutputPrefixChanged(m_currentTask, text);
  }
}

