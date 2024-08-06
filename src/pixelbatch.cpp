#include "pixelbatch.h"
#include "elideditemdelegate.h"
#include "ui_pixelbatch.h"

#include <QDesktopWidget>
#include <QScreen>

PixelBatch::PixelBatch(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PixelBatch),
      m_settings(Settings::instance()),
      m_preferencesWidget(new PreferencesWidget(this)),
      m_taskWidget(new TaskWidget(this)),
      m_taskActionWidget(new TaskActionWidget(this)),
      m_statusBarPermanentWidget(new QWidget(this->statusBar())),
      m_statusBarAddButton(
          new QPushButton(QIcon(":/resources/icons/image-add-line.png"),
                          tr("Add Images"), this)),
      m_statusBarProcessButton(
          new QPushButton(QIcon(":/resources/icons/bubble-chart-line.png"),
                          tr("Process Images"), this)),
      m_StatusbarPermanentMessageLabel(new QLabel(this)),
      m_fileHandler(new FileHandler(this)) {

  ui->setupUi(this);

  setWindowTitle(QApplication::applicationName() + " | v" + VERSIONSTR);

  setWindowIcon(QIcon(":/icons/app/icon-64.png"));

  initTaskWidget();

  initTaskActionWidget();

  setupStatusBar();

  initMenuBar();

  initPreferencesWidget();
}

PixelBatch::~PixelBatch() { delete ui; }

void PixelBatch::initTaskWidget() {

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

void PixelBatch::setupStatusBar() {

  statusBar()->setSizeGripEnabled(false);

  // init m_fielHandler
  connect(m_fileHandler, &FileHandler::addFileToTable, m_taskWidget,
          &TaskWidget::addFileToTable);

  // init m_statusBarAddButton

  QHBoxLayout *layout = new QHBoxLayout(m_statusBarPermanentWidget);
  layout->setSpacing(3);
  layout->setContentsMargins(6, 6, 6, 6);

  if (m_StatusbarPermanentMessageLabel == nullptr) {
    m_StatusbarPermanentMessageLabel = new QLabel(this);
  }
  layout->addWidget(m_StatusbarPermanentMessageLabel);

  // init "Add Images" button
  connect(m_statusBarAddButton, &QPushButton::clicked, this,
          &PixelBatch::addImages);
  layout->addWidget(m_statusBarAddButton);

  m_statusBarPermanentWidget->setLayout(layout);
  statusBar()->addPermanentWidget(m_statusBarPermanentWidget);

  // init m_statusBarAddButton
  connect(m_statusBarProcessButton, &QPushButton::clicked, m_taskWidget,
          &TaskWidget::processImages);

  if (m_statusBarPermanentWidget) {
    m_statusBarPermanentWidget->layout()->addWidget(m_statusBarProcessButton);
  }

  statusBar()->showMessage(
      QString(tr("%1 ver: %2")).arg(qApp->applicationName(), VERSIONSTR));
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
  QAction *removeAllFinishedTasksAction =
      new QAction(tr("Remove Finished Tasks"), editMenu);
  removeAllFinishedTasksAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  connect(removeAllFinishedTasksAction, &QAction::triggered, m_taskWidget,
          &TaskWidget::removeFinishedOperations);
  editMenu->addAction(removeAllFinishedTasksAction);

  QAction *removeAllTasksAction = new QAction(tr("Remove All Tasks"), editMenu);
  removeAllTasksAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
  connect(removeAllTasksAction, &QAction::triggered, m_taskWidget,
          &TaskWidget::clearAllOperations);
  editMenu->addAction(removeAllTasksAction);
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

void PixelBatch::initPreferencesWidget() {

  m_preferencesWidget->setWindowFlags(Qt::Dialog);
  m_preferencesWidget->setWindowTitle(QApplication::applicationName() + " | " +
                                      tr("Preferences"));
}

void PixelBatch::initTaskActionWidget() {

  QObject::connect(m_taskWidget, &TaskWidget::selectionChangedCustom,
                   m_taskActionWidget, &TaskActionWidget::updateActions);

  QObject::connect(m_taskWidget, &TaskWidget::toggleShowTaskActionWidget,
                   m_taskActionWidget, &TaskActionWidget::setVisible);

  ui->taskActionWidgetLayout->addWidget(m_taskActionWidget);

  m_taskActionWidget->setVisible(false);
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

void PixelBatch::openSettings() {
  if (m_preferencesWidget && m_preferencesWidget->isVisible() == false) {
    int screenNumber = qApp->desktop()->screenNumber(this);
    QRect screenRect = QGuiApplication::screens().at(screenNumber)->geometry();
    if (!screenRect.contains(m_preferencesWidget->pos())) {
      m_preferencesWidget->move(screenRect.center() -
                                m_preferencesWidget->rect().center());
    }
    m_preferencesWidget->show();
  }
}

void PixelBatch::quitApplication() { QApplication::quit(); }

void PixelBatch::reportIssue() { Q_UNIMPLEMENTED(); }

void PixelBatch::donate() { Q_UNIMPLEMENTED(); }

void PixelBatch::showAbout() {
  QMessageBox::about(
      this, tr("About") + " " + QApplication::applicationName(),
      QApplication::applicationName() + " " +
          tr("is an application for batch processing of images."));
}
