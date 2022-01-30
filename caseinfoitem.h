#ifndef CASEINFOITEM_H
#define CASEINFOITEM_H

#include <memory>

#include <QObject>
#include <QMetaType>

#include "caseinfo.h"


struct CaseInfoIemDetails
{
    int orderNO;
    QString name;
    bool running;
};



class CaseInfoItem : public QObject
{
    Q_OBJECT
public:
    explicit CaseInfoItem(QObject *parent = nullptr);

    bool getCaseRunning(int row);
    void setCaseRunning(int row, bool b);
    int getCaseOrderNO(int row);
    QString getCaseName(int row);
    int rowCount();

    void loadData(const std::map<int, std::shared_ptr<CaseInfo>>& caseItems);
    void saveData(std::map<int, std::shared_ptr<CaseInfo>>& caseItems);

    void changed(int sourceIndex, int destIndex);

signals:

private:
    QList<std::shared_ptr<CaseInfoIemDetails>> caseData_;

};

#endif // CASEINFOITEM_H
