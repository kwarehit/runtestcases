#include "datamanager.h"


#include "log.h"
#include "jsonmgr.h"
#include "refinecase.h"
#include "monitorthread.h"


DataManager::DataManager(QObject *parent)
    : QObject(parent)
{
}

void DataManager::setDefaultConfig()
{
    configFile_ = ConfigInfo{ "",
        {
            {"LRT"  ,  CaseStatus{0, true}  },
            {"STest",  CaseStatus{1, true}  },
            {"WTest",  CaseStatus{2, true}  },
            {"BBT"  ,  CaseStatus{3, true}  }
        },
        {}
    };
}

void DataManager::loadConfigFile()
{
    try
    {
        auto configPath = fs::current_path() / "conf.json";

        BOOST_LOG(processLog::get()) << "start try to load configure file: " << configPath << std::endl;

        if (!fs::exists(configPath))
        {
            setDefaultConfig();
            BOOST_LOG(processLog::get()) << "there is no configure file, will create it in later which path is " << configPath << std::endl;
        }
        else
        {
            JsonFile::loadJsonFile(configPath.string(), configFile_);
        }
    }
    catch (std::exception&)
    {
        throw;
    }
}

void DataManager::saveConfigFile()
{
    try
    {
        auto configPath = fs::current_path() / "conf.json";
        JsonFile::saveJsonFile(configPath.string(), configFile_);
    }
    catch (std::exception&)
    {
        throw;
    }
}


void DataManager::refineConfig()
{   
    try
    {
        caseCol_ = RefineCases::refineConfig(configFile_);
        if (configFile_.cases.empty())
        {
            BOOST_LOG(processLog::get()) << "No cases found" << std::endl;
        }
    }
    catch (std::exception&)
    {
        throw;
    }
}

void DataManager::saveConfig()
{
    try
    {
        configFile_ = RefineCases::saveConfig(caseCol_.value());
    }
    catch (std::exception&)
    {
        throw;
    }
}

void DataManager::setDataItem(std::shared_ptr<CaseInfoItem> data)
{
    dataItem_ = data;
}

std::string DataManager::getRootDir()
{
    try
    {
        return caseCol_.value().rootDir;
    }
    catch (std::exception&)
    {
        throw;
    }
}

void DataManager::setRootDir(const std::string rootDir)
{
    try
    {
        caseCol_.value().rootDir = rootDir;
    }
    catch (std::exception&)
    {
        throw;
    }
}

std::string DataManager::getCases()
{
    try
    {
        return boost::join(configFile_.cases, "\n\n");
    }
    catch (std::exception&)
    {
        throw;
    }
}

void DataManager::clearCases()
{
    try
    {
        return caseCol_.value().caseItems.clear();
    }
    catch (std::exception&)
    {
        throw;
    }
}

void DataManager::refine(const std::set<std::string>& cases)
{
    try
    {
        RefineCases::refine(cases, caseCol_.value().caseItems);
    }
    catch (std::exception&)
    {
        throw;
    }
}


void DataManager::setData()
{
    try
    {
        if(auto observe = dataItem_.lock())
        {
            observe->loadData(caseCol_.value().caseItems);
        }
    }
    catch (std::exception&)
    {
        throw;
    }
}

void DataManager::getData()
{
    try
    {
        if(auto observe = dataItem_.lock())
        {
            observe->saveData(caseCol_.value().caseItems);
        }
    }
    catch (std::exception&)
    {
        throw;
    }
}


void DataManager::setMonitorThread(std::shared_ptr<MonitorThread> monitorThread)
{
    monitorThread_ = monitorThread;
}

CaseCmd::cmdlist_type DataManager::getCmdList()
{
    try
    {
        if(auto ptr = monitorThread_.lock())
        {
            return ptr->getCaseCmdList(caseCol_);
        }
    }
    catch (std::exception&)
    {
        throw;
    }

    return {};
}

