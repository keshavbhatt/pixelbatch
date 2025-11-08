#include "taskactionwidget.h"
#include "ui_taskactionwidget.h"

TaskActionWidget::TaskActionWidget(QWidget *parent, TaskWidget *taskWidget)
    : QWidget(parent), ui(new Ui::TaskActionWidget), m_taskWidget(taskWidget) {
  ui->setupUi(this);

  connect(ui->removeFromListPb, &QPushButton::clicked, m_taskWidget,
          &TaskWidget::removeSelectedRow);

  connect(ui->openOptimizedInFilesPb, &QPushButton::clicked, m_taskWidget,
          &TaskWidget::openOptimizedImageInFileManagerForSelectedTask);

  connect(ui->openOptimizedPb, &QPushButton::clicked, m_taskWidget,
          &TaskWidget::openOptimizedImageInImageViewerForSelectedTask);

  connect(ui->openOriginalPb, &QPushButton::clicked, m_taskWidget,
          &TaskWidget::openOriginalImageInImageViewerForSelectedTask);

  connect(ui->compareImagesPb, &QPushButton::clicked, m_taskWidget,
          &TaskWidget::compareImagesForSelectedTask);
}

TaskActionWidget::~TaskActionWidget() { delete ui; }

void TaskActionWidget::updateActions() {

  if (!m_taskWidget) {
    return;
  }

  bool hasSelection = m_taskWidget->hasSelection();

  ui->removeFromListPb->setEnabled(hasSelection);
  ui->openOptimizedInFilesPb->setEnabled(hasSelection);
  ui->openOptimizedPb->setEnabled(hasSelection);
  ui->openOriginalPb->setEnabled(hasSelection);
  ui->compareImagesPb->setEnabled(hasSelection);
}
