#include "flickdataview.h"
#include "module_base_def.h"
#include "statdatatool.h"
#include <QComboBox>
#include <QDateTime>

CFlickDataView::CFlickDataView(QWidget *parent)
    : CStatViewBase(parent)
{
    m_mapFileName["sflick.dat"] = StatItemInfo("短闪变", "");
    m_mapFileName["lflick.dat"] = StatItemInfo("长闪变", "");

    QMap<QString, StatItemInfo>::iterator ite = m_mapFileName.begin();
    for (; ite != m_mapFileName.end(); ++ite) {
        m_pCombDataSel->addItem(ite.value().chinese);
    }
    m_pCombDataSel->setCurrentIndex(0);
}


bool CFlickDataView::_getStatDataFromFile(QString statFilePath, StatDataPoints *points)
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
    // 计算闪变起始时间
    UtcTime curTime = info.firstTime.toUtcTime();
    int stepTime = info.statCycle;
    if (statFilePath.contains("sflick"))
        stepTime = 60 * 10;
    else if (statFilePath.contains("lflick"))
        stepTime = 60 * 60 * 2;
    int tmp = curTime.tv_sec % stepTime;
    curTime.tv_sec += stepTime - tmp;
    for (int i = 0; i != (int)dats.size(); i++) {
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
        curTime.tv_sec += stepTime;
    }
    return true;
}
