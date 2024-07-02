#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include "imagetask.h"

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

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private slots:
  void onOptimizationFinished(const ImageTask &task, bool success);
  void onOptimizationError(const ImageTask &task, const QString &errorString);

private:
  void updateTaskWidgetHeader(const bool &contentLoaded = false);

  std::vector<QString> images;
  std::unordered_map<QString, int> fileRowMap; // Maps file names to row indices
  QLocale m_locale;
};

#endif // TASKWIDGET_H
