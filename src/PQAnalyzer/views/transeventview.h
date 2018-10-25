#ifndef CTRANSEVENTVIEW_H
#define CTRANSEVENTVIEW_H

#include "viewbase.h"
#include "eventdatatool.h"

class QTableWidget;

class CTransEventView : public QFrame, IViewBase
{
    Q_OBJECT

public:
    explicit CTransEventView(QWidget *parent = 0);
    virtual ~CTransEventView();
    virtual void setRootPath(QString path);
    virtual void refreshTransData();

signals:
    void updateTransData();

public slots:
    virtual void on_table_selection_change();
    virtual void on_plot_mouseWheel(QWheelEvent *event);
    virtual void on_update_transData();

protected:
    virtual void _initCtrls();
    virtual QString _getEventTypeStr(int type);
    virtual void _updateStatData(QCustomPlot* const plot,
        const StatGraphInfos &infos, const StatDataPoints &points);
    virtual void _defaultCtrls();
    virtual void _updateEventInfo(QTableWidget* const table, const TransEventInfo &info);

protected:
    QTableWidget *m_pEventInfo;
    QTableWidget *m_pEventList;

    QCustomPlot *m_pEventWave[2];
    StatDataPoints m_waveDataPoints[2];

    TransEventInfos m_curEventInfos;
};

#endif // CTRANSEVENTVIEW_H
