#include "monitor.h"

#include "iocontextwrapper.h"

Monitor::Monitor(IOContextWrapper& iocWrapper, QObject *parent)
    : QObject(parent)
    , iocWrapper_(iocWrapper)
{
}

void Monitor::show(const std::string& s)
{
    QString str(s.c_str());
    Q_EMIT addText(str);
}

void Monitor::stop()
{
    iocWrapper_.stop();
}




