#ifndef LOGTEXTEDIT_H
#define LOGTEXTEDIT_H

#include <QObject>

class LogText : public QObject
{
    Q_OBJECT
public:
    explicit LogText(QObject *parent = nullptr);

    static void getLog(const std::string& s);

signals:
    void addLog(const QString& s);

public slots:


private:
    void setSelf();

    inline static LogText* pThis_ = nullptr;
};

#endif // LOGTEXTEDIT_H
