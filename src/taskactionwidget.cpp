#include "taskactionwidget.h"
#include "ui_taskactionwidget.h"

TaskActionWidget::TaskActionWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::TaskActionWidget) {
  ui->setupUi(this);
}

TaskActionWidget::~TaskActionWidget() { delete ui; }

void TaskActionWidget::updateActions(QTableWidget *taskWidget) {
  QList<QTableWidgetItem *> selectedItems = taskWidget->selectedItems();
  bool hasSelection = !selectedItems.isEmpty();

  ui->pushButton->setEnabled(hasSelection);
  ui->pushButton_2->setDisabled(hasSelection);

  // if (hasSelection) {
  //   QTableWidgetItem *item = selectedItems.first();
  //   detailsLabel->setText("Details: " + item->text());
  // } else {
  //   detailsLabel->setText("Details: ");
  // }
}
