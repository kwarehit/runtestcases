#include "logtext.h"
#include "log.h"


LogText::LogText(QObject *parent)
    : QObject(parent)
{
    setSelf();
    InitLog(&LogText::getLog);
}

void LogText::setSelf()
{
    pThis_ = this;
}

void LogText::getLog(const std::string& s)
{
    if(pThis_)
    {
        QString str(s.c_str());
        Q_EMIT pThis_->addLog(str);
    }
}


