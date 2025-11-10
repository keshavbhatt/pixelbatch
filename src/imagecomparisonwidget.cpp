#include "imagecomparisonwidget.h"

#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLocale>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>

ImageComparisonWidget::ImageComparisonWidget(const QString &originalPath,
                                             const QString &optimizedPath,
                                             QWidget *parent)
    : QDialog(parent), m_originalPath(originalPath),
      m_optimizedPath(optimizedPath), m_imageLabel(nullptr),
      m_leftLabel(nullptr), m_rightLabel(nullptr), m_slider(nullptr),
      m_toggleButton(nullptr), m_sideBySideWidget(nullptr),
      m_overlayWidget(nullptr), m_syncScrollCheckbox(nullptr),
      m_leftScroll(nullptr), m_rightScroll(nullptr), m_isSideBySideView(true),
      m_syncScrollBars(true), m_updatingScrollBars(false), m_zoomFactor(1.0) {

  setWindowTitle(tr("Compare Images - Before and After"));
  resize(1200, 700);

  setupUI();
  loadImages();
  loadViewMode();
}

ImageComparisonWidget::~ImageComparisonWidget() {}

void ImageComparisonWidget::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Toggle button at top
  m_toggleButton = new QPushButton(tr("Switch to Slider View"));
  m_toggleButton->setIcon(QIcon(":/resources/icons/image-edit-line.png"));
  connect(m_toggleButton, &QPushButton::clicked, this,
          &ImageComparisonWidget::toggleView);
  mainLayout->addWidget(m_toggleButton);

  // Side-by-side view
  m_sideBySideWidget = new QWidget();
  QVBoxLayout *sideBySideMainLayout = new QVBoxLayout(m_sideBySideWidget);
  sideBySideMainLayout->setContentsMargins(0, 0, 0, 0);

  // Sync checkbox and zoom controls for side-by-side view
  QHBoxLayout *controlsLayout = new QHBoxLayout();
  m_syncScrollCheckbox = new QCheckBox(tr("Synchronize scroll bars"));
  m_syncScrollCheckbox->setChecked(true);
  connect(m_syncScrollCheckbox, &QCheckBox::toggled, this,
          &ImageComparisonWidget::onSyncCheckboxToggled);
  controlsLayout->addWidget(m_syncScrollCheckbox);
  controlsLayout->addStretch();

  // Zoom controls
  QLabel *zoomLabel = new QLabel(tr("Zoom:"));
  controlsLayout->addWidget(zoomLabel);

  QPushButton *zoomOutBtn = new QPushButton(tr("−"));
  zoomOutBtn->setMaximumWidth(30);
  zoomOutBtn->setToolTip(tr("Zoom Out (Ctrl+-)"));
  connect(zoomOutBtn, &QPushButton::clicked, this, &ImageComparisonWidget::zoomOut);
  controlsLayout->addWidget(zoomOutBtn);

  QPushButton *zoomResetBtn = new QPushButton(tr("100%"));
  zoomResetBtn->setMaximumWidth(50);
  zoomResetBtn->setToolTip(tr("Reset Zoom (Ctrl+0)"));
  connect(zoomResetBtn, &QPushButton::clicked, this, &ImageComparisonWidget::zoomReset);
  controlsLayout->addWidget(zoomResetBtn);

  QPushButton *zoomInBtn = new QPushButton(tr("+"));
  zoomInBtn->setMaximumWidth(30);
  zoomInBtn->setToolTip(tr("Zoom In (Ctrl++)"));
  connect(zoomInBtn, &QPushButton::clicked, this, &ImageComparisonWidget::zoomIn);
  controlsLayout->addWidget(zoomInBtn);

  QPushButton *zoomFitBtn = new QPushButton(tr("Fit"));
  zoomFitBtn->setMaximumWidth(50);
  zoomFitBtn->setToolTip(tr("Fit to Window"));
  connect(zoomFitBtn, &QPushButton::clicked, this, &ImageComparisonWidget::zoomFit);
  controlsLayout->addWidget(zoomFitBtn);

  sideBySideMainLayout->addLayout(controlsLayout);

  QHBoxLayout *sideBySideLayout = new QHBoxLayout();
  sideBySideLayout->setContentsMargins(0, 0, 0, 0);

  QVBoxLayout *leftLayout = new QVBoxLayout();
  leftLayout->setContentsMargins(0, 0, 0, 0);
  QLabel *leftTitle = new QLabel(tr("<b>Original</b>"));
  leftTitle->setAlignment(Qt::AlignCenter);
  leftLayout->addWidget(leftTitle);

  m_leftScroll = new QScrollArea();
  m_leftLabel = new QLabel();
  m_leftLabel->setScaledContents(false);
  m_leftLabel->setAlignment(Qt::AlignCenter);
  m_leftScroll->setWidget(m_leftLabel);
  m_leftScroll->setWidgetResizable(true);
  leftLayout->addWidget(m_leftScroll);

  QVBoxLayout *rightLayout = new QVBoxLayout();
  rightLayout->setContentsMargins(0, 0, 0, 0);
  QLabel *rightTitle = new QLabel(tr("<b>Optimized</b>"));
  rightTitle->setAlignment(Qt::AlignCenter);
  rightLayout->addWidget(rightTitle);

  m_rightScroll = new QScrollArea();
  m_rightLabel = new QLabel();
  m_rightLabel->setScaledContents(false);
  m_rightLabel->setAlignment(Qt::AlignCenter);
  m_rightScroll->setWidget(m_rightLabel);
  m_rightScroll->setWidgetResizable(true);
  rightLayout->addWidget(m_rightScroll);

  QSplitter *splitter = new QSplitter(Qt::Horizontal);
  QWidget *leftWidget = new QWidget();
  leftWidget->setLayout(leftLayout);
  QWidget *rightWidget = new QWidget();
  rightWidget->setLayout(rightLayout);

  splitter->addWidget(leftWidget);
  splitter->addWidget(rightWidget);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 1);

  sideBySideLayout->addWidget(splitter);
  sideBySideMainLayout->addLayout(sideBySideLayout, 1);

  mainLayout->addWidget(m_sideBySideWidget, 1);

  // Overlay view
  m_overlayWidget = new QWidget();
  QVBoxLayout *overlayLayout = new QVBoxLayout(m_overlayWidget);
  overlayLayout->setContentsMargins(0, 0, 0, 0);

  // Title and zoom controls for overlay view
  QHBoxLayout *overlayTopLayout = new QHBoxLayout();

  QLabel *overlayTitle = new QLabel(
      tr("<b>Slider View</b> - Move slider to compare (Left: Original, Right: "
         "Optimized)"));
  overlayTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  overlayTopLayout->addWidget(overlayTitle, 1);

  overlayTopLayout->addStretch();

  // Zoom controls for slider view
  QLabel *zoomLabel2 = new QLabel(tr("Zoom:"));
  overlayTopLayout->addWidget(zoomLabel2);

  QPushButton *zoomOutBtn2 = new QPushButton(tr("−"));
  zoomOutBtn2->setMaximumWidth(30);
  zoomOutBtn2->setToolTip(tr("Zoom Out (Ctrl+-)"));
  connect(zoomOutBtn2, &QPushButton::clicked, this, &ImageComparisonWidget::zoomOut);
  overlayTopLayout->addWidget(zoomOutBtn2);

  QPushButton *zoomResetBtn2 = new QPushButton(tr("100%"));
  zoomResetBtn2->setMaximumWidth(50);
  zoomResetBtn2->setToolTip(tr("Reset Zoom (Ctrl+0)"));
  connect(zoomResetBtn2, &QPushButton::clicked, this, &ImageComparisonWidget::zoomReset);
  overlayTopLayout->addWidget(zoomResetBtn2);

  QPushButton *zoomInBtn2 = new QPushButton(tr("+"));
  zoomInBtn2->setMaximumWidth(30);
  zoomInBtn2->setToolTip(tr("Zoom In (Ctrl++)"));
  connect(zoomInBtn2, &QPushButton::clicked, this, &ImageComparisonWidget::zoomIn);
  overlayTopLayout->addWidget(zoomInBtn2);

  QPushButton *zoomFitBtn2 = new QPushButton(tr("Fit"));
  zoomFitBtn2->setMaximumWidth(50);
  zoomFitBtn2->setToolTip(tr("Fit to Window"));
  connect(zoomFitBtn2, &QPushButton::clicked, this, &ImageComparisonWidget::zoomFit);
  overlayTopLayout->addWidget(zoomFitBtn2);

  overlayLayout->addLayout(overlayTopLayout);

  QScrollArea *overlayScroll = new QScrollArea();
  m_imageLabel = new QLabel();
  m_imageLabel->setScaledContents(false);
  m_imageLabel->setAlignment(Qt::AlignCenter);
  overlayScroll->setWidget(m_imageLabel);
  overlayScroll->setWidgetResizable(true);
  overlayLayout->addWidget(overlayScroll, 1);

  m_slider = new QSlider(Qt::Horizontal);
  m_slider->setRange(0, 100);
  m_slider->setValue(50);
  connect(m_slider, &QSlider::valueChanged, this,
          &ImageComparisonWidget::updateComparison);
  overlayLayout->addWidget(m_slider);

  mainLayout->addWidget(m_overlayWidget, 1);
  m_overlayWidget->hide();

  // Info section at bottom with minimal space
  QGroupBox *infoGroup = new QGroupBox(tr("Image Information"));
  infoGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  QHBoxLayout *infoLayout = new QHBoxLayout(infoGroup);
  infoLayout->setContentsMargins(6, 6, 6, 6);

  QFileInfo originalInfo(m_originalPath);
  QFileInfo optimizedInfo(m_optimizedPath);
  QLocale locale;

  qint64 originalSize = originalInfo.size();
  qint64 optimizedSize = optimizedInfo.size();
  qint64 savedBytes = originalSize - optimizedSize;
  double savedPercent =
      (originalSize > 0)
          ? ((double)(originalSize - optimizedSize) / originalSize) * 100.0
          : 0.0;

  QString infoText = QString(tr("<b>Original:</b> %1 (%2 bytes)  |  "
                                "<b>Optimized:</b> %3 (%4 bytes)  |  "
                                "<b>Saved:</b> %5 (%6%)"))
                         .arg(locale.formattedDataSize(originalSize))
                         .arg(originalSize)
                         .arg(locale.formattedDataSize(optimizedSize))
                         .arg(optimizedSize)
                         .arg(locale.formattedDataSize(savedBytes))
                         .arg(QString::number(savedPercent, 'f', 2));

  QLabel *infoLabel = new QLabel(infoText);
  infoLabel->setWordWrap(true);
  infoLayout->addWidget(infoLabel);

  mainLayout->addWidget(infoGroup);

  // Close button
  QPushButton *closeButton = new QPushButton(tr("Close"));
  closeButton->setIcon(QIcon(":/resources/icons/close-line.png"));
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
  mainLayout->addWidget(closeButton);

  setLayout(mainLayout);

  // Connect scroll bars for synchronization
  connect(m_leftScroll->horizontalScrollBar(), &QScrollBar::valueChanged, this,
          &ImageComparisonWidget::onLeftScrollChanged);
  connect(m_leftScroll->verticalScrollBar(), &QScrollBar::valueChanged, this,
          &ImageComparisonWidget::onLeftScrollChanged);
  connect(m_rightScroll->horizontalScrollBar(), &QScrollBar::valueChanged, this,
          &ImageComparisonWidget::onRightScrollChanged);
  connect(m_rightScroll->verticalScrollBar(), &QScrollBar::valueChanged, this,
          &ImageComparisonWidget::onRightScrollChanged);
}

