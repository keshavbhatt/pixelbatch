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
      m_formatName(formatName), m_formatOptimizers(optimizers) {
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

  foreach (auto optimizer, formatOptimizers()) {
    ui->optimizersComboBox->addItem(optimizer.getName());
  }
  connect(ui->optimizersComboBox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [=](int index) { this->optimizerChanged(index); });

  connect(ui->formatSettingsPushButton, &QPushButton::clicked, this, [=]() {
    auto optimizer = getOptimizerByName(ui->optimizersComboBox->currentText());
    if (optimizer.isValid()) {
      ImageFormatPrefWidgetFactory::instance().openPrefWidgetFor(optimizer);
    } else {
      qWarning() << "No Valid Optimizer found, cannot open Pref dialog.";
    }
  });

  connect(ui->restoreDefaultsButton, &QPushButton::clicked, this, [=]() {
    auto optimizer = getOptimizerByName(ui->optimizersComboBox->currentText());
    if (optimizer.isValid()) {
      QString optimizerName = optimizer.getName();

      QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Restore Defaults"),
        tr("Are you sure you want to restore all %1 settings to their default values?\n\nThis will overwrite your current settings.").arg(optimizerName),
        QMessageBox::Yes | QMessageBox::No
      );

      if (reply == QMessageBox::Yes) {
        // Create a temporary widget to call restoreDefaults
        ImageOptimizerPrefWidget *widget =
          ImageFormatPrefWidgetFactory::instance().createOptimizerWidget(optimizer);
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
      qWarning() << "No Valid Optimizer found, cannot restore defaults.";
    }
  });

  bool hasWidget = !formatOptimizers().isEmpty() &&
                   formatOptimizers().at(0).isValid() &&
                   ImageFormatPrefWidgetFactory::instance().hasPrefWidgetFor(
                       formatOptimizers().at(0));

  ui->formatSettingsPushButton->setEnabled(hasWidget);
  ui->restoreDefaultsButton->setEnabled(hasWidget);
  ui->formatName->setText(formatName);

  updateFotmatSettingPBTooltip();
}

ImageOptimizer ImageFormatPrefWidget::getOptimizerByName(const QString &name) {
  foreach (auto optimizer, m_formatOptimizers) {
    if (name.toLower() == optimizer.getName().toLower()) {
      return optimizer;
    }
  }
  return ImageOptimizer();
}

ImageFormatPrefWidget::~ImageFormatPrefWidget() { delete ui; }

QString ImageFormatPrefWidget::getFormatName() const { return m_formatName; }

QList<ImageOptimizer> ImageFormatPrefWidget::formatOptimizers() const {
  return m_formatOptimizers;
}

void ImageFormatPrefWidget::optimizerChanged(int index) {
  Q_UNUSED(index);
  updateFotmatSettingPBTooltip();
}

void ImageFormatPrefWidget::updateFotmatSettingPBTooltip() {
  ui->formatSettingsPushButton->setToolTip(
      ui->optimizersComboBox->currentText() + " | " + tr("Preferences"));
}
