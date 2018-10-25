#include "basedataview.h"
#include "qcustomplot.h"
#include "statdatatool.h"
#include <QComboBox>

CBaseDataView::CBaseDataView(QWidget *parent)
    : CStatViewBase(parent)
{
//    m_mapFileName["frequency.dat"] = StatItemInfo("频率", "Hz");
//    m_mapFileName["rms_vol.dat"] = StatItemInfo("电压有效值", "V");
//    m_mapFileName["rms_cur.dat"] = StatItemInfo("电流有效值", "A");
//    m_mapFileName["ang_vol.dat"] = StatItemInfo("电压相位角", "°");
//    m_mapFileName["ang_cur.dat"] = StatItemInfo("电流相位角", "°");

//    QMap<QString, StatItemInfo>::iterator ite = m_mapFileName.begin();
//    for (; ite != m_mapFileName.end(); ++ite) {
//        m_pCombDataSel->addItem(ite.value().chinese);
//    }
//    m_pCombDataSel->setCurrentIndex(0);

    m_mapFileName.insert("frequency.dat",StatItemInfo("频率", "Hz"));
    m_mapFileName.insert("rms_vol.dat",StatItemInfo("电压有效值", "V"));
    m_mapFileName.insert("rms_cur.dat",StatItemInfo("电流有效值", "A"));
    m_mapFileName.insert("ang_vol.dat",StatItemInfo("电压相位角", "°"));
    m_mapFileName.insert("ang_cur.dat",StatItemInfo("电流相位角", "°"));
    m_vecBoxName.append("频率");
    m_vecBoxName.append("电压有效值");
    m_vecBoxName.append("电流有效值");
    m_vecBoxName.append("电压相位角");
    m_vecBoxName.append("电流相位角");
    for(int i=0;i<m_vecBoxName.size();i++)
    {
        m_pCombDataSel->addItem(m_vecBoxName.at(i));
    }
    m_pCombDataSel->setCurrentIndex(0);
}

CBaseDataView::~CBaseDataView()
{

}

//bool CBaseDataView::_getStatDataFromFile(QString statFilePath, StatDataPoints *points)
//{
//    // 从统计文件中读取所有统计数据
//    CStatDataTool tool;
//    StatRecordInfo info;
//    int idx = statFilePath.lastIndexOf('/');
//    QString infoFilePath = statFilePath.left(idx + 1) + "stat.info";
//    for (int i = 0; i != 4; i++)
//        points[i].clear();
//    // 先读取统计配置信息,获取起始时间以及统计数据数量
//    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
//        return false;
//    if (!tool.InitReadFile(statFilePath.toStdString(), _getStatPhaseCount(statFilePath)))
//        return false;
//    // 转化为显示的统计数据缓存起来
//    StatDatas dats;
//    if (!tool.ReadDataFromFile(0, info.statTotalCnt - 1, dats))
//        return false;
//    qDebug()<<"info.meaCfg.pt="<<info.meaCfg.pt;
//    float params = 1;
//    if (statFilePath.contains("rms_vol"))
//        params = info.meaCfg.pt/1000;
//    else if (statFilePath.contains("rms_cur"))
//        params = 1;//info.meaCfg.ct;

//    for (int i = 0; i != (int)dats.size(); i++) {
//        UtcTime curTime = info.firstTime.toUtcTime();
//        curTime.tv_sec += i * info.statCycle;
//        DateTime curDate = curTime.toDateTime();
//        QDateTime tmp(QDate(curDate.year, curDate.month, curDate.day),
//                      QTime(curDate.hour, curDate.minute, curDate.second, curDate.ms));
//        StatOnePoint point[4];
//        for (int j = 0; j != (int)dats.at(i).max_val.size(); j++)
//            point[0].y.push_back(dats.at(i).max_val.at(j) * params);
//        for (int j = 0; j != (int)dats.at(i).min_val.size(); j++)
//            point[1].y.push_back(dats.at(i).min_val.at(j) * params);
//        for (int j = 0; j != (int)dats.at(i).avg_val.size(); j++)
//            point[2].y.push_back(dats.at(i).avg_val.at(j) * params);
//        for (int j = 0; j != (int)dats.at(i).cp95_val.size(); j++)
//            point[3].y.push_back(dats.at(i).cp95_val.at(j) * params);
//        for (int j = 0; j != 4; j++) {
//            point[j].x = tmp.toTime_t();
//            points[j].push_back(point[j]);
//        }
//    }
//    return true;
//}
