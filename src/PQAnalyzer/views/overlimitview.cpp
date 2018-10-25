#include "overlimitview.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QBoxLayout>

#define DEFAULT_TAB_ROW 1
#define DEFALUT_TAB_DIR "steady_event/"

COverLimitView::COverLimitView(QWidget *parent)
    : QFrame(parent), m_pEventList(NULL)
    , m_pEventParams(NULL)
{
    _initCtrls();
    connect(this, SIGNAL(updateLimitData()), this, SLOT(on_update_limitData()));
}

COverLimitView::~COverLimitView()
{
    delete m_pEventList;
    delete m_pEventParams;
}

void COverLimitView::setRootPath(QString path)
{
    m_statRootPath = path;
    CSteadyInfoTool tool;
    QString filePath = m_statRootPath + DEFALUT_TAB_DIR;
    tool.ReadEventInfo(filePath.toStdString(), m_curEventInfos);

    CStatDataTool statTool;
    filePath = m_statRootPath + "stat.info";
    statTool.ReadInfoFromFile(filePath.toStdString(), m_curStatInfo);

    emit updateLimitData();
}

void COverLimitView::on_update_limitData()
{
    _defaultCtrls();
    if (m_curEventInfos.empty())
        return;
    for (int i = 0; i != (int)m_curEventInfos.size(); i++) {
        m_pEventList->insertRow(i);
        QString eventType = _getEventTypeStr(m_curEventInfos.at(i).eventType);
        QString statTime = m_curEventInfos.at(i).startTime.toDateTime().toString().c_str();
        QString endTime = m_curEventInfos.at(i).endTime.toDateTime().toString().c_str();
        if (m_curEventInfos.at(i).endTime.tv_sec == -1)
            endTime = "--";
        QString phase;
        if (m_curEventInfos.at(i).eventPhase == -1)
            phase = "--";
        else
            phase = 'A' + m_curEventInfos.at(i).eventPhase;
        QString harm;
        if (m_curEventInfos.at(i).eventHarmIdx == -1)
            harm = "--";
        else
            harm.sprintf("%d次谐波", m_curEventInfos.at(i).eventHarmIdx);
        QString limit;
        limit.sprintf("%.3f", m_curEventInfos.at(i).eventLimit);
        QString value;
        value.sprintf("%.3f", m_curEventInfos.at(i).eventVal);
        m_pEventList->setItem(i, 0, new QTableWidgetItem(eventType));
        m_pEventList->setItem(i, 1, new QTableWidgetItem(statTime));
        m_pEventList->setItem(i, 2, new QTableWidgetItem(endTime));
        m_pEventList->setItem(i, 3, new QTableWidgetItem(phase));
        m_pEventList->setItem(i, 4, new QTableWidgetItem(harm));
        m_pEventList->setItem(i, 5, new QTableWidgetItem(limit));
        m_pEventList->setItem(i, 6, new QTableWidgetItem(value));

        for (int j = 0; j != m_pEventList->columnCount(); j++)
            m_pEventList->item(i, j)->setTextAlignment(Qt::AlignCenter);
    }

    int idx = 0;
    QString tmp;
    m_pEventParams->insertRow(idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("标称电压"));
    tmp.sprintf("%.3f V", m_curStatInfo.meaCfg.voltNominal);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("电压等级"));
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(
        m_curStatInfo.meaCfg.VoltLevelToString(m_curStatInfo.meaCfg.voltLevel)));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("频率上限值"));
    tmp.sprintf("%.3f Hz", m_curStatInfo.meaCfg.freqUpLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("频率下限值"));
    tmp.sprintf("%.3f Hz", m_curStatInfo.meaCfg.freqDownLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("电压上限值"));
    tmp.sprintf("%.3f %%", m_curStatInfo.meaCfg.voltUpLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("电压下限值"));
    tmp.sprintf("%.3f %%", m_curStatInfo.meaCfg.voltDownLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("电压不平衡越限值"));
    tmp.sprintf("%.3f %%", m_curStatInfo.meaCfg.voltUnbaLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("短闪变限值"));
    tmp.sprintf("%.3f", m_curStatInfo.meaCfg.pstLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("长闪变限值"));
    tmp.sprintf("%.3f", m_curStatInfo.meaCfg.pltLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("电压THD限值"));
    tmp.sprintf("%.3f %%", m_curStatInfo.meaCfg.voltThdLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("电压奇次谐波限值"));
    tmp.sprintf("%.3f %%", m_curStatInfo.meaCfg.voltOddHarmLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    m_pEventParams->insertRow(++idx);
    m_pEventParams->setItem(idx, 0, new QTableWidgetItem("电压偶次谐波限值"));
    tmp.sprintf("%.3f %%", m_curStatInfo.meaCfg.voltEvenHarmLimit);
    m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));

    for (int i = 0; i != 24; i++) {
        m_pEventParams->insertRow(++idx);
        tmp.sprintf("电流%d次谐波限值", i + 2);
        m_pEventParams->setItem(idx, 0, new QTableWidgetItem(tmp));
        tmp.sprintf("%.3f A", m_curStatInfo.meaCfg.currHarmLimit[i]);
        m_pEventParams->setItem(idx, 1, new QTableWidgetItem(tmp));
    }

    for (int i = 0; i != m_pEventParams->rowCount(); i++) {
        for (int j = 0; j != m_pEventParams->columnCount(); j++)
            m_pEventParams->item(i, j)->setTextAlignment(Qt::AlignCenter);
    }
}