void ImageComparisonWidget::loadImages() {
  m_originalPixmap.load(m_originalPath);
  m_optimizedPixmap.load(m_optimizedPath);

  if (m_originalPixmap.isNull() || m_optimizedPixmap.isNull()) {
    qWarning() << "Failed to load one or both images";
    return;
  }

  // Scale images if they're too large (max 2000px in either dimension)
  int maxDim = 2000;
  if (m_originalPixmap.width() > maxDim || m_originalPixmap.height() > maxDim) {
    m_originalPixmap = m_originalPixmap.scaled(
        maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
  if (m_optimizedPixmap.width() > maxDim ||
      m_optimizedPixmap.height() > maxDim) {
    m_optimizedPixmap = m_optimizedPixmap.scaled(
        maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
}

void ImageComparisonWidget::showSideBySide() {
  m_sideBySideWidget->show();
  m_overlayWidget->hide();
  m_isSideBySideView = true;
  m_toggleButton->setText(tr("Switch to Slider View"));

  updateZoom();

  saveViewMode();
}

void ImageComparisonWidget::showOverlay() {
  m_sideBySideWidget->hide();
  m_overlayWidget->show();
  m_isSideBySideView = false;
  m_toggleButton->setText(tr("Switch to Side-by-Side View"));

  updateZoom();
  updateComparison(m_slider->value());

  saveViewMode();
}

void ImageComparisonWidget::updateComparison(int value) {
  if (m_originalPixmap.isNull() || m_optimizedPixmap.isNull()) {
    return;
  }

  // Use scaled pixmaps if zoom is active, otherwise use original
  QPixmap &originalToUse = (m_zoomFactor != 1.0 && !m_originalPixmapScaled.isNull())
                            ? m_originalPixmapScaled
                            : m_originalPixmap;
  QPixmap &optimizedToUse = (m_zoomFactor != 1.0 && !m_optimizedPixmapScaled.isNull())
                             ? m_optimizedPixmapScaled
                             : m_optimizedPixmap;

  // Create a composite image
  QPixmap composite(originalToUse.size());
  QPainter painter(&composite);

  // Calculate split position
  int splitX = (originalToUse.width() * value) / 100;

  // Draw original image (left part)
  painter.drawPixmap(0, 0, originalToUse);

  // Draw optimized image (right part)
  if (splitX < originalToUse.width()) {
    QRect sourceRect(splitX, 0, optimizedToUse.width() - splitX,
                     optimizedToUse.height());
    QRect targetRect(splitX, 0, originalToUse.width() - splitX,
                     originalToUse.height());
    painter.drawPixmap(targetRect, optimizedToUse, sourceRect);
  }

  // Draw a vertical line at the split position
  painter.setPen(QPen(Qt::yellow, 3));
  painter.drawLine(splitX, 0, splitX, originalToUse.height());

  painter.end();

  m_imageLabel->setPixmap(composite);
}

void ImageComparisonWidget::toggleView() {
  if (m_isSideBySideView) {
    showOverlay();
  } else {
    showSideBySide();
  }
}

void ImageComparisonWidget::syncScrollBars() {
  if (!m_syncScrollBars || m_updatingScrollBars) {
    return;
  }

  m_updatingScrollBars = true;

  // Sync horizontal scroll bars
  int hValueLeft = m_leftScroll->horizontalScrollBar()->value();
  int hMaxLeft = m_leftScroll->horizontalScrollBar()->maximum();
  int hMaxRight = m_rightScroll->horizontalScrollBar()->maximum();

  if (hMaxLeft > 0 && hMaxRight > 0) {
    double hRatio = (double)hValueLeft / hMaxLeft;
    m_rightScroll->horizontalScrollBar()->setValue((int)(hRatio * hMaxRight));
  }

  // Sync vertical scroll bars
  int vValueLeft = m_leftScroll->verticalScrollBar()->value();
  int vMaxLeft = m_leftScroll->verticalScrollBar()->maximum();
  int vMaxRight = m_rightScroll->verticalScrollBar()->maximum();

  if (vMaxLeft > 0 && vMaxRight > 0) {
    double vRatio = (double)vValueLeft / vMaxLeft;
    m_rightScroll->verticalScrollBar()->setValue((int)(vRatio * vMaxRight));
  }

  m_updatingScrollBars = false;
}

void ImageComparisonWidget::onLeftScrollChanged() {
  if (!m_syncScrollBars || m_updatingScrollBars) {
    return;
  }

  m_updatingScrollBars = true;

  // Sync to right scroll area
  int hValueLeft = m_leftScroll->horizontalScrollBar()->value();
  int hMaxLeft = m_leftScroll->horizontalScrollBar()->maximum();
  int hMaxRight = m_rightScroll->horizontalScrollBar()->maximum();

  if (hMaxLeft > 0 && hMaxRight > 0) {
    double hRatio = (double)hValueLeft / hMaxLeft;
    m_rightScroll->horizontalScrollBar()->setValue((int)(hRatio * hMaxRight));
  }

  int vValueLeft = m_leftScroll->verticalScrollBar()->value();
  int vMaxLeft = m_leftScroll->verticalScrollBar()->maximum();
  int vMaxRight = m_rightScroll->verticalScrollBar()->maximum();

  if (vMaxLeft > 0 && vMaxRight > 0) {
    double vRatio = (double)vValueLeft / vMaxLeft;
    m_rightScroll->verticalScrollBar()->setValue((int)(vRatio * vMaxRight));
  }

  m_updatingScrollBars = false;
}

void ImageComparisonWidget::onRightScrollChanged() {
  if (!m_syncScrollBars || m_updatingScrollBars) {
    return;
  }

  m_updatingScrollBars = true;

  // Sync to left scroll area
  int hValueRight = m_rightScroll->horizontalScrollBar()->value();
  int hMaxRight = m_rightScroll->horizontalScrollBar()->maximum();
  int hMaxLeft = m_leftScroll->horizontalScrollBar()->maximum();

  if (hMaxRight > 0 && hMaxLeft > 0) {
    double hRatio = (double)hValueRight / hMaxRight;
    m_leftScroll->horizontalScrollBar()->setValue((int)(hRatio * hMaxLeft));
  }

  int vValueRight = m_rightScroll->verticalScrollBar()->value();
  int vMaxRight = m_rightScroll->verticalScrollBar()->maximum();
  int vMaxLeft = m_leftScroll->verticalScrollBar()->maximum();

  if (vMaxRight > 0 && vMaxLeft > 0) {
    double vRatio = (double)vValueRight / vMaxRight;
    m_leftScroll->verticalScrollBar()->setValue((int)(vRatio * vMaxLeft));
  }

  m_updatingScrollBars = false;
}

void ImageComparisonWidget::onSyncCheckboxToggled(bool checked) {
  m_syncScrollBars = checked;
}

void ImageComparisonWidget::saveViewMode() {
  QSettings settings;
  settings.setValue("ImageComparison/viewMode",
                    m_isSideBySideView ? "sidebyside" : "slider");
}

void ImageComparisonWidget::loadViewMode() {
  QSettings settings;
  QString viewMode =
      settings.value("ImageComparison/viewMode", "sidebyside").toString();

  if (viewMode == "slider") {
    showOverlay();
  } else {
    showSideBySide();
  }
}

void ImageComparisonWidget::zoomIn() {
  applyZoom(m_zoomFactor * 1.25);
}

void ImageComparisonWidget::zoomOut() {
  applyZoom(m_zoomFactor / 1.25);
}

void ImageComparisonWidget::zoomReset() {
  applyZoom(1.0);
}

void ImageComparisonWidget::zoomFit() {
  if (m_originalPixmap.isNull()) {
    return;
  }

  QSize scrollSize;

  // Get viewport size based on current view
  if (m_isSideBySideView && m_leftScroll) {
    scrollSize = m_leftScroll->viewport()->size();
  } else if (!m_isSideBySideView && m_imageLabel && m_imageLabel->parentWidget()) {
    // For slider view, get the scroll area size
    QScrollArea *scrollArea = qobject_cast<QScrollArea*>(m_imageLabel->parentWidget()->parentWidget());
    if (scrollArea) {
      scrollSize = scrollArea->viewport()->size();
    } else {
      return;
    }
  } else {
    return;
  }

  // Calculate zoom to fit the image in the scroll area
  QSize imageSize = m_originalPixmap.size();

  double widthRatio = (double)scrollSize.width() / imageSize.width();
  double heightRatio = (double)scrollSize.height() / imageSize.height();
  double fitZoom = qMin(widthRatio, heightRatio);

  // Ensure we don't zoom in beyond 100% for fit
  if (fitZoom > 1.0) {
    fitZoom = 1.0;
  }

  applyZoom(fitZoom);
}

void ImageComparisonWidget::applyZoom(double factor) {
  // Clamp zoom factor between 10% and 500%
  if (factor < 0.1) {
    factor = 0.1;
  } else if (factor > 5.0) {
    factor = 5.0;
  }

  m_zoomFactor = factor;
  updateZoom();
}

void ImageComparisonWidget::updateZoom() {
  if (m_originalPixmap.isNull() || m_optimizedPixmap.isNull()) {
    return;
  }

  // Calculate scaled size
  QSize originalSize = m_originalPixmap.size();
  QSize scaledSize(originalSize.width() * m_zoomFactor,
                   originalSize.height() * m_zoomFactor);

  // Scale the pixmaps
  m_originalPixmapScaled = m_originalPixmap.scaled(
      scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  m_optimizedPixmapScaled = m_optimizedPixmap.scaled(
      scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  if (m_isSideBySideView) {
    // Store current scroll positions (as ratios)
    double hRatioLeft = 0.0;
    double vRatioLeft = 0.0;
    if (m_leftScroll->horizontalScrollBar()->maximum() > 0) {
      hRatioLeft = (double)m_leftScroll->horizontalScrollBar()->value() /
                   m_leftScroll->horizontalScrollBar()->maximum();
    }
    if (m_leftScroll->verticalScrollBar()->maximum() > 0) {
      vRatioLeft = (double)m_leftScroll->verticalScrollBar()->value() /
                   m_leftScroll->verticalScrollBar()->maximum();
    }

    // Update the labels
    m_leftLabel->setPixmap(m_originalPixmapScaled);
    m_rightLabel->setPixmap(m_optimizedPixmapScaled);

    // Restore scroll positions after a short delay to allow layout update
    QTimer::singleShot(10, this, [this, hRatioLeft, vRatioLeft]() {
      if (m_leftScroll->horizontalScrollBar()->maximum() > 0) {
        m_leftScroll->horizontalScrollBar()->setValue(
            hRatioLeft * m_leftScroll->horizontalScrollBar()->maximum());
      }
      if (m_leftScroll->verticalScrollBar()->maximum() > 0) {
        m_leftScroll->verticalScrollBar()->setValue(
            vRatioLeft * m_leftScroll->verticalScrollBar()->maximum());
      }
    });
  } else {
    // In slider view, update the composite image
    updateComparison(m_slider->value());
  }
}

