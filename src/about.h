#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>

namespace Ui {
class About;
}

class About : public QWidget {
  Q_OBJECT

public:
  explicit About(QWidget *parent = nullptr);
  ~About();

protected:
  void keyPressEvent(QKeyEvent *event) override;

private slots:
  void on_debugInfoButton_clicked();
  void onDonateClicked();
  void onReportIssueClicked();
  void onSourceCodeClicked();
  void onWebsiteClicked();

private:
  Ui::About *ui;

  void setupConnections();
  void setupUI();
  QString generateDebugInfo();

  // Application info
  QString appName;
  QString appDescription;
  QString appVersion;
  QString appSourceCodeLink;
  QString appIssuesLink;
  QString appWebsiteLink;
  QString appLicense;

  // Author info
  QString appAuthorName;

  // Links
  QString donateLink;
  QString githubLink;
};

#endif // ABOUT_H
