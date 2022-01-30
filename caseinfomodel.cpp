#include "caseinfomodel.h"


#include "log.h"


CaseInfoModel::CaseInfoModel(QObject *parent)
    : QAbstractListModel(parent)
{
    caseData_ = std::make_shared<CaseInfoItem>();
}

void CaseInfoModel::setDataManger(std::shared_ptr<DataManager> p)
{
    dataMgr_ = p;
    getDataMgr()->setDataItem(caseData_);
}

std::shared_ptr<DataManager> CaseInfoModel::getDataMgr()
{
    return dataMgr_.lock();
}

void CaseInfoModel::loadData()
{
    try
    {
        getDataMgr()->loadConfigFile();
        getDataMgr()->refineConfig();
        getDataMgr()->setData();
        update();
    }
    catch (std::exception& e)
    {
        BOOST_LOG(processLog::get()) << e.what() << std::endl;
    }
}

void CaseInfoModel::saveData()
{
    try
    {
        getDataMgr()->saveConfig();
        getDataMgr()->saveConfigFile();
    }
    catch (std::exception& e)
    {
        BOOST_LOG(processLog::get()) << e.what() << std::endl;
    }
}

void CaseInfoModel::updateData()
{
    try
    {
        clear();
        getDataMgr()->setData();
        update();
    }
    catch (std::exception& e)
    {
        BOOST_LOG(processLog::get()) << e.what() << std::endl;
    }
}

void CaseInfoModel::changed(int sourceIndex, int destIndex)
{
    try
    {
        caseData_->changed(sourceIndex, destIndex);
        clear();
        getDataMgr()->getData();
        update();
    }
    catch (std::exception& e)
    {
        BOOST_LOG(processLog::get()) << e.what() << std::endl;
    }
}

QHash<int, QByteArray> CaseInfoModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { CaseTypeRole, "caseType" },
        { CaseRunningRole, "caseRunning" }
    };

    return roles;
}

int CaseInfoModel::rowCount(const QModelIndex &) const
{
    if(!caseData_)
    {
        return 0;
    }

    return caseData_->rowCount();
}

QVariant CaseInfoModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
    {
        switch (role)
        {
        case CaseTypeRole:
            return QString("%1-%2").arg(caseData_->getCaseOrderNO(index.row()))
                                   .arg(caseData_->getCaseName(index.row()));

        case CaseRunningRole:
            return caseData_->getCaseRunning(index.row());

        default:
            return QVariant();
        }

    }
    return QVariant();
}

bool CaseInfoModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    try
    {
        if (idx.row() < rowCount())
        {
            switch (role)
            {
            case CaseRunningRole:
                caseData_->setCaseRunning(idx.row(), value.toBool());
                getDataMgr()->getData();

                Q_EMIT dataChanged(index(idx.row(), 0), index(idx.row(), 0));
                return true;
            }
        }
    }
    catch (std::exception& ec)
    {
        BOOST_LOG(processLog::get()) << ec.what() << std::endl;
    }

    return false;
}

void CaseInfoModel::update()
{
    if(rowCount() <= 0)
    {
        return;
    }

    beginInsertRows(QModelIndex(), 0, rowCount()-1);
    endInsertRows();
}

void CaseInfoModel::clear()
{
    if(rowCount() <= 0)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    endRemoveRows();
}
