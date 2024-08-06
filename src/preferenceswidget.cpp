#include "preferenceswidget.h"
#include "ui_preferenceswidget.h"

#include <QAction>
#include <QIcon>

PreferencesWidget::PreferencesWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PreferencesWidget),
      m_settings(Settings::instance()) {
  ui->setupUi(this);

  // LOAD SETTINGS
  ui->outputDirPathLineEdit->setText(m_settings.getOptimizedPath());
  ui->optimizedFilePrefixLineEdit->setText(m_settings.getOutputFilePrefix());

  // UI
  QAction *changeOutputDirPath =
      new QAction(QIcon(":/resources/icons/folder-open-line.png"), "",
                  ui->outputDirPathLineEdit);
  changeOutputDirPath->setToolTip(tr("Change Output Directory Path"));
  ui->outputDirPathLineEdit->addAction(changeOutputDirPath,
                                       QLineEdit::TrailingPosition);

  // BINDINGS
}

PreferencesWidget::~PreferencesWidget() { delete ui; }
