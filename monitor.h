#pragma once

#include <QObject>

#include "commonhdr.h"

class IOContextWrapper;
class Monitor : public QObject
{
   Q_OBJECT

public:

    using buffer_type = std::string;

    explicit Monitor(IOContextWrapper& iocWrapper, QObject *parent = nullptr);

    void show(const std::string& s);

    void stop();

signals:
    void addText(const QString& s);

private:
    IOContextWrapper& iocWrapper_;
};

