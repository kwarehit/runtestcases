#pragma once

#include "commonhdr.h"

#include <QThread>

#include "caseinfo.h"
#include "cmdprocess.h"
#include "monitor.h"
#include "casecmd.h"


class IOContextWrapper;
class DataManager;

class MonitorThread : public QThread
{
    Q_OBJECT
public:
    explicit MonitorThread(IOContextWrapper& ioWrapper
            , Monitor& monitor
            , std::shared_ptr<DataManager> dataMgr
            , QObject *parent = nullptr);

    void run();

    CaseCmd::cmdlist_type getCaseCmdList(std::optional<CaseCollection>&);

public slots:
    void onStopProcess();

signals:
    void enableRunButton(bool b);
    void enableStopButton(bool b);

private:
    void runCase();

    IOContextWrapper& ioWrapper_;
    Monitor& monitor_;
    std::weak_ptr<DataManager> dataMgr_;
    std::shared_ptr<CmdProcess<Monitor>> cmdProc_ = nullptr;
    bool running_ = false;
};


