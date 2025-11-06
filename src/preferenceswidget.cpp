#include "preferenceswidget.h"
#include "imageformatprefwidget.h"
#include "thememanager.h"
#include "ui_preferenceswidget.h"

#include <QAction>
#include <QFileDialog>
#include <QIcon>
#include <QStyleFactory>

#include <worker/imageworkerfactory.h>

PreferencesWidget::PreferencesWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PreferencesWidget),
      m_settings(Settings::instance()) {
  ui->setupUi(this);

  // UI
  QAction *changeOutputDirPathAction =
      new QAction(QIcon(":/resources/icons/folder-open-line.png"), "",
                  ui->outputDirPathLineEdit);
  changeOutputDirPathAction->setToolTip(tr("Change Output Directory Path"));
  ui->outputDirPathLineEdit->addAction(changeOutputDirPathAction,
                                       QLineEdit::TrailingPosition);
  connect(changeOutputDirPathAction, &QAction::triggered, this,
          &PreferencesWidget::onChangeOutputDirPath);

  QAction *cleanOptimizedFilePrefixAction =
      new QAction(QIcon(":/resources/icons/brush-2-line.png"), "",
                  ui->optimizedFilePrefixLineEdit);
  cleanOptimizedFilePrefixAction->setToolTip(
      tr("Clear Optimized File Prefix Keyword"));
  ui->optimizedFilePrefixLineEdit->addAction(cleanOptimizedFilePrefixAction,
                                             QLineEdit::TrailingPosition);
  connect(cleanOptimizedFilePrefixAction, &QAction::triggered, this,
          &PreferencesWidget::onCleanOptimizedFilePrefix);

  QAction *resetFilepickerLastOpenedPathAction =
      new QAction(QIcon(":/resources/icons/reset-left-line.png"), "",
                  ui->filepickerLastOpenedPathLineEdit);
  resetFilepickerLastOpenedPathAction->setToolTip(
      tr("Restore Default File Picker Path"));
  ui->filepickerLastOpenedPathLineEdit->addAction(
      resetFilepickerLastOpenedPathAction, QLineEdit::TrailingPosition);
  connect(resetFilepickerLastOpenedPathAction, &QAction::triggered, this,
          &PreferencesWidget::onResetFilepickerLastOpenedPath);

  ui->concurrentTasksSpinBox->setRange(1, 20);
  ui->concurrentTasksSpinBox->setToolTip(
      tr("Number of tasks that can be processed in parallel.<br><br>") +
      QString("Range Min: %1; Max: %2")
          .arg(ui->concurrentTasksSpinBox->minimum())
          .arg(ui->concurrentTasksSpinBox->maximum()));

  ui->filepickerLastOpenedPathLineEdit->setReadOnly(true);
  ui->outputDirPathLineEdit->setReadOnly(true);

  // LOAD SETTINGS AND BIND
  ui->outputDirPathLineEdit->setText(m_settings.getOptimizedPath());
  connect(ui->outputDirPathLineEdit, &QLineEdit::textChanged, this,
          [=](const QString &arg1) { m_settings.setOptimizedPath(arg1); });

  ui->optimizedFilePrefixLineEdit->setText(m_settings.getOutputFilePrefix());
  connect(ui->optimizedFilePrefixLineEdit, &QLineEdit::textChanged, this,
          [=](const QString &arg1) { m_settings.setOutputFilePrefix(arg1); });

  ui->concurrentTasksSpinBox->setValue(m_settings.getMaxConcurrentTasks());
  connect(ui->concurrentTasksSpinBox,
          QOverload<int>::of(&QSpinBox::valueChanged), this,
          [=](int arg1) { m_settings.setMaxConcurrentTasks(arg1); });

  ui->filepickerLastOpenedPathLineEdit->setText(
      m_settings.getLastOpenedImageDirPath());
  connect(
      ui->filepickerLastOpenedPathLineEdit, &QLineEdit::textChanged, this,
      [=](const QString &arg1) { m_settings.setLastOpenedImageDirPath(arg1); });

  ui->filepickerRememberLastPathCheckBox->setChecked(
      m_settings.getRememberOpenLastOpenedPath());
  connect(ui->filepickerRememberLastPathCheckBox, &QCheckBox::toggled, this,
          [=](bool arg1) { m_settings.setRememberOpenLastOpenedPath(arg1); });

  // Theme and Style
  // Load use system theme setting
  bool useSystemTheme = m_settings.getUseSystemTheme();
  ui->useSystemThemeCheckBox->setChecked(useSystemTheme);

  // Populate available styles
  QStringList availableStyles = QStyleFactory::keys();
  ui->styleComboBox->addItems(availableStyles);

  // Load current theme
  QString currentTheme = m_settings.getTheme();
  int themeIndex = ui->themeComboBox->findText(currentTheme);
  if (themeIndex >= 0) {
    ui->themeComboBox->setCurrentIndex(themeIndex);
  }

  // Load current style
  QString currentStyle = m_settings.getStyle();
  int styleIndex = ui->styleComboBox->findText(currentStyle, Qt::MatchFixedString);
  if (styleIndex >= 0) {
    ui->styleComboBox->setCurrentIndex(styleIndex);
  }

  // Enable/disable theme controls based on system theme checkbox
  ui->themeComboBox->setEnabled(!useSystemTheme);
  ui->styleComboBox->setEnabled(!useSystemTheme);
  ui->themeLabel->setEnabled(!useSystemTheme);
  ui->styleLabel->setEnabled(!useSystemTheme);

  // Connect theme and style change signals
  connect(ui->useSystemThemeCheckBox, &QCheckBox::toggled,
          this, &PreferencesWidget::onUseSystemThemeToggled);
  connect(ui->themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &PreferencesWidget::onThemeChanged);
  connect(ui->styleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &PreferencesWidget::onStyleChanged);

  QList<ImageType> supportedImageTypes =
      ImageTypeUtils::getSupportedImageTypes();
  foreach (auto imageType, supportedImageTypes) {
    auto formatName = ImageTypeUtils::imageTypeToString(imageType);
    auto prefWidget = new ImageFormatPrefWidget(
        this, formatName,
        ImageWorkerFactory::instance().getOptimizersForFormat(formatName));
    prefWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->formatPrefVerticalLayout->addWidget(prefWidget);
  }
}

