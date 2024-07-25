#include "pixelbatch.h"
#include "elideditemdelegate.h"
#include "ui_pixelbatch.h"

PixelBatch::PixelBatch(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PixelBatch), m_taskWidget(nullptr),
      m_statusBarPermanentWidget(nullptr), m_statusBarAddButton(nullptr),
      m_statusBarProcessButton(nullptr),
      m_StatusbarPermanentMessageLabel(nullptr), m_fileHandler(nullptr),
      m_settings(Settings::instance()){

  ui->setupUi(this);

  setWindowTitle(QApplication::applicationName() + " | v" + VERSIONSTR);

  setWindowIcon(QIcon(":/icons/app/icon-64.png"));

  initTaskWidget();

  setupStatusBar();

  initMenuBar();
}

PixelBatch::~PixelBatch() { delete ui; }

void PixelBatch::initTaskWidget() {

  if (m_taskWidget == nullptr) {

    m_taskWidget = new TaskWidget(this);

    m_taskWidget->setColumnCount(5);

    QStringList horizontalHeaders;
    horizontalHeaders << tr("File")        // 0
                      << tr("Status")      // 1
                      << tr("Size Before") // 2
                      << tr("Size After")  // 3
                      << tr("Savings");    // 4
    m_taskWidget->setHorizontalHeaderLabels(horizontalHeaders);

    m_taskWidget->setAlternatingRowColors(true);

    // status column will have fixed size
    int firstColumnWidth = 56;
    m_taskWidget->setColumnWidth(1, firstColumnWidth);

    // filenames can be long string, elide them
    ElidedItemDelegate *elideItemDelegate = new ElidedItemDelegate(this);
    m_taskWidget->setItemDelegateForColumn(0, elideItemDelegate);
    m_taskWidget->setWordWrap(false);

    // connections
    connect(m_taskWidget, &TaskWidget::setStatusRequested, this,
            &PixelBatch::setStatus);

    connect(m_taskWidget, &TaskWidget::toggleShowStatusBarAddButton, this,
            &PixelBatch::toggleShowStatusBarAddButton);

    connect(m_taskWidget, &TaskWidget::statusMessageUpdated, this,
            &PixelBatch::setStatus);

    ui->taskWidgetLayout->addWidget(m_taskWidget);
  }
}

void PixelBatch::setupStatusBar() {

  statusBar()->setSizeGripEnabled(false);

  // init m_fielHandler
  if (m_fileHandler == nullptr) {
    m_fileHandler = new FileHandler(this);
    connect(m_fileHandler, &FileHandler::addFileToTable, m_taskWidget,
            &TaskWidget::addFileToTable);
  }

  if (m_statusBarPermanentWidget == nullptr) {
    m_statusBarPermanentWidget = new QWidget(this->statusBar());
  }

  // init m_statusBarAddButton
  if (m_statusBarAddButton == nullptr) {
    QHBoxLayout *layout = new QHBoxLayout(m_statusBarPermanentWidget);
    layout->setSpacing(3);
    layout->setContentsMargins(6, 6, 6, 6);

    if (m_StatusbarPermanentMessageLabel == nullptr) {
      m_StatusbarPermanentMessageLabel = new QLabel(this);
    }
    layout->addWidget(m_StatusbarPermanentMessageLabel);

    // add "Add Images" button
    m_statusBarAddButton = new QPushButton(tr("Add Images"), this);
    connect(m_statusBarAddButton, &QPushButton::clicked, this,
            &PixelBatch::addImages);
    layout->addWidget(m_statusBarAddButton);

    m_statusBarPermanentWidget->setLayout(layout);
    statusBar()->addPermanentWidget(m_statusBarPermanentWidget);
  }

  // init m_statusBarAddButton
  if (m_statusBarProcessButton == nullptr) {
    m_statusBarProcessButton = new QPushButton(tr("Process Images"), this);
    connect(m_statusBarProcessButton, &QPushButton::clicked, m_taskWidget,
            &TaskWidget::processImages);

    if (m_statusBarPermanentWidget) {
      m_statusBarPermanentWidget->layout()->addWidget(m_statusBarProcessButton);
    }
  }

  statusBar()->showMessage(QString(tr("Welcome to %1 ver: %2"))
                               .arg(qApp->applicationName(), VERSIONSTR));
}

