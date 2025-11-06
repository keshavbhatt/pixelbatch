#include "about.h"
#include "constants.h"
#include "ui_about.h"

#include <QDesktopServices>
#include <QGraphicsOpacityEffect>
#include <QProcess>
#include <QPropertyAnimation>
#include <QStyle>
#include <QStyleFactory>
#include <QSysInfo>
#include <QTimer>
#include <QUrl>

About::About(QWidget *parent) : QWidget(parent), ui(new Ui::About) {
  ui->setupUi(this);

  // Initialize application info
  appName = QApplication::applicationName();
  appDescription = "Batch Image Optimization Tool for JPEG, PNG, GIF, and SVG";
  appVersion = VERSIONSTR;
  appLicense = "GPL-3.0-or-later";

  // Organization
  appAuthorName = "Keshav Bhatt and Contributors";

  // Links
  appSourceCodeLink = "https://github.com/keshavbhatt/pixelbatch";
  appIssuesLink = "https://github.com/keshavbhatt/pixelbatch/issues";
  appWebsiteLink = "https://ktechpit.com/USS/public";
  donateLink = "https://www.paypal.com/paypalme/keshavnrj/"
               "15USD?note=Donation%20for%20PixelBatch%20Application";
  githubLink = "https://github.com/keshavbhatt";

  setupUI();
  setupConnections();

  setWindowTitle(appName + " | About");
  setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
s
  // Start with minimum size
  adjustSize();

  // Fade-in animation
  ui->centerWidget->hide();

  if (ui->centerWidget->graphicsEffect() == nullptr) {
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    ui->centerWidget->setGraphicsEffect(eff);
  }

  QPropertyAnimation *a =
      new QPropertyAnimation(ui->centerWidget->graphicsEffect(), "opacity");
  a->setDuration(800);
  a->setStartValue(0);
  a->setEndValue(1);
  a->setEasingCurve(QEasingCurve::InCurve);
  a->start(QPropertyAnimation::DeleteWhenStopped);
  ui->centerWidget->show();
}

About::~About() { delete ui; }

void About::setupUI() {
  // App name and description
  ui->appNameDesc->setText(
      QString("<p style='margin-top:4px; margin-bottom:4px; margin-left:0px;"
              " margin-right:0px; -qt-block-indent:0; text-indent:0px;'>"
              "<span style='font-size:16pt; font-weight:600;'>%1</span></p>"
              "<p style='margin-top:4px; margin-bottom:4px; margin-left:0px;"
              " margin-right:0px; -qt-block-indent:0; text-indent:0px;'>"
              "<span style='font-size:10pt;'>%2</span></p>")
          .arg(appName, appDescription));

  // Version and license
  ui->version->setText(QString("Version: %1 | %2").arg(appVersion, appLicense));

  // Credits and links
  ui->desc2->setText(
      QString("<p style='margin-top:2px; margin-bottom:2px;'>"
              "<span style='font-weight:600;'>Developed by:</span> %1</p>"
              "<p style='margin-top:2px; margin-bottom:2px;'>"
              "<span style='font-weight:600;'>Website:</span> "
              "<a href='%2'>%2</a></p>")
          .arg(appAuthorName, appWebsiteLink));

  // Generate and set debug info (hidden by default)
  ui->debugInfoText->setHtml(generateDebugInfo());
  ui->debugInfoText->hide();
  ui->debugInfoButton->setText(tr("Show Debug Info"));
}

void About::setupConnections() {
  connect(ui->donate, &QPushButton::clicked, this, &About::onDonateClicked);
  connect(ui->reportIssue, &QPushButton::clicked, this,
          &About::onReportIssueClicked);
  connect(ui->sourceCode, &QPushButton::clicked, this,
          &About::onSourceCodeClicked);
  connect(ui->website, &QPushButton::clicked, this, &About::onWebsiteClicked);
}

QString About::generateDebugInfo() {
  QString debugInfo = "<h3>System Information</h3>";
  debugInfo +=
      QString("<p><b>OS:</b> %1</p>").arg(QSysInfo::prettyProductName());
  debugInfo += QString("<p><b>Kernel:</b> %1 %2</p>")
                   .arg(QSysInfo::kernelType(), QSysInfo::kernelVersion());
  debugInfo += QString("<p><b>CPU Architecture:</b> %1</p>")
                   .arg(QSysInfo::currentCpuArchitecture());

  debugInfo += "<h3>Application Information</h3>";
  debugInfo += QString("<p><b>Version:</b> %1</p>").arg(appVersion);
  debugInfo += QString("<p><b>Application Name:</b> %1</p>").arg(appName);

#ifdef GIT_HASH
  debugInfo += QString("<p><b>Git Hash:</b> %1</p>").arg(GIT_HASH);
#endif
#ifdef GIT_BRANCH
  debugInfo += QString("<p><b>Git Branch:</b> %1</p>").arg(GIT_BRANCH);
#endif
#ifdef BUILD_TIMESTAMP
  debugInfo += QString("<p><b>Build Time:</b> %1</p>").arg(BUILD_TIMESTAMP);
#endif

  debugInfo += "<h3>Qt Information</h3>";
  debugInfo += QString("<p><b>Qt Version:</b> %1</p>").arg(qVersion());
  debugInfo += QString("<p><b>Qt Build:</b> %1</p>").arg(QT_VERSION_STR);

  // Current style and theme
  debugInfo += QString("<p><b>Current Style:</b> %1</p>")
                   .arg(QApplication::style()->objectName());

  // Available styles
  QStringList styles = QStyleFactory::keys();
  debugInfo +=
      QString("<p><b>Available Styles:</b> %1</p>").arg(styles.join(", "));

  debugInfo += "<h3>Optimizer Tools</h3>";

  // Check optimizer tools availability
  QStringList optimizers = {"jpegoptim", "pngquant", "gifsicle", "svgo"};
  for (const QString &tool : optimizers) {
    QProcess process;
    process.start(tool, QStringList() << "--version");
    process.waitForFinished(1000);

    QString version = "Not found";
    if (process.exitCode() == 0) {
      version = QString(process.readAllStandardOutput())
                    .trimmed()
                    .split('\n')
                    .first();
      if (version.isEmpty()) {
        version = QString(process.readAllStandardError())
                      .trimmed()
                      .split('\n')
                      .first();
      }
    }
    debugInfo += QString("<p><b>%1:</b> %2</p>")
                     .arg(tool, version.isEmpty() ? "Not found" : version);
  }

  return debugInfo;
}

void About::on_debugInfoButton_clicked() {
  if (ui->debugInfoText->isVisible()) {
    ui->debugInfoText->hide();
    ui->debugInfoButton->setText(tr("Show Debug Info"));
    QTimer::singleShot(150, this, [=]() { this->adjustSize(); });
  } else {
    ui->debugInfoText->show();
    ui->debugInfoButton->setText(tr("Hide Debug Info"));
    this->adjustSize();
  }
}

void About::onDonateClicked() { QDesktopServices::openUrl(QUrl(donateLink)); }

void About::onReportIssueClicked() {
  QDesktopServices::openUrl(QUrl(appIssuesLink));
}

void About::onSourceCodeClicked() {
  QDesktopServices::openUrl(QUrl(appSourceCodeLink));
}

void About::onWebsiteClicked() {
  QDesktopServices::openUrl(QUrl(appWebsiteLink));
}
