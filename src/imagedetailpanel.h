#ifndef IMAGEDETAILPANEL_H
#define IMAGEDETAILPANEL_H

#include "imagetask.h"
#include "settings.h"

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class ImageDetailPanel : public QWidget {
  Q_OBJECT

public:
  explicit ImageDetailPanel(QWidget *parent = nullptr);
  ~ImageDetailPanel();

  void setImageTask(ImageTask *task);
  void clear();


private:
  void setupUI();
  void updateImagePreview();
  void updateImageInfo();
  void updateOptimizerSettings();
  void loadOptimizerWidget();

  ImageTask *m_currentTask;
  Settings &m_settings;

  // UI components
  QLabel *m_previewLabel;
  QLabel *m_filenameLabel;
  QLabel *m_sizeLabel;
  QLabel *m_dimensionsLabel;
  QLabel *m_formatLabel;
  QVBoxLayout *m_optimizerSettingsLayout;
  QWidget *m_currentOptimizerWidget;


  QPixmap m_originalPixmap;
};

#endif // IMAGEDETAILPANEL_H

