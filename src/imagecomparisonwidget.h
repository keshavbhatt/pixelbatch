#ifndef IMAGECOMPARISONWIDGET_H
#define IMAGECOMPARISONWIDGET_H

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QVBoxLayout>

class ImageComparisonWidget : public QDialog {
  Q_OBJECT

public:
  explicit ImageComparisonWidget(const QString &originalPath,
                                 const QString &optimizedPath,
                                 QWidget *parent = nullptr);
  ~ImageComparisonWidget();

private slots:
  void updateComparison(int value);
  void toggleView();
  void syncScrollBars();
  void onLeftScrollChanged();
  void onRightScrollChanged();
  void onSyncCheckboxToggled(bool checked);

private:
  void setupUI();
  void loadImages();
  void showSideBySide();
  void showOverlay();
  void saveViewMode();
  void loadViewMode();

  QString m_originalPath;
  QString m_optimizedPath;

  QPixmap m_originalPixmap;
  QPixmap m_optimizedPixmap;

  QLabel *m_imageLabel;
  QLabel *m_leftLabel;
  QLabel *m_rightLabel;
  QSlider *m_slider;
  QPushButton *m_toggleButton;
  QWidget *m_sideBySideWidget;
  QWidget *m_overlayWidget;
  QCheckBox *m_syncScrollCheckbox;
  QScrollArea *m_leftScroll;
  QScrollArea *m_rightScroll;

  bool m_isSideBySideView;
  bool m_syncScrollBars;
  bool m_updatingScrollBars;
};

#endif // IMAGECOMPARISONWIDGET_H

