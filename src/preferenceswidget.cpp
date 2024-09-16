#include "preferenceswidget.h"
#include "imageformatprefwidget.h"
#include "ui_preferenceswidget.h"

#include <QAction>
#include <QIcon>

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
  // TODO add trigger binding

  QAction *cleanOptimizedFilePrefixAction =
      new QAction(QIcon(":/resources/icons/brush-2-line.png"), "",
                  ui->optimizedFilePrefixLineEdit);
  cleanOptimizedFilePrefixAction->setToolTip(
      tr("Clear Optimized File Prefix Keyword"));
  ui->optimizedFilePrefixLineEdit->addAction(cleanOptimizedFilePrefixAction,
                                             QLineEdit::TrailingPosition);
  // TODO add trigger binding

  QAction *resetFilepickerLastOpenedPathAction =
      new QAction(QIcon(":/resources/icons/reset-left-line.png"), "",
                  ui->filepickerLastOpenedPathLineEdit);
  resetFilepickerLastOpenedPathAction->setToolTip(
      tr("Restore Default File Picker Path"));
  ui->filepickerLastOpenedPathLineEdit->addAction(
      resetFilepickerLastOpenedPathAction, QLineEdit::TrailingPosition);
  // TODO add trigger binding

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
  // todo BIND

  ui->optimizedFilePrefixLineEdit->setText(m_settings.getOutputFilePrefix());
  connect(ui->optimizedFilePrefixLineEdit, &QLineEdit::textChanged, this,
          [=](const QString &arg1) { m_settings.setOutputFilePrefix(arg1); });

  ui->concurrentTasksSpinBox->setValue(m_settings.getMaxConcurrentTasks());
  connect(ui->concurrentTasksSpinBox,
          QOverload<int>::of(&QSpinBox::valueChanged), this,
          [=](int arg1) { m_settings.setMaxConcurrentTasks(arg1); });

  ui->filepickerLastOpenedPathLineEdit->setText(
      m_settings.getLastOpenedImageDirPath());
  // todo BIND

  ui->filepickerRememberLastPathCheckBox->setChecked(
      m_settings.getRememberOpenLastOpenedPath());
  connect(ui->filepickerRememberLastPathCheckBox, &QCheckBox::toggled, this,
          [=](bool arg1) { m_settings.setRememberOpenLastOpenedPath(arg1); });

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
