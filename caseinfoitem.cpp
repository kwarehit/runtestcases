#include "caseinfoitem.h"

#include "log.h"
#include "configinfo.h"



CaseInfoItem::CaseInfoItem(QObject *parent)
    : QObject(parent)
{

}

void CaseInfoItem::loadData(const std::map<int, std::shared_ptr<CaseInfo>>& caseItems)
{
    caseData_.clear();
    for(auto& i : caseItems)
    {
        caseData_.append(std::make_shared<CaseInfoIemDetails>(
                          CaseInfoIemDetails{i.second->orderNO
                          , i.second->caseType.c_str()
                          , i.second->isRunning}
                      )
        );
    }
}

void CaseInfoItem::saveData(std::map<int, std::shared_ptr<CaseInfo>>& caseItems)
{
    std::list<std::shared_ptr<CaseInfo>> tmpCaseInfo;

    std::transform(std::begin(caseItems), std::end(caseItems), std::back_inserter(tmpCaseInfo)
        , [](auto& v) { return v.second; }
    );

    for(auto& val: tmpCaseInfo)
    {
        auto it = std::find_if(std::begin(caseData_), std::end(caseData_)
                               , [&](auto& v){ return val->caseType.c_str() == v->name;});
        if(it != std::end(caseData_))
        {
           val->orderNO = (*it)->orderNO;
           val->isRunning = (*it)->running;
        }
    }

    caseItems.clear();
    for(auto& v: tmpCaseInfo)
    {
        caseItems[v->orderNO] = v;
    }
}

void CaseInfoItem::changed(int sourceIndex, int destIndex)
{
    auto val = caseData_[sourceIndex];
    caseData_.removeAt(sourceIndex);

    caseData_.insert(destIndex, val);

    for(auto i = 0; i < caseData_.size(); ++i)
    {
        caseData_[i]->orderNO = i;
    }
}


bool CaseInfoItem::getCaseRunning(int row)
{
    return caseData_[row]->running;
}

void CaseInfoItem::setCaseRunning(int row, bool b)
{
    caseData_[row]->running = b;
}

int CaseInfoItem::getCaseOrderNO(int row)
{
    return caseData_[row]->orderNO;
}

QString CaseInfoItem::getCaseName(int row)
{
    return caseData_[row]->name;
}

int CaseInfoItem::rowCount()
{
    return caseData_.size();
}
