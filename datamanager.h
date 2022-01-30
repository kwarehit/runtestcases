#pragma once

#include <QObject>

#include "commonhdr.h"

#include "caseinfo.h"
#include "configinfo.h"
#include "caseinfoitem.h"
#include "casecmd.h"

class MonitorThread;

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    void loadConfigFile();

    void saveConfigFile();

    void refineConfig();

    void saveConfig();

    std::string getRootDir();
    void setRootDir(const std::string rootDir);

    std::string getCases();

    void refine(const std::set<std::string>& cases);

    void setDataItem(std::shared_ptr<CaseInfoItem> data);
    void setMonitorThread(std::shared_ptr<MonitorThread> monitorThread);

    void setData();
    void getData();

    CaseCmd::cmdlist_type getCmdList();

    void clearCases();


signals:

public slots:

private:
    void setDefaultConfig();

    ConfigInfo configFile_;
    std::optional<CaseCollection> caseCol_;

    std::weak_ptr<CaseInfoItem> dataItem_;
    std::weak_ptr<MonitorThread> monitorThread_;
};

