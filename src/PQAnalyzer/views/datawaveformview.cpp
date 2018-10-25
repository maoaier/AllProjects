#include "datawaveformview.h"

CDataWaveformView::CDataWaveformView(QWidget *parent)
   : QFrame(parent),m_pCustomPlot(NULL)
{
    m_iWaveNumber=3;
     _initCtrls();
     _initGraph();//初始化线条
     _getWavePoint();
}

CDataWaveformView::~CDataWaveformView()
{
    delete m_pCustomPlot;
    delete m_pLabDataSel;
    delete m_pCombDataSel;
    for (int i = 0; i != 4; i++)
        delete m_pChecPhaseType[i];
}
//鼠标滚动事件
void CDataWaveformView::on_plot_mouseWheel(QWheelEvent *event)
{
    _onPlotMouseWheel(m_pCustomPlot,event);
}

void CDataWaveformView::run()
{
    if(!m_statRootPath.isEmpty())
    {
        QString key;
        key.sprintf("%dharm_rms_vol.dat",1);
        QString statFilePath=m_statRootPath+key;
        qDebug()<<__FILE__<<__LINE__;
        _getStatDataFromFile(statFilePath, m_curWaveDataPoints);
    }
}

void CDataWaveformView::_getWavePoint()
{

    if(!m_curWaveDataPoints[0].isEmpty())
        m_curWaveDataPoints[0].clear();

    int samplingPoint=SAMPLING_RATE*m_iWaveNumber;//采样点等于采样率*周波数
    for(int i=0;i<samplingPoint;i++)
    {
       StatOnePoint onePoint;
       onePoint.x=i;
       onePoint.y.append(220*qSin(2*M_PI*i/SAMPLING_RATE+0/180.0*M_PI));
       onePoint.y.append(220*qSin(2*M_PI*i/SAMPLING_RATE-120/180.0*M_PI));
       onePoint.y.append(220*qSin(2*M_PI*i/SAMPLING_RATE+120/180.0*M_PI));
       onePoint.y.append(0);
       m_curWaveDataPoints[0].append(onePoint);
    }
    _plotWaveData(m_curWaveDataPoints[0]);
}

//画线条
void CDataWaveformView::_initGraph()
{
    QColor cols[] = {QColor(255,138,5), QColor(0,255,80),QColor(255,0,40), Qt::blue};
    for(int i=0;i<4;i++)
    {
        m_pCustomPlot->addGraph();
        m_pCustomPlot->graph(i)->setPen(cols[i]);
        m_pCustomPlot->graph(i)->setName(m_pChecPhaseType[i]->text());
        m_pCustomPlot->graph(i)->setLineStyle(QCPGraph::lsLine);
        m_pCustomPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
        m_pCustomPlot->graph(i)->setSelectable(false);
        m_pCustomPlot->graph(i)->setVisible(true);
    }
}

void CDataWaveformView::setRootPath(QString path)
{
    m_statRootPath = path;
}

//初始化数据
void CDataWaveformView::_initCtrls()
{

    m_pCustomPlot =new QCustomPlot(this);
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
    QString waveformType[]={"电压波形","电流波形"};
    for(int i=0;i<2;i++)
    {
           m_pCombDataSel->addItem(waveformType[i]);
           m_mapItemType.insert(waveformType[i],VOLTAGE_WAVEFORM+i);
    }
//    connect(m_pCombDataSel, SIGNAL(currentIndexChanged(int)), this, SLOT(on_datSel_combIndexChanged(int)));
    QString phaseType[] = {"A", "B", "C", "N"};
    for (int i = 0; i != 4; i++) {
        m_pChecPhaseType[i] = new QCheckBox(phaseType[i], this);
        m_pChecPhaseType[i]->setChecked(true);
        connect(m_pChecPhaseType[i], SIGNAL(stateChanged(int)), this, SLOT(on_phaseType_checkStateChanged(int)));
    }

    // 对页面控件进行布局
    QHBoxLayout *dataSelLayout = new QHBoxLayout;
    dataSelLayout->addWidget(m_pLabDataSel, 2);
    dataSelLayout->addWidget(m_pCombDataSel, 8);


    QHBoxLayout *phaseTypeLayout = new QHBoxLayout;
    for (int i = 0; i != 4; i++)
        phaseTypeLayout->addWidget(m_pChecPhaseType[i]);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addLayout(dataSelLayout);
    infoLayout->addLayout(phaseTypeLayout);
    infoLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pCustomPlot,8);
    mainLayout->addLayout(infoLayout, 2);
    setLayout(mainLayout);
}

//相位改变
void CDataWaveformView::on_phaseType_checkStateChanged(int /*state*/)
{
    for (int i = 0; i < m_pCustomPlot->graphCount(); i++) {
        m_pCustomPlot->graph(i)->setVisible(m_pChecPhaseType[i]->isChecked());
        m_pCustomPlot->replot();
    }
}


//获取数据从文件zhong
bool CDataWaveformView::_getStatDataFromFile(QString statFilePath, StatDataPoints *points)
{
    // 从统计文件中读取所有统计数据
    qDebug()<<__FILE__<<__LINE__;
    CStatDataTool tool;
    StatRecordInfo info;
    int idx = statFilePath.lastIndexOf('/');
    QString infoFilePath = statFilePath.left(idx + 1) + "stat.info";
    for (int i = 0; i != 2; i++)
        points[i].clear();
    // 先读取统计配置信息,获取起始时间以及统计数据数量
    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
        return false;
    if (!tool.InitReadFile(statFilePath.toStdString(),4))
        return false;
    // 转化为显示的统计数据缓存起来
    StatDatas dats;
    if (!tool.ReadDataFromFile(0, info.statTotalCnt - 1, dats))
        return false;
    for (int i = 0; i != (int)dats.size(); i++)
    {
        UtcTime curTime = info.firstTime.toUtcTime();
        curTime.tv_sec += i * info.statCycle;
        DateTime curDate = curTime.toDateTime();
        QDateTime tmp(QDate(curDate.year, curDate.month, curDate.day),
                      QTime(curDate.hour, curDate.minute, curDate.second, curDate.ms));
        StatOnePoint point;
        for (int j = 0; j != (int)dats.at(i).cp95_val.size(); j++)
            point.y.push_back(dats.at(i).cp95_val.at(j));

        point.x = tmp.toTime_t();
        points[0].push_back(point);
    }
    _plotWaveData(points[0]);
    return true;
}

//绘制波形数据
void CDataWaveformView::_plotWaveData(const StatDataPoints &points)
{
    QVector <double> x,waveLine[4];
    for(int i=0;i<points.size();i++)
    {
        x.append(m_curWaveDataPoints[0].at(i).x);
        waveLine[0].append(points.at(i).y.at(0));
        waveLine[1].append(points.at(i).y.at(1));
        waveLine[2].append(points.at(i).y.at(2));
        waveLine[3].append(points.at(i).y.at(3));
    }

    for(int i=0;i<4;i++)
    {
        m_pCustomPlot->graph(i)->setData(x,waveLine[i]);
    }
    m_pCustomPlot->graph(2)->rescaleAxes(false);
    m_pCustomPlot->graph(2)->rescaleAxes(true);
    m_pCustomPlot->replot();
}

