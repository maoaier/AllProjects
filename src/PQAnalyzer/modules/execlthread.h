#ifndef CEXECLTHREAD_H
#define CEXECLTHREAD_H

#include <QThread>
#include "execl.h"

class CExeclThread : public QThread
{
    Q_OBJECT
public:
    enum ExeThdType {
        ExeInit,
        ExeReport,
    };

    CExeclThread(QObject *parent);
    ~CExeclThread();
    void startFunc(int type) { m_thdType = type; start(); }
    void setRootPath(QString path) { m_statRootPath = path; }
    void setReportPath(QString path) { m_reportPath = path; }
    void run();

signals:
    bool closedlg();
    void sendTips(QString tips);

protected:
    bool _getWorkTabCont(CWorkingCondition &work, QString &fileName);
    bool _getVoltPageCont(CVoltageQuality &volt, int curPer, int maxPer);
    bool _getCurrPageCont(CCurrentQuality &curr, int curPer, int maxPer);
    bool _getPowerPageCont(CPowerQuality &pow, int curPer, int maxPer);

    bool _getItemCont(QString filePath, float *max, float *min, float *avg, float *cp95);
    int _getStatPhaseCount(QString statFileName);

protected:
    QString m_reportPath;
    QString m_statRootPath;
    int m_thdType;
    CExecl *m_pTool;
};

#endif // CEXECLTHREAD_H
