#include "harmdataview.h"
#include "module_base_def.h"
#include "qcustomplot.h"
#include "statdatatool.h"
#include <QComboBox>

CHarmDataView::CHarmDataView(QWidget *parent)
    : CStatViewBase(parent)
{
 /*
        for (int i = 0; i != HARM_MAX; i++) {
        QString chn, key;
        key.sprintf("%dharm_rms_vol.dat", i + 1);
        chn.sprintf("%d次电压谐波幅值", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "V");

        key.sprintf("%dharm_rms_cur.dat", i + 1);
        chn.sprintf("%d次电流谐波幅值", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "A");

        key.sprintf("%dharm_ang_vol.dat", i + 1);
        chn.sprintf("%d次电压谐波相位", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "°");

        key.sprintf("%dharm_ang_cur.dat", i + 1);
        chn.sprintf("%d次电流谐波相位", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "°");

        key.sprintf("%dharm_per_vol.dat", i + 1);
        chn.sprintf("%d次电压谐波含有率", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "%");

        key.sprintf("%dharm_per_cur.dat", i + 1);
        chn.sprintf("%d次电流谐波含有率", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "%");

        key = "harm_thd_per_vol.dat";
        chn = "电压总谐波畸变率";
        m_mapFileName[key] = StatItemInfo(chn, "%");

        key = "harm_thd_per_cur.dat";
        chn = "电流总谐波畸变率";
        m_mapFileName[key] = StatItemInfo(chn, "%");
    }

    QMap<QString, StatItemInfo>::iterator ite = m_mapFileName.begin();
    for (; ite != m_mapFileName.end(); ++ite) {
        m_pCombDataSel->addItem(ite.value().chinese);
    }
    m_pCombDataSel->setCurrentIndex(0);
*/

//修改，使下拉框排序
    QString chn, key;
    for (int i = 0; i != HARM_MAX; i++)
    {

        key.sprintf("%dharm_rms_vol.dat", i + 1);
        chn.sprintf("%d次电压谐波幅值", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "V");
        m_vecBoxName.append(chn);

        key.sprintf("%dharm_rms_cur.dat", i + 1);
        chn.sprintf("%d次电流谐波幅值", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "A");
        m_vecBoxName.append(chn);

        key.sprintf("%dharm_ang_vol.dat", i + 1);
        chn.sprintf("%d次电压谐波相位", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "°");
        m_vecBoxName.append(chn);

        key.sprintf("%dharm_ang_cur.dat", i + 1);
        chn.sprintf("%d次电流谐波相位", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "°");
        m_vecBoxName.append(chn);

        key.sprintf("%dharm_per_vol.dat", i + 1);
        chn.sprintf("%d次电压谐波含有率", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "%");
        m_vecBoxName.append(chn);

        key.sprintf("%dharm_per_cur.dat", i + 1);
        chn.sprintf("%d次电流谐波含有率", i + 1);
        m_mapFileName[key] = StatItemInfo(chn, "%");
        m_vecBoxName.append(chn);
    }
    key = "harm_thd_per_vol.dat";
    chn = "电压总谐波畸变率";
    m_mapFileName[key] = StatItemInfo(chn, "%");
    m_vecBoxName.append(chn);

    key = "harm_thd_per_cur.dat";
    chn = "电流总谐波畸变率";
    m_mapFileName[key] = StatItemInfo(chn, "%");
    m_vecBoxName.append(chn);

    for(int i=0;i<m_vecBoxName.size();i++)
    {
        m_pCombDataSel->addItem(m_vecBoxName.at(i));
    }
    m_pCombDataSel->setCurrentIndex(0);



}

CHarmDataView::~CHarmDataView()
{

}

//bool CHarmDataView::_getStatDataFromFile(QString statFilePath, StatDataPoints *points)
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

//    float params = 1;
//    if (statFilePath.contains("harm_rms_cur"))
//        params = info.meaCfg.ct;

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
