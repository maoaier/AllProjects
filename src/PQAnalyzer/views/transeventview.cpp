#include "transeventview.h"
#include "qcustomplot.h"

#define DEFAULT_TAB_ROW 1
#define DEFALUT_TAB_DIR "trans_event/"

///////////////////////////////////////////////////////////////////////////////

CTransEventView::CTransEventView(QWidget *parent)
    : QFrame(parent), m_pEventInfo(NULL)
    , m_pEventList(NULL)
{
    for (int i = 0; i != 2; i++)
        m_pEventWave[i] = NULL;

    _initCtrls();

    connect(this, SIGNAL(updateTransData()), this, SLOT(on_update_transData()));
}

CTransEventView::~CTransEventView()
{
    delete m_pEventInfo;
    delete m_pEventList;
    for (int i = 0; i != 2; i++)
        delete m_pEventWave[i];
}

void CTransEventView::setRootPath(QString path)
{
    m_statRootPath = path;
    refreshTransData();
}

void CTransEventView::refreshTransData()
{
    CTransInfoTool tool;
    QString filePath = m_statRootPath + DEFALUT_TAB_DIR;
    tool.ReadEventInfo(filePath.toStdString(), m_curEventInfos);
    emit updateTransData();
}

void CTransEventView::on_update_transData()
{
    _defaultCtrls();
    if (m_curEventInfos.empty())
        return;
    for (int i = 0; i != (int)m_curEventInfos.size(); i++) {
        m_pEventList->insertRow(i);
        QString eventType = _getEventTypeStr(m_curEventInfos.at(i).eventType);
        QString eventTime = m_curEventInfos.at(i).eventStaTime.toDateTime().toString().c_str();
        m_pEventList->setItem(i, 0, new QTableWidgetItem(eventType));
        m_pEventList->setItem(i, 1, new QTableWidgetItem(eventTime));

        for (int j = 0; j != m_pEventList->columnCount(); j++)
            m_pEventList->item(i, j)->setTextAlignment(Qt::AlignCenter);
    }
}

void CTransEventView::_initCtrls()
{
    m_pEventList = new QTableWidget(DEFAULT_TAB_ROW, 2, this);
    m_pEventList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pEventList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pEventList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    m_pEventList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    QStringList header;
    header << "事件类型" << "发生时间";
    m_pEventList->verticalHeader()->setHidden(true);
    m_pEventList->setHorizontalHeaderLabels(header);
    connect(m_pEventList, SIGNAL(itemSelectionChanged()), this, SLOT(on_table_selection_change()));

    m_pEventInfo = new QTableWidget(DEFAULT_TAB_ROW, 2, this);
    m_pEventInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pEventInfo->setSelectionMode(QAbstractItemView::NoSelection);
    header.clear();
    header << "信息类型" << "信息内容";
    m_pEventInfo->verticalHeader()->setHidden(true);
    m_pEventInfo->setHorizontalHeaderLabels(header);
    m_pEventInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    m_pEventInfo->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    QStringList titles;
    titles << "电压波形" << "电流波形";
    for (int i = 0; i != 2; i++) {
        m_pEventWave[i] = new QCustomPlot(this);
        m_pEventWave[i]->legend->setVisible(true);
        m_pEventWave[i]->yAxis->setLabel(titles.at(i));
        m_pEventWave[i]->yAxis->setLabelFont(QFont(QFont().family(), 11));
        m_pEventWave[i]->xAxis->setTickLabelFont(QFont(QFont().family(), 11));
        m_pEventWave[i]->yAxis->setTickLabelFont(QFont(QFont().family(), 11));
        m_pEventWave[i]->xAxis->setTickLabelType(QCPAxis::ltDateTime);
        m_pEventWave[i]->xAxis->setDateTimeFormat("yyyy-MM-dd\nhh:mm:ss.zzz");
        m_pEventWave[i]->axisRect()->setupFullAxesBox();
        m_pEventWave[i]->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        connect(m_pEventWave[i], SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(on_plot_mouseWheel(QWheelEvent*)));
    }

    _defaultCtrls();

    // 对页面控件进行布局
    QVBoxLayout *waveLayout = new QVBoxLayout;
    waveLayout->addWidget(m_pEventWave[0]);
    waveLayout->addWidget(m_pEventWave[1]);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(m_pEventList);
    infoLayout->addWidget(m_pEventInfo);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(infoLayout, 2.5);
    mainLayout->addLayout(waveLayout, 7.5);
    setLayout(mainLayout);
}

QString CTransEventView::_getEventTypeStr(int type)
{
    switch (type) {
    case TE_VOLT_SWELL:
        return "电压暂升";
    case TE_VOLT_SAG:
        return "电压暂降";
    case TE_VOLT_INTERRUPT:
        return "电压中断";
    case TE_CURR_SHOCK:
        return "冲击电流";
    default:
        break;
    }
    return QString("");
}

