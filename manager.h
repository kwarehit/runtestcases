#pragma once


#include <QObject>

#include "commonhdr.h"

#include "monitorthread.h"


class CaseInfoModel;
class Monitor;
class IOContextWrapper;
class DataManager;
class LogText;
class MonitorThread;
class CaseInfoModel;

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);

    ~Manager();

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setDataMgr(CaseInfoModel* pModel);

    Q_INVOKABLE QString getRootDir();
    Q_INVOKABLE void setRootDir(const QString& rootDir);

    Q_INVOKABLE QString getCases();


signals:
    void setEnabledRunButton(bool b);
    void setEnabledStopButton(bool b);
    void addMonitorText(const QString& s);
    void addLogText(const QString& s);
    void stopProcess();

public slots:
    void onCaseTextChanged(const QString& text);

private slots:
    void onEnabledRunButton(bool b);
    void onEnabledStopButton(bool b);
    void onAddMonitorText(const QString& s);
    void onAddLogText(const QString& s);

private:
    std::shared_ptr<MonitorThread> thread_ = nullptr;
    std::shared_ptr<DataManager> dataMgr_ = nullptr;
    std::shared_ptr<IOContextWrapper> iocWrapper_ = nullptr;
    std::shared_ptr<Monitor> monitor_ = nullptr;
    std::shared_ptr<LogText> logText_ = nullptr;
    CaseInfoModel* model_ = nullptr;
};


