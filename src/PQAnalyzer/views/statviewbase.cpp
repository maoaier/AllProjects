#include "statviewbase.h"
#include "qcustomplot.h"
#include "statdatatool.h"
#include <QBoxLayout>

CStatViewBase::CStatViewBase(QWidget *parent)
    : QFrame(parent), m_pCustomPlot(NULL)
    , m_pLabDataSel(NULL), m_pCombDataSel(NULL)
    , m_pLabDataType(NULL), m_pCombDataType(NULL)
{
    for (int i = 0; i != 5; i++)
        m_pChecPhaseType[i] = NULL;

    _initCtrls();

    connect(this, SIGNAL(updateCurStatData()), this, SLOT(on_update_cur_statdata()));
}

CStatViewBase::~CStatViewBase()
{
    delete m_pCustomPlot;
    delete m_pLabDataSel;
    delete m_pCombDataSel;
    delete m_pLabDataType;
    delete m_pCombDataType;
    for (int i = 0; i != 5; i++)
        delete m_pChecPhaseType[i];
}

void CStatViewBase::setRootPath(QString path)
{
    m_statRootPath = path;
    refreshStatData();
}

void CStatViewBase::refreshStatData()
{
    // 打开文件后设置页面为默认状态
    for (int i = 0; i != 5; i++) {
        m_pChecPhaseType[i]->setChecked(true);
    }
    if (m_pCombDataSel->currentIndex() != 0)
        m_pCombDataSel->setCurrentIndex(0);
    else
        _updateDiffStatData(0);
}

bool CStatViewBase::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        int datIdx = m_pCombDataType->currentIndex();
        if (m_curStatDataPoints[datIdx].empty())
            return true;
        // 计算鼠标在画图控件中的位置
        QPoint pos = ((QHelpEvent *)event)->pos();
        QRect rect_plot = m_pCustomPlot->geometry();
        QRect rect_axis = m_pCustomPlot->xAxis->axisRect()->rect();
        QPoint pos_axis(pos.x() - rect_plot.left(), pos.y() - rect_plot.top());
        if (!rect_axis.contains(pos_axis)) {
            QToolTip::hideText();
            return true;
        }
        // 确定要显示的数值标题以及数量信息
        StatGraphInfos infos;
        QString strInfos;
        QMap<QString, StatItemInfo>::iterator ite = m_mapFileName.begin();
        ite += m_pCombDataSel->currentIndex();
        int count = _getStatPhaseCount(ite.key());
        if (count < 0)
            return true;
        if (count == 1) {
            StatGraphInfo info;
            info.name = ite.value().chinese;
            info.unit = ite.value().unit;
            info.visiable = true;
            infos.push_back(info);
        }
        else {
            for (int i = 0; i != 5; i++) {
                StatGraphInfo info;
                info.name = ite.value().chinese + " " + m_pChecPhaseType[i]->text();
                info.unit = ite.value().unit;
                info.visiable = m_pChecPhaseType[i]->isChecked();
                infos.push_back(info);
            }
        }
        // 通过鼠标位置查找画图控件中的坐标值
        double val_x = m_pCustomPlot->xAxis->pixelToCoord(pos_axis.x());
        double val_y = m_pCustomPlot->yAxis->pixelToCoord(pos_axis.y());
        if (val_x < m_curStatDataPoints[datIdx].first().x || val_x > m_curStatDataPoints[datIdx].last().x) {
            QToolTip::hideText();
            return true;
        }
        // 当鼠标位置在画图控件中的点的5个像素之内时,才显示tooltip
        double onepix_x = m_pCustomPlot->xAxis->range().size()/rect_axis.width();
        double onepix_y = m_pCustomPlot->yAxis->range().size()/rect_axis.height();
        for (int i = 0; i != m_curStatDataPoints[datIdx].size(); i++) {
            double cur_x = m_curStatDataPoints[datIdx].at(i).x;
            if (qAbs(val_x - cur_x) > 5*onepix_x)
                continue;
            StatOnePoint curPoint = m_curStatDataPoints[datIdx].at(i);
            for (int j = 0; j != curPoint.y.size(); j++) {
                if (qAbs(val_y - curPoint.y.at(j)) < 5*onepix_y) {
                    strInfos = _getStatPointInfo(infos, curPoint);
                    QToolTip::showText(((QHelpEvent *)event)->globalPos(), strInfos);
                    return true;
                }
            }
        }
        QToolTip::hideText();
        return true;
    }
    return QWidget::event(event);
}