void COverLimitView::_initCtrls()
{
    m_pEventList = new QTableWidget(DEFAULT_TAB_ROW, 7, this);
    m_pEventList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pEventList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pEventList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pEventList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_pEventList->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    QStringList header;
    header << "事件类型" << "开始时间" << "结束时间" << "事件相序" << "谐波次数" << "越限阈值" << "越限值";
    m_pEventList->verticalHeader()->setHidden(true);
    m_pEventList->setHorizontalHeaderLabels(header);

    m_pEventParams = new QTableWidget(DEFAULT_TAB_ROW, 2, this);
    m_pEventParams->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pEventParams->setSelectionMode(QAbstractItemView::NoSelection);
    m_pEventParams->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pEventParams->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header.clear();
    header << "参数类型" << "参数内容";
    m_pEventParams->verticalHeader()->setHidden(true);
    m_pEventParams->setHorizontalHeaderLabels(header);

    _defaultCtrls();

    // 对页面控件进行布局
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pEventList, 8);
    mainLayout->addWidget(m_pEventParams, 2);
    setLayout(mainLayout);
}

void COverLimitView::_defaultCtrls()
{
    for (int i = m_pEventList->rowCount(); i >= 0; i--)
        m_pEventList->removeRow(i);
    for (int i = m_pEventParams->rowCount(); i >= 0; i--)
        m_pEventParams->removeRow(i);
}

QString COverLimitView::_getEventTypeStr(int type)
{
    switch (type) {
    case SE_FREQ_UP_LIMIT:
        return "频率上越限";
    case SE_FREQ_DOWN_LIMIT:
        return "频率下越限";
    case SE_VOLT_UP_LIMIT:
        return "电压上越限";
    case SE_VOLT_DOWN_LIMIT:
        return "电压下越限";
    case SE_VOLT_UNBALANCE_LIMIT:
        return "不平衡越限";
    case SE_PST_LIMIT:
        return "短闪变越限";
    case SE_PLT_LIMIT:
        return "长闪变越限";
    case SE_VOLT_THD_LIMIT:
        return "电压THD越限";
    case SE_VOLT_HARM_LIMIT:
        return "电压谐波越限";
    case SE_CURR_HARM_LIMIT:
        return "电流谐波越限";
    default:
        break;
    }
    return QString("");
}
