#include "monitorthread.h"

#include "log.h"
#include "datamanager.h"
#include "iocontextwrapper.h"


MonitorThread::MonitorThread(IOContextWrapper& ioWrapper
                             , Monitor& monitor
                             , std::shared_ptr<DataManager> dataMgr
                             , QObject *parent)
    : QThread(parent)
    , ioWrapper_(ioWrapper)
    , monitor_(monitor)
    , dataMgr_(dataMgr)
{
}

CaseCmd::cmdlist_type MonitorThread::getCaseCmdList(std::optional<CaseCollection>& caseCol)
{
    try
    {
        return CaseCmd::getCaseCmd(caseCol.value_or(CaseCollection{}));
    }
    catch (std::exception&)
    {
        throw;
    }
}

void MonitorThread::runCase()
{
    try
    {
        if(auto data = dataMgr_.lock())
        {
            auto lst = data->getCmdList();

            Q_EMIT enableRunButton(false);
            Q_EMIT enableStopButton(true);
            running_ = true;

            for (auto& c : lst)
            {
                if(!running_)
                {
                    continue;
                }

                BOOST_LOG(processLog::get()) << "start run following cases:\n\t" << boost::join(c.second, "\n\t") << std::endl;

                cmdProc_ = std::make_shared<CmdProcess<Monitor>>(ioWrapper_.getIOContext(), monitor_);
                cmdProc_->start(c.first, fs::path(data->getRootDir()));
                ioWrapper_.run();
            }

            if(lst.empty())
            {
                BOOST_LOG(processLog::get()) << "no cases need to be run" << std::endl;
            }
        }

        Q_EMIT enableRunButton(true);
        Q_EMIT enableStopButton(false);
        running_ = false;

        BOOST_LOG(processLog::get()) << "run cases end" << std::endl;
    }
    catch (std::exception& e)
    {
        Q_EMIT enableRunButton(true);
        Q_EMIT enableStopButton(false);
        running_ = false;

        cmdProc_->stop();

        BOOST_LOG(processLog::get()) << e.what() << std::endl;
    }
}

void MonitorThread::run()
{
    runCase();
}

void MonitorThread::onStopProcess()
{
    if(cmdProc_)
    {
        running_ = false;

        cmdProc_->stop();
    }
}
