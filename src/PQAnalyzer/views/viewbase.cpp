#include "viewbase.h"
#include "qcustomplot.h"

void IViewBase::_onPlotMouseWheel(QCustomPlot *plot, QWheelEvent *event)
{
    // 当鼠标在x轴或y轴上滚动时,只对相应的轴进行缩放功能
    QPoint pos = event->pos();
    QRect rect_axis = plot->xAxis->axisRect()->rect();
    if (rect_axis.contains(pos)) {
        plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
        return;
    }

    if (pos.x() < rect_axis.left())
        plot->axisRect()->setRangeZoom(plot->yAxis->orientation());
    else if (pos.y() > rect_axis.bottom())
        plot->axisRect()->setRangeZoom(plot->xAxis->orientation());
}
