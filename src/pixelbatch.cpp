#include "pixelbatch.h"
#include "elideditemdelegate.h"
#include "ui_pixelbatch.h"
#include <QHeaderView>

PixelBatch::PixelBatch(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PixelBatch), m_taskWidget(nullptr),
      m_statusBarAddButton(nullptr),
      m_permanentStatusbarMessageLabel(new QLabel(this)) {

  ui->setupUi(this);

  setWindowTitle(QApplication::applicationName() + " | v" + VERSIONSTR);

  setWindowIcon(QIcon(":/icons/app/icon-64.png"));

  initTaskWidget();
  setupStatusBar();
}

PixelBatch::~PixelBatch() { delete ui; }

void PixelBatch::initTaskWidget() {

  if (m_taskWidget == nullptr) {

    m_taskWidget = new TaskWidget(this);

    m_taskWidget->setColumnCount(4);

    QStringList horizontalHeaders;
    horizontalHeaders << ""
                      << "File"
                      << "Size"
                      << "Savings";
    m_taskWidget->setHorizontalHeaderLabels(horizontalHeaders);

    m_taskWidget->setAlternatingRowColors(true);

    // status column will have fixed size
    int firstColumnWidth = 56;
    m_taskWidget->setColumnWidth(0, firstColumnWidth);

    // resize all other columns in stretch
    m_taskWidget->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Stretch);
    m_taskWidget->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::Stretch);
    m_taskWidget->horizontalHeader()->setSectionResizeMode(
        3, QHeaderView::Stretch);

    // filenames can be long string, elide them
    ElidedItemDelegate *elideItemDelegate = new ElidedItemDelegate(this);
    m_taskWidget->setItemDelegateForColumn(1, elideItemDelegate);
    m_taskWidget->setWordWrap(false);

    // connections
    connect(m_taskWidget, &TaskWidget::setStatusRequested, this,
            &PixelBatch::setStatus);

    connect(m_taskWidget, &TaskWidget::toggleShowStatusBarAddButton, this,
            &PixelBatch::toggleShowStatusBarAddButton);

    ui->taskWidgetLayout->addWidget(m_taskWidget);
  }
}

void PixelBatch::setupStatusBar() {
  if (m_statusBarAddButton == nullptr) {

    statusBar()->setSizeGripEnabled(false);

    QWidget *permanentWidget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(permanentWidget);
    layout->setSpacing(3);
    layout->setContentsMargins(6, 6, 6, 6);

    // fixed message label
    layout->addWidget(m_permanentStatusbarMessageLabel);

    // add images button
    m_statusBarAddButton = new QPushButton("Add Images", this);
    connect(m_statusBarAddButton, &QPushButton::clicked, this, [=]() {

    });
    layout->addWidget(m_statusBarAddButton);

    permanentWidget->setLayout(layout);
    statusBar()->addPermanentWidget(permanentWidget);

    statusBar()->showMessage(QString("Welcome to %1 ver: %2")
                                 .arg(qApp->applicationName(), VERSIONSTR));
  }
}

void PixelBatch::setStatus(const QString &message) {
  m_permanentStatusbarMessageLabel->setText(message);
}

void PixelBatch::toggleShowStatusBarAddButton(const bool visible) {
  m_statusBarAddButton->setVisible(visible);
}
