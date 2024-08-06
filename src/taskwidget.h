#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include "imagetask.h"
#include "settings.h"
#include "taskactionwidget.h"
#include "taskwidgetoverlay.h"

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QHeaderView>
#include <QImageReader>
#include <QMimeData>
#include <QQueue>
#include <QTableWidget>

class TaskWidget : public QTableWidget {

  Q_OBJECT

public:
  explicit TaskWidget(QWidget *parent = nullptr);

public slots:
  void addFileToTable(const QString &filePath);
  void processImages();
  void removeFinishedOperations();
  void clearAllOperations();

signals:
  void setStatusRequested(const QString &message);
  void toggleShowStatusBarAddButton(const bool visible);
  void statusMessageUpdated(const QString &message);
  void selectionChangedCustom(QTableWidget *);
  void toggleShowTaskActionWidget(bool visible);

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected) override;

private slots:
  void onOptimizationFinished(ImageTask *task, bool success);
  void onOptimizationError(ImageTask *task, const QString &errorString);

private:
  QList<ImageTask *> m_imageTasks;

  TaskWidgetOverlay *m_overlayWidget;

  QLocale m_locale;
  Settings &m_settings;

  void updateTaskSizeAfter(ImageTask *task, const QString text);
  void updateTaskWidgetHeader(const bool &contentLoaded = false);
  void updateTaskSaving(ImageTask *task, const QString text);
  void updateTaskStatus(ImageTask *task, const QString optionalDetail = "");
  void processNextBatch();

  QQueue<ImageTask *> m_imageTaskQueue;
  int m_activeTasks = 0;
  void removeTask(ImageTask *task);
  int findRowByImageTask(ImageTask *task);
  void removeTasksByStatus(const ImageTask::Status &status);
  void updateStatusBarMessage(const QString &message);
  QString getSummaryStatus() const;

  void updateTaskOverlayWidget();
};

#endif // TASKWIDGET_H