PreferencesWidget::~PreferencesWidget() { delete ui; }

void PreferencesWidget::onChangeOutputDirPath() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Select Output Directory"), m_settings.getOptimizedPath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!dir.isEmpty()) {
    if (!dir.endsWith(QDir::separator())) {
      dir += QDir::separator();
    }
    ui->outputDirPathLineEdit->setText(dir);
  }
}

void PreferencesWidget::onCleanOptimizedFilePrefix() {
  ui->optimizedFilePrefixLineEdit->clear();
}

void PreferencesWidget::onResetFilepickerLastOpenedPath() {
  ui->filepickerLastOpenedPathLineEdit->setText(
      Constants::DEFAULT_INPUT_LAST_IMAGE_DIR_PATH);
}

void PreferencesWidget::onThemeChanged(int index) {
  QString theme = ui->themeComboBox->itemText(index);
  m_settings.setTheme(theme);
  bool useSystemTheme = m_settings.getUseSystemTheme();
  applyTheme(theme, useSystemTheme);
}

void PreferencesWidget::onStyleChanged(int index) {
  QString style = ui->styleComboBox->itemText(index);
  m_settings.setStyle(style);
  bool useSystemTheme = m_settings.getUseSystemTheme();
  applyStyle(style, useSystemTheme);
}

void PreferencesWidget::onUseSystemThemeToggled(bool checked) {
  m_settings.setUseSystemTheme(checked);

  // Enable/disable custom theme controls
  ui->themeComboBox->setEnabled(!checked);
  ui->styleComboBox->setEnabled(!checked);
  ui->themeLabel->setEnabled(!checked);
  ui->styleLabel->setEnabled(!checked);

  // Apply theme immediately
  if (checked) {
    // Use system theme
    ThemeManager::applyTheme("", true);
    ThemeManager::applyStyle("", true);
  } else {
    // Use custom theme
    QString theme = m_settings.getTheme();
    QString style = m_settings.getStyle();
    ThemeManager::applyTheme(theme, false);
    ThemeManager::applyStyle(style, false);
  }
}

void PreferencesWidget::applyTheme(const QString &theme, bool useSystemTheme) {
  ThemeManager::applyTheme(theme, useSystemTheme);
}

void PreferencesWidget::applyStyle(const QString &style, bool useSystemTheme) {
  ThemeManager::applyStyle(style, useSystemTheme);
}

