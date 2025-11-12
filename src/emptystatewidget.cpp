#include "emptystatewidget.h"

#include <QIcon>

EmptyStateWidget::EmptyStateWidget(QWidget *parent) : QWidget(parent) {
  setupUI();
}

EmptyStateWidget::~EmptyStateWidget() {}

void EmptyStateWidget::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(40, 40, 40, 40);

  QLabel *iconLabel = new QLabel();
  iconLabel->setPixmap(
      QIcon(":/resources/icons/image-line.png").pixmap(64, 64));
  iconLabel->setAlignment(Qt::AlignCenter);

  QLabel *titleLabel = new QLabel(tr("No Image Selected"));
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(titleFont.pointSize() + 4);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  titleLabel->setAlignment(Qt::AlignCenter);

  QLabel *instructionLabel = new QLabel(
      tr("Select an image from the list to:\n\n"
         "• View image preview and details\n"
         "• Customize optimization settings\n"
         "• See format-specific options"));
  instructionLabel->setAlignment(Qt::AlignCenter);
  instructionLabel->setWordWrap(true);
  instructionLabel->setEnabled(false);

  mainLayout->addStretch();
  mainLayout->addWidget(iconLabel);
  mainLayout->addSpacing(20);
  mainLayout->addWidget(titleLabel);
  mainLayout->addSpacing(10);
  mainLayout->addWidget(instructionLabel);
  mainLayout->addStretch();

  setMinimumWidth(300);
  setLayout(mainLayout);
}