void CTransEventView::on_table_selection_change()
{
    int idx = m_pEventList->currentRow();
    CTransInfoTool tool;
    TransEventWave wave;
    QString filePath = m_statRootPath + DEFALUT_TAB_DIR;
    for (int i = 0; i != 2; i++)
        m_waveDataPoints[i].clear();
    if (m_curEventInfos.empty())
        return;

    if (!tool.ReadEventWave(filePath.toStdString(), m_curEventInfos.at(idx), wave))
        return;
    for (int i = 0; i != (int)wave.size(); i++) {
        for (int j = 0; j != 2; j++) {
            StatOnePoint point;
            TransEventPoint wavePt = wave.at(i);
            DateTime curDate = wavePt.potTime.toDateTime();
            QDateTime tmp(QDate(curDate.year, curDate.month, curDate.day),
                          QTime(curDate.hour, curDate.minute, curDate.second, curDate.ms));
            point.x = tmp.toTime_t();
            point.x += wavePt.potTime.tv_usec/1000.0/1000.0;
            for (int k = 0; k != MAX_PHASE; k++) {
                point.y.push_back(wavePt.potWave[j][k]);
            }
            m_waveDataPoints[j].push_back(point);
        }
    }

    StatGraphInfos statInfos[2];
    QColor cols[] = {Qt::darkYellow, Qt::darkGreen, Qt::red, Qt::blue};
    QString names[] = {"A", "B", "C", "N"};
    for (int i = 0; i != 2; i++) {
        for (int j = 0; j != MAX_PHASE; j++) {
            StatGraphInfo info;
            info.col = cols[j];
            info.name = names[j];
            info.visiable = true;
            statInfos[i].push_back(info);
        }
    }

    for (int i = 0; i != 2; i++) {
        _updateStatData(m_pEventWave[i], statInfos[i], m_waveDataPoints[i]);
    }

    _updateEventInfo(m_pEventInfo, m_curEventInfos.at(idx));
}

void CTransEventView::_updateStatData(QCustomPlot* const plot, const StatGraphInfos &infos, const StatDataPoints &points)
{
    plot->clearGraphs();
    if (points.empty()) {
        plot->replot();
        return;
    }
    for (int i = 0; i != infos.size(); i++) {
        plot->addGraph();
        plot->graph(i)->setPen(infos.at(i).col);
        plot->graph(i)->setName(infos.at(i).name);
        plot->graph(i)->setLineStyle(QCPGraph::lsLine);
//        plot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
        plot->graph(i)->setSelectable(false);
        plot->graph(i)->setVisible(infos.at(i).visiable);
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
        plot->graph(i)->setData(x, y[i]);
    }

    double max_x = x.last(), min_x = x.at(0);
    plot->xAxis->setRange(min_x - (max_x - min_x)*0.1, max_x + (max_x - min_x)*0.1);
    plot->yAxis->setRange(min_y - (max_y - min_y)*0.1, max_y + (max_y - min_y)*0.1);
    plot->replot();
}

void CTransEventView::on_plot_mouseWheel(QWheelEvent *event)
{
    _onPlotMouseWheel(m_pEventWave[0], event);
    _onPlotMouseWheel(m_pEventWave[1], event);
}

void CTransEventView::_defaultCtrls()
{
    for (int i = m_pEventInfo->rowCount(); i >= 0; i--)
        m_pEventInfo->removeRow(i);
    for (int i = m_pEventList->rowCount(); i >= 0; i--)
        m_pEventList->removeRow(i);
    for (int i = 0; i != 2; i++) {
        m_pEventWave[i]->clearGraphs();
        m_pEventWave[i]->replot();
    }
}

void CTransEventView::_updateEventInfo(QTableWidget * const table, const TransEventInfo &info)
{
    QString tmp;
    QString phases[] = {"A", "B", "C"};
    for (int i = table->rowCount(); i >= 0; i--)
        table->removeRow(i);
    int idx = 0;
    table->insertRow(idx);
    table->setItem(idx, 0, new QTableWidgetItem("事件类型"));
    table->setItem(idx, 1, new QTableWidgetItem(_getEventTypeStr(info.eventType)));

    table->insertRow(++idx);
    table->setItem(idx, 0, new QTableWidgetItem("事件相序"));
    for (int i = 0; i != PHASE_N; i++) {
        if (!info.eventPhase[i])
            continue;
        tmp += phases[i];
        tmp += "  ";
    }
    table->setItem(idx, 1, new QTableWidgetItem(tmp));

    table->insertRow(++idx);
    table->setItem(idx, 0, new QTableWidgetItem("开始时间"));
    table->setItem(idx, 1, new QTableWidgetItem(info.eventStaTime.toDateTime().toString().c_str()));

    table->insertRow(++idx);
    table->setItem(idx, 0, new QTableWidgetItem("结束时间"));
    table->setItem(idx, 1, new QTableWidgetItem(info.eventEndTime.toDateTime().toString().c_str()));

    table->insertRow(++idx);
    table->setItem(idx, 0, new QTableWidgetItem("判断阈值"));
    tmp.sprintf("%.3f %c", info.eventLimit, info.eventType==TE_CURR_SHOCK ? 'A' : 'V');
    table->setItem(idx, 1, new QTableWidgetItem(tmp));

    for (int i = 0; i != 3; i++) {
        QString title;
        title.sprintf("特征幅值 %c", 'A'+i);
        table->insertRow(++idx);
        table->setItem(idx, 0, new QTableWidgetItem(title));
        tmp.sprintf("%.3f %c", info.eventVal[i], info.eventType==TE_CURR_SHOCK ? 'A' : 'V');
        table->setItem(idx, 1, new QTableWidgetItem(tmp));
    }

    for (int i = 0; i != table->rowCount(); i++) {
        for (int j = 0; j != table->columnCount(); j++)
            table->item(i, j)->setTextAlignment(Qt::AlignCenter);
    }
}
