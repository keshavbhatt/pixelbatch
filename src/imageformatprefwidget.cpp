#include "imageformatprefwidget.h"
#include "OptimizerPrefWidgets/imageformatprefwidgetfactory.h"
#include "ui_imageformatprefwidget.h"

#include <QDebug>

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

  bool hasWidget = !formatOptimizers().isEmpty() &&
                   formatOptimizers().at(0).isValid() &&
                   ImageFormatPrefWidgetFactory::instance().hasPrefWidgetFor(
                       formatOptimizers().at(0));

  ui->formatSettingsPushButton->setEnabled(hasWidget);
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
