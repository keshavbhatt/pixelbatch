#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QObject>
#include <imagetask.h>


struct ImageTask;
class ImageWorker : public QObject {
    Q_OBJECT

public:
    ImageWorker(QObject* parent = nullptr) : QObject(parent) {}
    virtual void optimize(const ImageTask &task) = 0;
    virtual ~ImageWorker() = default;

signals:
    void optimizationFinished(const ImageTask &task, bool success);
    void optimizationError(const ImageTask &task, const QString &errorString);

protected:
    void executeProcess(const QString &program, const QStringList &arguments, const ImageTask &task) {
        QProcess *process = new QProcess(this);

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
                    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
                    emit optimizationFinished(task, success);
                    process->deleteLater();
                });

        connect(process, &QProcess::errorOccurred,
                this, [=](QProcess::ProcessError error) {
                    emit optimizationError(task, "Process error: " + QString::number(error));
                    process->deleteLater();
                });

        process->start(program, arguments);
        // qDebug() << process->arguments();
    }
};

#endif // IMAGEWORKER_H
