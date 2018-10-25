#ifndef IVIEWBASE_H
#define IVIEWBASE_H

#include <QFrame>
#include "base_def.h"

class QCustomPlot;

class IViewBase
{
public:
    IViewBase() {}
    virtual ~IViewBase() {}
    virtual void setRootPath(QString path) = 0;

protected:
    virtual void _onPlotMouseWheel(QCustomPlot *plot, QWheelEvent *event);

protected:
    QString m_statRootPath; // 统计文件根目录
};

#endif // IVIEWBASE_H