void CStatViewBase::_initCtrls()
{
    m_pCustomPlot = new QCustomPlot(this);
    m_pCustomPlot->legend->setVisible(true);
    m_pCustomPlot->legend->setBrush(QColor(255, 255, 255, 233));
    m_pCustomPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 11));
    m_pCustomPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 11));
    m_pCustomPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    m_pCustomPlot->xAxis->setDateTimeFormat("yyyy-MM-dd\nhh:mm:ss");
    m_pCustomPlot->axisRect()->setupFullAxesBox();
    m_pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    connect(m_pCustomPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(on_plot_mouseWheel(QWheelEvent*)));

    m_pLabDataSel = new QLabel("数据项:   ", this);
    m_pCombDataSel = new QComboBox(this);
    connect(m_pCombDataSel, SIGNAL(currentIndexChanged(int)), this, SLOT(on_datSel_combIndexChanged(int)));

    m_pLabDataType = new QLabel("数据类型:", this);
    m_pCombDataType = new QComboBox(this);
    m_pCombDataType->addItem("最大值");
    m_pCombDataType->addItem("最小值");
    m_pCombDataType->addItem("平均值");
    m_pCombDataType->addItem("CP95值");
    m_pCombDataType->setCurrentIndex(0);
    connect(m_pCombDataType, SIGNAL(currentIndexChanged(int)), this, SLOT(on_datType_combIndexChanged(int)));

    QString phaseType[] = {"A", "B", "C", "N", "SUM"};
    for (int i = 0; i != 5; i++) {
        m_pChecPhaseType[i] = new QCheckBox(phaseType[i], this);
        m_pChecPhaseType[i]->setChecked(true);
        connect(m_pChecPhaseType[i], SIGNAL(stateChanged(int)), this, SLOT(on_phaseType_checkStateChanged(int)));
    }

    // 对页面控件进行布局
    QHBoxLayout *dataSelLayout = new QHBoxLayout;
    dataSelLayout->addWidget(m_pLabDataSel, 2);
    dataSelLayout->addWidget(m_pCombDataSel, 8);

    QHBoxLayout *dataTypeLayout = new QHBoxLayout;
    dataTypeLayout->addWidget(m_pLabDataType, 2);
    dataTypeLayout->addWidget(m_pCombDataType, 8);

    QHBoxLayout *phaseTypeLayout = new QHBoxLayout;
    for (int i = 0; i != 5; i++)
        phaseTypeLayout->addWidget(m_pChecPhaseType[i]);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addLayout(dataSelLayout);
    infoLayout->addLayout(dataTypeLayout);
    infoLayout->addLayout(phaseTypeLayout);
    infoLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pCustomPlot, 8);
    mainLayout->addLayout(infoLayout, 2);
    setLayout(mainLayout);
}

void CStatViewBase::on_plot_mouseWheel(QWheelEvent *event)
{
    _onPlotMouseWheel(m_pCustomPlot, event);
}

void CStatViewBase::_updateDiffStatData(int index)
{
    index++;
    // 选择不同的数据项,重新将文件中的统计数据缓存起来
    QMap<QString, StatItemInfo>::iterator ite = m_mapFileName.begin();
    //通过文本框来查找需要导出的数据项
    for (; ite != m_mapFileName.end(); ++ite) {
        if(ite.value().chinese==m_pCombDataSel->currentText())
            break ;
    }
//    ite += index;
    QString statFilePath = m_statRootPath + ite.key();
    _getStatDataFromFile(statFilePath, m_curStatDataPoints);
    emit updateCurStatData();
}

