#ifndef COVERLIMITVIEW_H
#define COVERLIMITVIEW_H

#include "viewbase.h"
#include "statdatatool.h"
#include "eventdatatool.h"

class QTableWidget;

class COverLimitView : public QFrame, IViewBase
{
    Q_OBJECT

public:
    explicit COverLimitView(QWidget *parent = 0);
    virtual ~COverLimitView();
    virtual void setRootPath(QString path);

signals:
    void updateLimitData();

public slots:
    virtual void on_update_limitData();

protected:
    virtual void _initCtrls();
    virtual void _defaultCtrls();
    virtual QString _getEventTypeStr(int type);

protected:
    QTableWidget *m_pEventList;
    QTableWidget *m_pEventParams;

    StatRecordInfo m_curStatInfo;
    SteadyEventInfos m_curEventInfos;
};

#endif // COVERLIMITVIEW_H
