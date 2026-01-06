#include "imageformatprefwidget.h"
#include "OptimizerPrefWidgets/imageformatprefwidgetfactory.h"
#include "OptimizerPrefWidgets/imageoptimizerprefwidget.h"
#include "ui_imageformatprefwidget.h"

#include <QDebug>
#include <QMessageBox>

ImageFormatPrefWidget::ImageFormatPrefWidget(QWidget *parent,
                                             QString formatName,
                                             QList<ImageOptimizer> optimizers)
    : QWidget(parent), ui(new Ui::ImageFormatPrefWidget),
      m_formatName(formatName),
      m_optimizer(optimizers.isEmpty() ? ImageOptimizer() : optimizers.at(0)) {
  ui->setupUi(this);

  ui->formatSettingsPushButton->setSizePolicy(QSizePolicy::Maximum,
                                              QSizePolicy::Maximum);
  ui->formatSettingsPushButton->setText("");
  ui->formatSettingsPushButton->setIcon(
      QIcon(":/resources/icons/settings-2-line.png"));

  ui->restoreDefaultsButton->setSizePolicy(QSizePolicy::Maximum,
                                           QSizePolicy::Maximum);
  ui->restoreDefaultsButton->setText("");
  ui->restoreDefaultsButton->setIcon(
      QIcon(":/resources/icons/restart-line.png"));

  // Set format name and optimizer label
  ui->formatName->setText(formatName);
  ui->optimizerLabel->setText(m_optimizer.isValid() ? m_optimizer.getName() : tr("None"));

  connect(ui->formatSettingsPushButton, &QPushButton::clicked, this, [=]() {
    if (m_optimizer.isValid()) {
      ImageFormatPrefWidgetFactory::instance().openPrefWidgetFor(m_optimizer);
    } else {
      qWarning() << "No valid optimizer found, cannot open Pref dialog.";
    }
  });

  connect(ui->restoreDefaultsButton, &QPushButton::clicked, this, [=]() {
    if (m_optimizer.isValid()) {
      QString optimizerName = m_optimizer.getName();

      QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Restore Defaults"),
        tr("Are you sure you want to restore all %1 settings to their default values?\n\nThis will overwrite your current settings.").arg(optimizerName),
        QMessageBox::Yes | QMessageBox::No
      );

      if (reply == QMessageBox::Yes) {
        // Create a temporary widget to call restoreDefaults
        ImageOptimizerPrefWidget *widget =
          ImageFormatPrefWidgetFactory::instance().createOptimizerWidget(m_optimizer);
        if (widget) {
          widget->restoreDefaults();
          delete widget;

          QMessageBox::information(
            this,
            tr("Defaults Restored"),
            tr("%1 settings have been restored to defaults.").arg(optimizerName)
          );
        }
      }
    } else {
      qWarning() << "No valid optimizer found, cannot restore defaults.";
    }
  });

  bool hasWidget = m_optimizer.isValid() &&
                   ImageFormatPrefWidgetFactory::instance().hasPrefWidgetFor(m_optimizer);

  ui->formatSettingsPushButton->setEnabled(hasWidget);
  ui->restoreDefaultsButton->setEnabled(hasWidget);

  updateTooltips();
}

ImageOptimizer ImageFormatPrefWidget::getOptimizer() const {
  return m_optimizer;
}

ImageFormatPrefWidget::~ImageFormatPrefWidget() { delete ui; }

QString ImageFormatPrefWidget::getFormatName() const { return m_formatName; }

void ImageFormatPrefWidget::updateTooltips() {
  if (m_optimizer.isValid()) {
    ui->formatSettingsPushButton->setToolTip(
        m_optimizer.getName() + " | " + tr("Preferences"));
    ui->restoreDefaultsButton->setToolTip(
        tr("Restore default settings for %1").arg(m_optimizer.getName()));
  }
}
