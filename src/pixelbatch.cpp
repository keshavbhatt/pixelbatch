#include "pixelbatch.h"
#include "elideditemdelegate.h"
#include "ui_pixelbatch.h"

PixelBatch::PixelBatch(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PixelBatch), m_taskWidget(nullptr),
    m_statusBarAddButton(nullptr), m_statusBarProcessButton(nullptr),
      m_permanentStatusbarMessageLabel(new QLabel(this)),
      m_fileHandler(nullptr) {

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

    m_taskWidget->setColumnCount(4);

    QStringList horizontalHeaders;
    horizontalHeaders << "Status" << tr("File") << tr("Size") << tr("Savings");
    m_taskWidget->setHorizontalHeaderLabels(horizontalHeaders);

    m_taskWidget->setAlternatingRowColors(true);

    // status column will have fixed size
    int firstColumnWidth = 56;
    m_taskWidget->setColumnWidth(0, firstColumnWidth);

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

  statusBar()->setSizeGripEnabled(false);

  // init m_fielHandler
  if (m_fileHandler == nullptr) {
    m_fileHandler = new FileHandler(this);
    connect(m_fileHandler, &FileHandler::addFileToTable, m_taskWidget,
            &TaskWidget::addFileToTable);
  }

  QWidget *permanentWidget = new QWidget;
  // init m_statusBarAddButton
  if (m_statusBarAddButton == nullptr) {
    QHBoxLayout *layout = new QHBoxLayout(permanentWidget);
    layout->setSpacing(3);
    layout->setContentsMargins(6, 6, 6, 6);

    // fixed message label
    layout->addWidget(m_permanentStatusbarMessageLabel);

    // add "Add Images" button
    m_statusBarAddButton = new QPushButton(tr("Add Images"), this);
    connect(m_statusBarAddButton, &QPushButton::clicked, m_fileHandler,
            &FileHandler::addFiles);
    layout->addWidget(m_statusBarAddButton);

    permanentWidget->setLayout(layout);
    statusBar()->addPermanentWidget(permanentWidget);
  }

  if(m_statusBarProcessButton  == nullptr){
      m_statusBarProcessButton = new QPushButton(tr("Process Images"), this);
      connect(m_statusBarProcessButton, &QPushButton::clicked, m_taskWidget, &TaskWidget::processImages);

      permanentWidget->layout()->addWidget(m_statusBarProcessButton);
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
  fileMenu->addAction(addImagesAction);

  QAction *settingsAction = new QAction(tr("Preferences"), fileMenu);
  settingsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
  fileMenu->addAction(settingsAction);

  fileMenu->addSeparator();

  QAction *quitAction = new QAction(tr("Quit"), fileMenu);
  quitAction->setShortcut(QKeySequence(QKeySequence::Quit));
  fileMenu->addAction(quitAction);
  // END FILE MENU

  // EDIT MENU
  QAction *clearAction = new QAction(tr("Cancel All Operations"), fileMenu);
  clearAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
  editMenu->addAction(clearAction);
  // END EDIT MENU

  // ABOUT MENU
  QAction *reportIssueAction = new QAction(tr("Report Issue"), fileMenu);
  aboutMenu->addAction(reportIssueAction);

  QAction *donateAction = new QAction(tr("Donate"), fileMenu);
  aboutMenu->addAction(donateAction);

  aboutMenu->addSeparator();

  QAction *aboutAction = new QAction(tr("About"), fileMenu);
  aboutMenu->addAction(aboutAction);
  // END  ABOUT MENU

  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(editMenu);
  menuBar()->addMenu(aboutMenu);
}

void PixelBatch::setStatus(const QString &message) {
  m_permanentStatusbarMessageLabel->setText(message);
}

void PixelBatch::toggleShowStatusBarAddButton(const bool visible) {
  m_statusBarAddButton->setVisible(visible);
}