void PixelBatch::initMenuBar() {
  QMenu *fileMenu = new QMenu(tr("File"), this);
  QMenu *editMenu = new QMenu(tr("Edit"), this);
  QMenu *aboutMenu = new QMenu(tr("About"), this);

  // FILE MENU
  QAction *addImagesAction = new QAction(tr("Add Images"), fileMenu);
  addImagesAction->setShortcut(QKeySequence(QKeySequence::Open));
  connect(addImagesAction, &QAction::triggered, this, &PixelBatch::addImages);
  fileMenu->addAction(addImagesAction);

  QAction *settingsAction = new QAction(tr("Preferences"), fileMenu);
  settingsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
  connect(settingsAction, &QAction::triggered, this, &PixelBatch::openSettings);
  fileMenu->addAction(settingsAction);

  fileMenu->addSeparator();

  QAction *quitAction = new QAction(tr("Quit"), fileMenu);
  quitAction->setShortcut(QKeySequence(QKeySequence::Quit));
  connect(quitAction, &QAction::triggered, this, &PixelBatch::quitApplication);
  fileMenu->addAction(quitAction);
  // END FILE MENU

  // EDIT MENU
  QAction *removeFinishedAction =
      new QAction(tr("Remove Finished Tasks"), editMenu);
  removeFinishedAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  connect(removeFinishedAction, &QAction::triggered, this,
          &PixelBatch::removeFinishedOperations);
  editMenu->addAction(removeFinishedAction);

  QAction *clearAction = new QAction(tr("Cancel All Tasks"), editMenu);
  clearAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
  connect(clearAction, &QAction::triggered, this,
          &PixelBatch::clearAllOperations);
  editMenu->addAction(clearAction);
  // END EDIT MENU

  // ABOUT MENU
  QAction *reportIssueAction = new QAction(tr("Report Issue"), aboutMenu);
  connect(reportIssueAction, &QAction::triggered, this,
          &PixelBatch::reportIssue);
  aboutMenu->addAction(reportIssueAction);

  QAction *donateAction = new QAction(tr("Donate"), aboutMenu);
  connect(donateAction, &QAction::triggered, this, &PixelBatch::donate);
  aboutMenu->addAction(donateAction);

  aboutMenu->addSeparator();

  QAction *aboutAction = new QAction(tr("About"), aboutMenu);
  connect(aboutAction, &QAction::triggered, this, &PixelBatch::showAbout);
  aboutMenu->addAction(aboutAction);
  // END ABOUT MENU

  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(editMenu);
  menuBar()->addMenu(aboutMenu);
}

void PixelBatch::setStatus(const QString &message) {
  m_StatusbarPermanentMessageLabel->setText(message);
}

void PixelBatch::toggleShowStatusBarAddButton(const bool visible) {
  m_statusBarAddButton->setVisible(visible);
}

void PixelBatch::addImages() {
  if (m_fileHandler) {
    m_fileHandler->addFiles(m_settings.getLastOpenedImageDirPath());
  } else {
    qWarning() << "Unable to Add files, fileHandler not initialized.";
  }
}

void PixelBatch::openSettings() { Q_UNIMPLEMENTED(); }

void PixelBatch::quitApplication() { QApplication::quit(); }

void PixelBatch::removeFinishedOperations() { Q_UNIMPLEMENTED(); }

void PixelBatch::clearAllOperations() { Q_UNIMPLEMENTED(); }

void PixelBatch::reportIssue() { Q_UNIMPLEMENTED(); }

void PixelBatch::donate() { Q_UNIMPLEMENTED(); }

void PixelBatch::showAbout() {
  QMessageBox::about(
      this, tr("About") + " " + QApplication::applicationName(),
      QApplication::applicationName() + " " +
          tr("is an application for batch processing of images."));
}
