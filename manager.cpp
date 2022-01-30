#include "manager.h"

#include "commonhdr.h"
#include "log.h"
#include "caseinfomodel.h"
#include "monitor.h"
#include "iocontextwrapper.h"
#include "datamanager.h"
#include "logtext.h"
#include "caseinfomodel.h"


Manager::Manager(QObject *parent)
    : QObject(parent)
{
    dataMgr_ = std::make_shared<DataManager>();
    logText_ = std::make_shared<LogText>();
    iocWrapper_ = std::make_shared<IOContextWrapper>();
    monitor_ = std::make_shared<Monitor>(*iocWrapper_);
    thread_ = std::make_shared<MonitorThread>(*iocWrapper_, *monitor_, dataMgr_, this);
    dataMgr_->setMonitorThread(thread_);

    connect(logText_.get(), &LogText::addLog, this, &Manager::onAddLogText);
    connect(monitor_.get(), &Monitor::addText, this, &Manager::onAddMonitorText);
    connect(thread_.get(), &MonitorThread::enableRunButton, this, &Manager::onEnabledRunButton);
    connect(thread_.get(), &MonitorThread::enableStopButton, this, &Manager::onEnabledStopButton);
    connect(this, &Manager::stopProcess, thread_.get(), &MonitorThread::onStopProcess);
}

Manager::~Manager()
{
}

void Manager::setDataMgr(CaseInfoModel* pModel)
{
    model_ = pModel;
    pModel->setDataManger(dataMgr_);
}

void Manager::onAddMonitorText(const QString& s)
{
    Q_EMIT addMonitorText(s);
}

void Manager::onEnabledRunButton(bool b)
{
    Q_EMIT setEnabledRunButton(b);
}

void Manager::onEnabledStopButton(bool b)
{
    Q_EMIT setEnabledStopButton(b);
}

void Manager::onAddLogText(const QString& s)
{
    Q_EMIT addLogText(s);
}

QString Manager::getRootDir()
{
    try
    {
        return QString::fromStdString(dataMgr_->getRootDir());
    }
    catch (std::exception& e)
    {
        BOOST_LOG(processLog::get()) << e.what() << std::endl;
        return "";
    }
}

void Manager::setRootDir(const QString& rootDir)
{
    try
    {
        dataMgr_->setRootDir(rootDir.toStdString());
    }
    catch (std::exception& e)
    {
        BOOST_LOG(processLog::get()) << e.what() << std::endl;
    }
}

QString Manager::getCases()
{
    try
    {
        return QString::fromStdString(dataMgr_->getCases());
    }
    catch (std::exception& e)
    {
        BOOST_LOG(processLog::get()) << e.what() << std::endl;
    }

    return {};
}

void Manager::onCaseTextChanged(const QString& text)
{
    try
    {
        auto listStr = text.split('\n', QString::SkipEmptyParts);

        std::set<std::string> cases;
        for(auto i = 0; i<listStr.size(); ++i)
        {
            QString s = listStr[i].trimmed();

            cases.insert(s.toStdString());
        }
        
        dataMgr_->clearCases();
        dataMgr_->refine(cases);

        if(model_)
        {
            model_->updateData();
        }
    }
    catch (std::exception& ec)
    {
        BOOST_LOG(processLog::get()) << ec.what() << std::endl;
    }
}

void Manager::start()
{
    thread_->start();
}

void Manager::stop()
{
    Q_EMIT stopProcess();
}








