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
#include <QThread>
#include <imagetask.h>

struct ImageTask;
class ImageWorker : public QObject {
  Q_OBJECT

public:
  ImageWorker(QObject *parent = nullptr) : QObject(parent) {}
  virtual void optimize(ImageTask *task) = 0;
  virtual ~ImageWorker() {
    // Kill any running processes when worker is destroyed
    qDebug() << "ImageWorker destructor - terminating" << m_runningProcesses.count() << "processes";

    for (QProcess *process : m_runningProcesses) {
      if (process && process->state() != QProcess::NotRunning) {
        qDebug() << "Terminating process:" << process->program() << process->processId();

        // Try terminate first (SIGTERM - graceful)
        process->terminate();
        if (!process->waitForFinished(500)) {
          // If still running after 500ms, force kill (SIGKILL)
          qDebug() << "Process didn't terminate gracefully, forcing kill:" << process->processId();
          process->kill();
          process->waitForFinished(500);
        }

        // Double-check it's dead
        if (process->state() != QProcess::NotRunning) {
          qWarning() << "WARNING: Process still running after kill attempt:" << process->processId();
        } else {
          qDebug() << "Process successfully terminated:" << process->program();
        }
      }
    }

    // Delete all process objects immediately
    qDeleteAll(m_runningProcesses);
    m_runningProcesses.clear();

    qDebug() << "ImageWorker destructor complete";
  }

signals:
  void optimizationFinished(ImageTask *task, bool success);
  void optimizationError(ImageTask *task, const QString &errorString);

protected:
  QList<QProcess*> m_runningProcesses;  // Track all processes

  void executeProcess(const QString &program, const QStringList &arguments,
                      ImageTask *task) {
    QProcess *process = new QProcess(this);

    // Track this process
    m_runningProcesses.append(process);

    connect(
        process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this,
        [task, this, process](int exitCode, QProcess::ExitStatus exitStatus) {
          // Remove from tracking list
          m_runningProcesses.removeOne(process);

          if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            qDebug() << "Process finished successfully for" << task->imagePath;
            emit optimizationFinished(task, true);
          } else {
            qWarning().noquote() << "Process finished with error:";
            qWarning().noquote()
                << serializeProcessError(process, exitCode, exitStatus);
            emit optimizationError(task, "Process failed with exit code: " +
                                             QString::number(exitCode));
            QFile::remove(task->optimizedPath);
          }
          process->deleteLater();
        });

    connect(process, &QProcess::errorOccurred, this,
            [task, this, process](QProcess::ProcessError error) {
              // Remove from tracking list
              m_runningProcesses.removeOne(process);

              qWarning().noquote() << "Error occurred with the process:";
              qWarning().noquote()
                  << serializeProcessError(process, 500, QProcess::CrashExit);
              emit optimizationError(
                  task, "ErrorString: " + process->errorString() +
                            " ErrorCode: " + QString::number(error));
              QFile::remove(task->optimizedPath);
              process->deleteLater();
            });

    qDebug() << "Starting process for" << task->imagePath << "with program"
             << program << "and arguments" << arguments;
    process->start(program, arguments);

    if (!process->waitForStarted()) {
      qDebug() << "Process failed to start for" << task->imagePath;
      m_runningProcesses.removeOne(process);
      emit optimizationError(task, "Process failed to start");
      process->deleteLater();
    }
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
