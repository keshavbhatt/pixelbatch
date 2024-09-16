#include "jpegoptimprefwidget.h"
#include "ui_jpegoptimprefwidget.h"

JpegOptimPrefWidget::JpegOptimPrefWidget(QWidget *parent)
    : ImageOptimizerPrefWidget(parent), ui(new Ui::JpegOptimPrefWidget) {
  ui->setupUi(this);
}

JpegOptimPrefWidget::~JpegOptimPrefWidget() { delete ui; }

void JpegOptimPrefWidget::loadSettings()
{

}