void CStatViewBase::on_update_cur_statdata()
{
    // 更新当前数据项的数据
    StatGraphInfos infos;
    QMap<QString, StatItemInfo>::iterator ite = m_mapFileName.begin();
    ite += m_pCombDataSel->currentIndex();
    int count = _getStatPhaseCount(ite.key());
    if (count < 0)
        return;
    if (count == 1) {
        StatGraphInfo info;
        info.col = Qt::blue;
        info.name = ite.value().chinese;
        info.unit = ite.value().unit;
        info.visiable = true;
        infos.push_back(info);
        for (int i = 0; i != 5; i++)
            m_pChecPhaseType[i]->setVisible(false);
    }
    else {
        // ABCN,SUM,5个勾选框对应不同的颜色,以及不同的线条标题
        QColor cols[] = {QColor(200,200,0), QColor(0,255,80),QColor(255,0,40), Qt::blue, Qt::black};//修改**********//
        for (int i = 0; i != 5; i++) {
            if (i >= count) {
                m_pChecPhaseType[i]->setVisible(false);
                continue;
            }
            m_pChecPhaseType[i]->setVisible(true);
            StatGraphInfo info;
            info.col = cols[i];
            info.name = ite.value().chinese + " " + m_pChecPhaseType[i]->text();
            info.unit = ite.value().unit;
            info.visiable = m_pChecPhaseType[i]->isChecked();
            infos.push_back(info);
        }
    }
    _plotStatData(infos, m_curStatDataPoints[m_pCombDataType->currentIndex()]);
}

//从容器中读取数据，显示到QCustomPlot界面中，画线
void CStatViewBase::_plotStatData(const StatGraphInfos &infos, const StatDataPoints &points)
{
    // 更新画图控件的内容
    m_pCustomPlot->clearGraphs();
    if (points.empty()) {
        m_pCustomPlot->replot();
        return;
    }
    for (int i = 0; i != infos.size(); i++) {
        m_pCustomPlot->addGraph();
        m_pCustomPlot->graph(i)->setPen(infos.at(i).col);
        m_pCustomPlot->graph(i)->setName(infos.at(i).name);
        m_pCustomPlot->graph(i)->setLineStyle(QCPGraph::lsLine);
        m_pCustomPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
        m_pCustomPlot->graph(i)->setSelectable(false);
        m_pCustomPlot->graph(i)->setVisible(infos.at(i).visiable);
    }

    QVector<double> x;
    QVector<double> y[infos.size()];
    double max_y, min_y;
    for (int i = 0; i != points.size(); i++) {
        x.push_back(points.at(i).x);
        for (int j = 0; j != points.at(i).y.size(); j++) {
            if (j >= infos.size())
                continue;
            y[j].push_back(points.at(i).y.at(j));

            if (i == 0) {
                max_y = points.at(i).y.at(j);
                min_y = points.at(i).y.at(j);
            }
            max_y = qMax(max_y, points.at(i).y.at(j));
            min_y = qMin(min_y, points.at(i).y.at(j));
        }
    }
    for (int i = 0; i != infos.size(); i++) {
        m_pCustomPlot->graph(i)->setData(x, y[i]);
    }

    double max_x = x.last(), min_x = x.at(0);
    if (qAbs(max_y - min_y) < 10e-4) {
        max_y += 1;
        min_y -= 1;
    }
    m_pCustomPlot->xAxis->setRange(min_x - (max_x - min_x)*0.1, max_x + (max_x - min_x)*0.1);
    m_pCustomPlot->yAxis->setRange(min_y - (max_y - min_y)*0.1, max_y + (max_y - min_y)*0.1);
    m_pCustomPlot->replot();
}

int CStatViewBase::_getStatPhaseCount(QString statFileName)
{
    // 针对不同统计文件,统计不同的数据项,1-不区分abcn,4-abcn的数据,5-abcn以及总的数据
    QMap<QString, int> fileDatMap;
    fileDatMap["frequency"] = 1;
    fileDatMap["pos_value"] = 1;
    fileDatMap["pos_ang"] = 1;
    fileDatMap["neg_value"] = 1;
    fileDatMap["neg_ang"] = 1;
    fileDatMap["zero_value"] = 1;
    fileDatMap["zero_ang"] = 1;
    fileDatMap["neg_unbalance"] = 1;
    fileDatMap["zero_unbalance"] = 1;
    fileDatMap["active_power"] = 5;
    fileDatMap["reactive_power"] = 5;
    fileDatMap["apparent_power"] = 5;
    fileDatMap["power_factor"] = 5;
    fileDatMap["rms"] = 4;
    fileDatMap["ang"] = 4;
    fileDatMap["pos_peak"] = 4;
    fileDatMap["neg_peak"] = 4;
    fileDatMap["harm_dc_rms"] = 4;
    fileDatMap["harm_dc_per"] = 4;
    fileDatMap["harm_thd_rms"] = 4;
    fileDatMap["harm_thd_per"] = 4;
    fileDatMap["harm_power"] = 4;
    fileDatMap["harm_rms"] = 4;
    fileDatMap["harm_per"] = 4;
    fileDatMap["harm_ang"] = 4;
    fileDatMap["inter_harm_rms"] = 4;
    fileDatMap["inter_harm_per"] = 4;
    fileDatMap["high_harm_rms"] = 4;
    fileDatMap["high_harm_per"] = 4;
    fileDatMap["sflick"] = 4;
    fileDatMap["lflick"] = 4;
    for (QMap<QString, int>::iterator ite = fileDatMap.begin();
         ite != fileDatMap.end(); ++ite) {
        if (!statFileName.contains(ite.key()))
            continue;
        return ite.value();
    }
    return -1;
}

