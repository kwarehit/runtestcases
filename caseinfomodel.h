#ifndef CASEINFOMODEL_H
#define CASEINFOMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "caseinfoitem.h"
#include "datamanager.h"


class CaseInfoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CaseInfoModel(QObject *parent = nullptr);

    enum  CaseInfoRole {
        CaseTypeRole = Qt::DisplayRole,
        CaseRunningRole = Qt::UserRole
    };
    Q_ENUM(CaseInfoRole)

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    void setDataManger(std::shared_ptr<DataManager> p);

    Q_INVOKABLE void changed(int sourceIndex, int destIndex);

    Q_INVOKABLE void loadData();
    Q_INVOKABLE void saveData();

    void updateData();

signals:

protected:

    void update();
    void clear();

    std::shared_ptr<DataManager> getDataMgr();

private:
    std::shared_ptr<CaseInfoItem> caseData_ = nullptr;
    std::weak_ptr<DataManager> dataMgr_;

};

#endif // CASEINFOMODEL_H
