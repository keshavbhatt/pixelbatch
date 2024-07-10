#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include "imagetask.h"
#include "settings.h"

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QHeaderView>
#include <QImageReader>
#include <QMimeData>
#include <QTableWidget>

class TaskWidget : public QTableWidget {

  Q_OBJECT

public:
  explicit TaskWidget(QWidget *parent = nullptr);

public slots:
  void addFileToTable(const QString &filePath);
  void processImages();

signals:
  void setStatusRequested(const QString &message);
  void toggleShowStatusBarAddButton(const bool visible);
  void statusMessageUpdated(const QString &message);

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private slots:
  void onOptimizationFinished(const ImageTask &task, bool success);
  void onOptimizationError(const ImageTask &task, const QString &errorString);
  void updateStatusMessage();

private:
  QList<ImageTask> m_imageTasks;
  QLocale m_locale;
  Settings &m_settings;

  void updateTaskSizeAfter(const ImageTask &task, const QString text);
  void updateTaskWidgetHeader(const bool &contentLoaded = false);
  void updateTaskSaving(const ImageTask &task, const QString text);
  void updateTaskStatus(const ImageTask &task, const QString text,
                        const QString tooltip = "");
};

#endif // TASKWIDGET_H