bool CStatViewBase::_getStatDataFromFile(QString statFilePath, StatDataPoints *points)
{
    // 从统计文件中读取所有统计数据
    CStatDataTool tool;
    StatRecordInfo info;
    int idx = statFilePath.lastIndexOf('/');
    QString infoFilePath = statFilePath.left(idx + 1) + "stat.info";
    for (int i = 0; i != 4; i++)
        points[i].clear();
    // 先读取统计配置信息,获取起始时间以及统计数据数量
    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
        return false;
    if (!tool.InitReadFile(statFilePath.toStdString(), _getStatPhaseCount(statFilePath)))
        return false;
    // 转化为显示的统计数据缓存起来
    StatDatas dats;
    if (!tool.ReadDataFromFile(0, info.statTotalCnt - 1, dats))
        return false;
    for (int i = 0; i != (int)dats.size(); i++) {
        UtcTime curTime = info.firstTime.toUtcTime();
        curTime.tv_sec += i * info.statCycle;
        DateTime curDate = curTime.toDateTime();
        QDateTime tmp(QDate(curDate.year, curDate.month, curDate.day),
                      QTime(curDate.hour, curDate.minute, curDate.second, curDate.ms));
        StatOnePoint point[4];
        for (int j = 0; j != (int)dats.at(i).max_val.size(); j++)
            point[0].y.push_back(dats.at(i).max_val.at(j));
        for (int j = 0; j != (int)dats.at(i).min_val.size(); j++)
            point[1].y.push_back(dats.at(i).min_val.at(j));
        for (int j = 0; j != (int)dats.at(i).avg_val.size(); j++)
            point[2].y.push_back(dats.at(i).avg_val.at(j));
        for (int j = 0; j != (int)dats.at(i).cp95_val.size(); j++)
            point[3].y.push_back(dats.at(i).cp95_val.at(j));
        for (int j = 0; j != 4; j++) {
            point[j].x = tmp.toTime_t();
            points[j].push_back(point[j]);
        }
    }
    return true;
}

void CStatViewBase::on_datSel_combIndexChanged(int index)
{
    _updateDiffStatData(index);
}

void CStatViewBase::on_datType_combIndexChanged(int /*index*/)
{
    emit updateCurStatData();
}

void CStatViewBase::on_phaseType_checkStateChanged(int /*state*/)
{
    for (int i = 0; i != m_pCustomPlot->graphCount(); i++) {
        if (i >= 5)
            continue;
        m_pCustomPlot->graph(i)->setVisible(m_pChecPhaseType[i]->isChecked());
        m_pCustomPlot->replot();
    }
}

QString CStatViewBase::_getStatPointInfo(const StatGraphInfos &infos, const StatOnePoint &point)
{
    QString pointInfos;
    for (int i = 0; i != infos.size(); i++) {
        if (!infos.at(i).visiable || i >= point.y.size())
            continue;
        QString tmp;
        tmp.sprintf("%s :\n    %.3f %s\n", infos.at(i).name.toStdString().c_str(),
            point.y.at(i), infos.at(i).unit.toStdString().c_str());
        pointInfos += tmp;
    }
    QDateTime pointTime = QDateTime::fromTime_t(point.x);
    pointInfos += "数据时标 :\n";
    pointInfos += pointTime.toString("    yyyy-MM-dd hh:mm:ss");
    return pointInfos;
}
