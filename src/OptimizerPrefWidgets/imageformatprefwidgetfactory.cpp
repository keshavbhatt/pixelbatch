#include "imageformatprefwidgetfactory.h"
#include "gifsicleprefwidget.h"
#include "jpegoptimprefwidget.h"
#include "pngquantprefwidget.h"
#include "svgoprefwidget.h"
#include "worker/imageworkerfactory.h"
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSettings>
#include <QVBoxLayout>

// Event filter to save optimizer dialog size on close
class OptimizerDialogEventFilter : public QObject {
public:
  OptimizerDialogEventFilter(QScrollArea *parent, const QString &optimizerName)
      : QObject(parent), m_optimizerName(optimizerName), m_scrollArea(parent) {}

protected:
  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::Close && m_scrollArea) {
      // Save the current size before closing
      QSettings settings;
      QString sizeKey = QString("OptimizerDialog/%1/size").arg(m_optimizerName);
      settings.setValue(sizeKey, m_scrollArea->size());
      qDebug() << "Saved" << m_optimizerName << "dialog size:" << m_scrollArea->size();
    }
    return QObject::eventFilter(obj, event);
  }

private:
  QString m_optimizerName;
  QScrollArea *m_scrollArea;
};

ImageFormatPrefWidgetFactory &ImageFormatPrefWidgetFactory::instance() {
  static ImageFormatPrefWidgetFactory instance;
  return instance;
}

ImageFormatPrefWidgetFactory::ImageFormatPrefWidgetFactory() {
  registerOptimizers();
}

void ImageFormatPrefWidgetFactory::openPrefWidgetFor(
    const ImageOptimizer &imageOptimizer) {
  QString optimizerName = imageOptimizer.getName();

  if (optimizerWidgetMap.contains(optimizerName)) {
    ImageOptimizerPrefWidget *widget =
        optimizerWidgetMap[optimizerName](nullptr);
    if (widget) {
      // Wrap the widget in a QScrollArea to handle overflow on small screens
      QScrollArea *scrollArea = new QScrollArea();
      scrollArea->setWidget(widget);
      scrollArea->setWidgetResizable(true);
      scrollArea->setWindowTitle(optimizerName + " " + QObject::tr("Preferences"));
      scrollArea->setAttribute(Qt::WA_DeleteOnClose);

      widget->loadSettings();

      // Set a better initial size based on content
      widget->adjustSize();
      QSize widgetSize = widget->sizeHint();

      // Set minimum size to ensure usability
      int minWidth = 500;
      int minHeight = 400;

      // Calculate initial size (content size + some padding for scroll area)
      int initialWidth = qMax(minWidth, widgetSize.width() + 40);
      int initialHeight = qMax(minHeight, widgetSize.height() + 40);

      // Restore last user-set size from settings
      QSettings settings;
      QString sizeKey = QString("OptimizerDialog/%1/size").arg(optimizerName);
      QSize savedSize = settings.value(sizeKey, QSize(initialWidth, initialHeight)).toSize();

      // Apply the size (use saved size if available, otherwise use calculated initial size)
      scrollArea->resize(savedSize);

      // Save size when window is closed
      QObject::connect(scrollArea, &QScrollArea::destroyed, [optimizerName]() {
        // Note: We can't access scrollArea->size() here as it's being destroyed
        // The size is saved in closeEvent via eventFilter instead
      });

      // Install event filter to save size on close
      scrollArea->installEventFilter(new OptimizerDialogEventFilter(scrollArea, optimizerName));

      scrollArea->show();
      qDebug() << "Opened preference widget for" << optimizerName
               << "with size:" << scrollArea->size();
    }
  } else {
    qDebug() << "No preference widget registered for optimizer:"

             << optimizerName;
  }
}

QWidget *ImageFormatPrefWidgetFactory::createPrefWidgetFor(
    const ImageOptimizer &imageOptimizer, QWidget *parent, bool wrapInScrollArea) {
  QString optimizerName = imageOptimizer.getName();

  if (optimizerWidgetMap.contains(optimizerName)) {
    ImageOptimizerPrefWidget *widget =
        optimizerWidgetMap[optimizerName](parent);
    if (widget) {
      widget->loadSettings();
      qDebug() << "Created preference widget for" << optimizerName;

      if (wrapInScrollArea) {
        // Wrap the widget in a QScrollArea to handle overflow on small screens
        QScrollArea *scrollArea = new QScrollArea(parent);
        scrollArea->setWidget(widget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        return scrollArea;
      } else {
        // Return widget directly without scroll area wrapping
        return widget;
      }
    }
  } else {
    qDebug() << "No preference widget registered for optimizer:"
             << optimizerName;
  }

  return nullptr;
}

ImageOptimizerPrefWidget *ImageFormatPrefWidgetFactory::createOptimizerWidget(
    const ImageOptimizer &imageOptimizer, QWidget *parent) {
  QString optimizerName = imageOptimizer.getName();

  if (optimizerWidgetMap.contains(optimizerName)) {
    ImageOptimizerPrefWidget *widget =
        optimizerWidgetMap[optimizerName](parent);
    if (widget) {
      qDebug() << "Created optimizer widget for" << optimizerName;
      return widget;
    }
  } else {
    qDebug() << "No optimizer widget registered for:" << optimizerName;
  }

  return nullptr;
}

void ImageFormatPrefWidgetFactory::registerOptimizers() {
  QList<ImageOptimizer> registeredImageOptimizers =
      ImageWorkerFactory::instance().getRegisteredImageOptimizers();

  for (const ImageOptimizer &optimizer : qAsConst(registeredImageOptimizers)) {
    QString optimizerName = optimizer.getName();

    if (optimizerName == "Jpegoptim") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new JpegOptimPrefWidget(parent);
      };
    } else if (optimizerName == "Pngquant") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new PngQuantPrefWidget(parent);
      };
    } else if (optimizerName == "Gifsicle") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new GifsiclePrefWidget(parent);
      };
    } else if (optimizerName == "SVGO") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new SvgoPrefWidget(parent);
      };
    }
  }
}

bool ImageFormatPrefWidgetFactory::hasPrefWidgetFor(
    const ImageOptimizer &imageOptimizer) const {
  QString optimizerName = imageOptimizer.getName();
  return optimizerWidgetMap.contains(optimizerName);
}
