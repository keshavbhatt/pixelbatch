#include "imageformatprefwidget.h"
#include "ui_imageformatprefwidget.h"

ImageFormatPrefWidget::ImageFormatPrefWidget(QWidget *parent,
                                             QString formatName,
                                             QStringList optimizers)
    : QWidget(parent), ui(new Ui::ImageFormatPrefWidget),
      m_formatName(formatName), m_formatOptimizers(optimizers) {
  ui->setupUi(this);

  ui->formatSettingsPushButton->setSizePolicy(QSizePolicy::Maximum,
                                              QSizePolicy::Maximum);
  ui->formatSettingsPushButton->setText("");
  ui->formatSettingsPushButton->setIcon(
      QIcon(":/resources/icons/settings-2-line.png"));
  connect(ui->optimizersComboBox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [=](int index) { this->optimizerChanged(index); });

  ui->formatName->setText(formatName);
  ui->optimizersComboBox->addItems(formatOptimizers());
}

ImageFormatPrefWidget::~ImageFormatPrefWidget() { delete ui; }

QString ImageFormatPrefWidget::getFormatName() const { return m_formatName; }

QStringList ImageFormatPrefWidget::formatOptimizers() const {
  return m_formatOptimizers;
}

void ImageFormatPrefWidget::optimizerChanged(int) {
  ui->formatSettingsPushButton->setToolTip(
      ui->optimizersComboBox->currentText() + " " + tr("Preferences"));
}
