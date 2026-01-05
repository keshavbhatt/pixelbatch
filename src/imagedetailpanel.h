#ifndef IMAGEDETAILPANEL_H
#define IMAGEDETAILPANEL_H

#include "imagetask.h"
#include "settings.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
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

signals:
  void customOutputDirChanged(ImageTask *task, const QString &dir);
  void customOutputPrefixChanged(ImageTask *task, const QString &prefix);
  void customOutputDirCleared(ImageTask *task);
  void customOutputPrefixCleared(ImageTask *task);
  void customOptimizerSettingsChanged(ImageTask *task, const QVariantMap &settings);
  void customOptimizerSettingsCleared(ImageTask *task);

private slots:
  void onChangeOutputDir();
  void onUseCustomOutputToggled(bool checked);
  void onOutputPrefixChanged(const QString &text);
  void onSaveOptimizerSettings();
  void onResetOptimizerSettings();

private:
  void setupUI();
  void updateImagePreview();
  void updateImageInfo();
  void updateOptimizerSettings();
  void updateOutputSettings();
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
  QPushButton *m_saveOptimizerButton;
  QPushButton *m_resetOptimizerButton;
  QLabel *m_customSettingsIndicator;

  // Output settings
  QCheckBox *m_useCustomOutputCheckBox;
  QLineEdit *m_outputDirLineEdit;
  QLineEdit *m_outputPrefixLineEdit;
  QPushButton *m_changeOutputDirButton;

  QPixmap m_originalPixmap;
};

#endif // IMAGEDETAILPANEL_H

