#include "pixelbatch.h"
#include "about.h"
#include "desktoputils.h"
#include "elideditemdelegate.h"
#include "thememanager.h"
#include "ui_pixelbatch.h"

#include <QDesktopWidget>
#include <QScreen>
#include <QThread>

PixelBatch::PixelBatch(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PixelBatch),
      m_settings(Settings::instance()),
      m_preferencesWidget(new PreferencesWidget(this)),
      m_taskWidget(new TaskWidget(this)),
      m_taskActionWidget(new TaskActionWidget(this, m_taskWidget)),
      m_imageDetailPanel(new ImageDetailPanel(this)),
      m_emptyStateWidget(nullptr),
      m_mainSplitter(new QSplitter(Qt::Horizontal, this)),
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

  applySavedThemeAndStyle();

  setWindowTitle(QApplication::applicationName() + " | v" + VERSIONSTR);

  setWindowIcon(QIcon(":/icons/app/icon-64.png"));

  setMinimumSize(900, 500); // Increased minimum size for split layout

  initTaskWidget();

  initTaskActionWidget();

  initImageDetailPanel();

  setupMainLayout();

  setupStatusBar();

  initMenuBar();

  initPreferencesWidget();

  updateStatusBarButtons(); // keep it last
}

PixelBatch::~PixelBatch() {
  qDebug() << "PixelBatch destructor - cleaning up";

  // Cancel any active processing before destroying
  if (m_taskWidget) {
    qDebug() << "Calling cancelAllProcessing...";
    m_taskWidget->cancelAllProcessing();
    qDebug() << "cancelAllProcessing complete";

    // Give a moment for processes to die
    QThread::msleep(100);
  }

  qDebug() << "Deleting UI";
  delete ui;
  qDebug() << "PixelBatch destructor complete";
}

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

  connect(m_taskWidget, &TaskWidget::statusMessageUpdated, this,
          &PixelBatch::setStatus);

  connect(m_taskWidget, &TaskWidget::isProcessingChanged, this,
          &PixelBatch::updateStatusBarButtons);
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

  m_taskActionWidget->setVisible(false);
  m_imageDetailPanel->setVisible(false);
}

void PixelBatch::initImageDetailPanel() {
  m_emptyStateWidget = new EmptyStateWidget(this);

  connect(m_taskWidget, &TaskWidget::selectedImageTaskChanged,
          m_imageDetailPanel, &ImageDetailPanel::setImageTask);

  connect(m_taskWidget, &TaskWidget::selectionChangedCustom,
          this, [this]() {
            bool hasSelection = m_taskWidget->hasSelection();
            m_imageDetailPanel->setVisible(hasSelection);
            m_emptyStateWidget->setVisible(!hasSelection);
          });

  m_imageDetailPanel->setMinimumWidth(300);

  m_imageDetailPanel->setVisible(false);
  m_emptyStateWidget->setVisible(true);
}

void PixelBatch::setupMainLayout() {
  QWidget *leftPanel = new QWidget();
  QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(0);

  leftLayout->addWidget(m_taskWidget);
  leftLayout->addWidget(m_taskActionWidget);

  // A container for the right panel that can swap between empty state and detail panel
  QWidget *rightPanelContainer = new QWidget();
  QVBoxLayout *rightPanelLayout = new QVBoxLayout(rightPanelContainer);
  rightPanelLayout->setContentsMargins(0, 0, 0, 0);
  rightPanelLayout->setSpacing(0);

  rightPanelLayout->addWidget(m_emptyStateWidget);
  rightPanelLayout->addWidget(m_imageDetailPanel);

  // Add both panels to the splitter
  m_mainSplitter->addWidget(leftPanel);
  m_mainSplitter->addWidget(rightPanelContainer);

  // Set splitter proportions (70% for task list, 30% for detail panel)
  m_mainSplitter->setStretchFactor(0, 7);
  m_mainSplitter->setStretchFactor(1, 3);

  // Make splitter more flexible - allow collapsing
  m_mainSplitter->setCollapsible(0,
                                 false); // Don't collapse task list completely
  m_mainSplitter->setCollapsible(1, true); // Allow detail panel to be collapsed

  // Add splitter to the main layout
  ui->taskWidgetLayout->addWidget(m_mainSplitter);
}

void PixelBatch::setStatus(const QString &message) {
  m_StatusbarPermanentMessageLabel->setText(message);
}

void PixelBatch::updateStatusBarButtons(bool processing) {
  if (m_taskWidget) {
    auto taskStatusCounts = m_taskWidget->getTaskStatusCounts();

    // Enable process button if there are pending tasks OR completed/error tasks
    // (for re-processing)
    bool hasTasksToProcess = taskStatusCounts.pendingCount > 0 ||
                             taskStatusCounts.completedCount > 0 ||
                             taskStatusCounts.errorCount > 0;

    m_statusBarProcessButton->setEnabled(hasTasksToProcess && !processing);

    // Update button text based on state
    if (taskStatusCounts.pendingCount > 0) {
      m_statusBarProcessButton->setText(tr("Process Images"));
    } else if (taskStatusCounts.completedCount > 0 ||
               taskStatusCounts.errorCount > 0) {
      m_statusBarProcessButton->setText(tr("Re-process Images"));
    }

    m_statusBarAddButton->setEnabled(!processing);
  }
}

void PixelBatch::updateMenuActions(bool processing) {
  if (m_taskWidget) {
    auto taskStatusCounts = m_taskWidget->getTaskStatusCounts();

    // Enable process button if there are pending tasks OR completed/error tasks
    // (for re-processing)
    bool hasTasksToProcess = taskStatusCounts.pendingCount > 0 ||
                             taskStatusCounts.completedCount > 0 ||
                             taskStatusCounts.errorCount > 0;

    m_statusBarProcessButton->setEnabled(hasTasksToProcess && !processing);
    m_statusBarAddButton->setEnabled(!processing);
  }
}

void PixelBatch::addImages() {
  if (m_fileHandler) {
    m_fileHandler->addFiles(m_settings.getLastOpenedImageDirPath());
  } else {
    qWarning() << "Unable to Add files, fileHandler not initialized.";
    QMessageBox::critical(this, tr("Error"),
                          tr("Unable to add images. Please try again."));
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

void PixelBatch::reportIssue() {
  DesktopUtils::openUrl("https://github.com/keshavbhatt/pixelbatch/issues");
}

void PixelBatch::donate() {
  DesktopUtils::openUrl("https://github.com/sponsors/pixelbatch");
}

void PixelBatch::showAbout() {
  About *aboutDialog = new About(this);
  aboutDialog->setAttribute(Qt::WA_DeleteOnClose);

  // Center on screen
  int screenNumber = qApp->desktop()->screenNumber(this);
  QRect screenRect = QGuiApplication::screens().at(screenNumber)->geometry();
  aboutDialog->move(screenRect.center() - aboutDialog->rect().center());

  aboutDialog->show();
}

void PixelBatch::applySavedThemeAndStyle() {
  bool useSystemTheme = m_settings.getUseSystemTheme();

  if (useSystemTheme) {
    // Use system theme and style
    ThemeManager::applyTheme("", true);
    ThemeManager::applyStyle("", true);
  } else {
    // Apply saved custom theme
    QString theme = m_settings.getTheme();
    ThemeManager::applyTheme(theme, false);

    // Apply saved custom style
    QString style = m_settings.getStyle();
    ThemeManager::applyStyle(style, false);
  }
}

void PixelBatch::addFileFromCommandLine(const QString &filePath) {
  if (m_taskWidget) {
    m_taskWidget->addFileToTable(filePath);
  }
}
