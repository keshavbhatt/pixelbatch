#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <imagetask.h>

struct ImageTask;
class ImageWorker : public QObject {
  Q_OBJECT

public:
  ImageWorker(QObject *parent = nullptr) : QObject(parent) {}
  virtual void optimize(const ImageTask &task) = 0;
  virtual ~ImageWorker() = default;

signals:
  void optimizationFinished(const ImageTask &task, bool success);
  void optimizationError(const ImageTask &task, const QString &errorString);

protected:
  void executeProcess(const QString &program, const QStringList &arguments,
                      const ImageTask &task) {
    QProcess *process = new QProcess(this);

    connect(process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
              if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                emit optimizationFinished(task, true);
              } else {
                qWarning().noquote() << "Process finished with error:";
                qWarning().noquote()
                    << serializeProcessError(process, exitCode, exitStatus);

                emit optimizationError(task, "Process failed with exit code: " +
                                                 QString::number(exitCode));
              }
              process->deleteLater();
            });

    connect(process, &QProcess::errorOccurred, this,
            [=](QProcess::ProcessError error) {
              qWarning().noquote() << "Error ocurred with the process:";
              qWarning().noquote()
                  << serializeProcessError(process, 500, QProcess::CrashExit);
              emit optimizationError(
                  task, "ErrorString: " + process->errorString() +
                            " ErrorCode: " + QString::number(error));
              process->deleteLater();
            });

    process->start(program, arguments);
  }

  QByteArray serializeProcessError(QProcess *process, int exitCode,
                                   QProcess::ExitStatus exitStatus) {
    QJsonObject errorObject;
    errorObject["exitCode"] = exitCode;
    errorObject["exitStatus"] = static_cast<int>(exitStatus);
    errorObject["errorString"] = process->errorString();
    errorObject["program"] = process->program();
    errorObject["arguments"] = QJsonArray::fromStringList(process->arguments());

    QJsonDocument errorDoc(errorObject);
    return errorDoc.toJson(QJsonDocument::Indented);
  }
};

#endif // IMAGEWORKER_H
