#include "execlthread.h"
#include "statdatatool.h"
#include "mainwindow.h"
#include <QDesktopServices>

#define DEFAULT_VAL "--"

CExeclThread::CExeclThread(QObject *parent)
    : QThread(parent)
{
    m_pTool = new CExecl();
}

CExeclThread::~CExeclThread()
{
}

void CExeclThread::run()
{
    if (m_thdType == ExeInit) {
//        QString tempName = QDir::toNativeSeparators(QDir::currentPath() + "/res/template");
//        QString tempPath = tempName + ".xlsx";
//        QFileInfo info(tempPath);
//        if (!info.isFile())
//            m_pTool->createATemplate(tempName);
//        else
//            m_pTool->setTemplateName(tempName);
        ((MainWindow*)parent())->_initViewsData();
    }
    else if (m_thdType == ExeReport) {
        QString fileName;
        CVoltageQuality voltData;
        CCurrentQuality currData;
        CPowerQuality powData;
        _getWorkTabCont(voltData.workingCondition, fileName);
        currData.workingCondition = voltData.workingCondition;
        powData.workingCondition = voltData.workingCondition;
        _getVoltPageCont(voltData, 0, 30);
        _getCurrPageCont(currData, 30, 60);
        _getPowerPageCont(powData, 60, 90);
        QString repName = m_reportPath + "/" + fileName;
        repName = QDir::toNativeSeparators(repName);
        m_pTool->exportFormless(voltData, currData, powData, repName);
        emit sendTips("报表导出完成");
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_reportPath));
    }
    emit closedlg();
}

bool CExeclThread::_getWorkTabCont(CWorkingCondition &work, QString &fileName)
{
    CStatDataTool tool;
    StatRecordInfo info;
    QString infoFilePath = m_statRootPath + "stat.info";
    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
        return false;
    work.monitoingPsoition = info.meaCfg.measurePos;
    work.monitoingTime.sprintf("%s ~ %s", info.startTime.toString().c_str(),
                               info.endTime.toString().c_str());
    work.voltageLevel = info.meaCfg.VoltLevelToString(info.meaCfg.voltLevel);
    work.dataPT.sprintf("%.3f", info.meaCfg.pt);
    work.dataCT.sprintf("%.3f", info.meaCfg.ct);
    work.minimumShortCircuitCpy.sprintf("%.3f", info.meaCfg.miniCapacity);

    fileName.sprintf("%04d%02d%02d_%02d%02d%02d", info.startTime.year, info.startTime.month,
                     info.startTime.day, info.startTime.hour, info.startTime.minute, info.startTime.second);
    return true;
}

bool CExeclThread::_getVoltPageCont(CVoltageQuality &volt, int curPer, int maxPer)
{
    CStatDataTool tool;
    StatRecordInfo info;
    QString tip;
    QString infoFilePath = m_statRootPath + "stat.info";
    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
        return false;
    // 基波电压
    QString itemPath = m_statRootPath + "1harm_rms_vol.dat";
    float max[4], min[4], avg[4], cp95[4];
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i != 3; i++) {
        volt.voltageData[0][i][0].sprintf("%.3f", max[i]);
        volt.voltageData[0][i][1].sprintf("%.3f", avg[i]);
        volt.voltageData[0][i][2].sprintf("%.3f", min[i]);
        volt.voltageData[0][i][3].sprintf("%.3f", cp95[i]);
        volt.voltageData[0][i][4] = DEFAULT_VAL;
    }
    volt.voltageDataStandard[0] = DEFAULT_VAL;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)/3);
    emit sendTips(tip);
    // 谐波电压
    for (int i = 1; i != 25; i++) {
        float limit = (i % 2 == 1 ? info.meaCfg.voltEvenHarmLimit : info.meaCfg.voltOddHarmLimit);
        itemPath.sprintf("%s%dharm_per_vol.dat", m_statRootPath.toStdString().c_str(), i + 1);
        _getItemCont(itemPath, max, min, avg, cp95);
        for (int j = 0; j != 3; j++) {
            volt.voltageData[i][j][0].sprintf("%.3f", max[j]);
            volt.voltageData[i][j][1].sprintf("%.3f", avg[j]);
            volt.voltageData[i][j][2].sprintf("%.3f", min[j]);
            volt.voltageData[i][j][3].sprintf("%.3f", cp95[j]);
            volt.voltageData[i][j][4] = (cp95[j] > limit ? "不合格" : "合格");
        }
        volt.voltageDataStandard[i].sprintf("%.3f", limit);
    }
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)*2/3);
    emit sendTips(tip);
    // 总谐波畸变率
    itemPath = m_statRootPath + "harm_thd_per_vol.dat";
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i != 3; i++) {
        volt.voltageData[25][i][0].sprintf("%.3f", max[i]);
        volt.voltageData[25][i][1].sprintf("%.3f", avg[i]);
        volt.voltageData[25][i][2].sprintf("%.3f", min[i]);
        volt.voltageData[25][i][3].sprintf("%.3f", cp95[i]);
        volt.voltageData[25][i][4] = (cp95[i] > info.meaCfg.voltThdLimit ? "不合格" : "合格");
    }
    volt.voltageDataStandard[25].sprintf("%.3f", info.meaCfg.voltThdLimit);
    tip.sprintf("导出报表，进度%d%%", maxPer);
    emit sendTips(tip);
    return true;
}

bool CExeclThread::_getCurrPageCont(CCurrentQuality &curr, int curPer, int maxPer)
{
    CStatDataTool tool;
    StatRecordInfo info;
    QString tip;
    QString infoFilePath = m_statRootPath + "stat.info";
    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
        return false;
    // 基波电流
    QString itemPath = m_statRootPath + "1harm_rms_cur.dat";
    float max[4], min[4], avg[4], cp95[4];
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i != 3; i++) {
        curr.currentData[0][i][0].sprintf("%.3f", max[i]/* * info.meaCfg.ct*/);
        curr.currentData[0][i][1].sprintf("%.3f", avg[i]/* * info.meaCfg.ct*/);
        curr.currentData[0][i][2].sprintf("%.3f", min[i]/* * info.meaCfg.ct*/);
        curr.currentData[0][i][3].sprintf("%.3f", cp95[i]/* * info.meaCfg.ct*/);
        curr.currentData[0][i][4] = DEFAULT_VAL;
    }
    curr.currentDataStandard[0] = DEFAULT_VAL;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)/2);
    emit sendTips(tip);
    // 谐波电流
    for (int i = 1; i != 25; i++) {
        float limit = info.meaCfg.currHarmLimit[i - 1];
        itemPath.sprintf("%s%dharm_rms_cur.dat", m_statRootPath.toStdString().c_str(), i + 1);
        _getItemCont(itemPath, max, min, avg, cp95);
        for (int j = 0; j != 3; j++) {
            curr.currentData[i][j][0].sprintf("%.3f", max[j]/* * info.meaCfg.ct*/);
            curr.currentData[i][j][1].sprintf("%.3f", avg[j]/* * info.meaCfg.ct*/);
            curr.currentData[i][j][2].sprintf("%.3f", min[j]/* * info.meaCfg.ct*/);
            curr.currentData[i][j][3].sprintf("%.3f", cp95[j]/* * info.meaCfg.ct*/);
            curr.currentData[i][j][4] = (cp95[j] > limit ? "不合格" : "合格");
        }
        curr.currentDataStandard[i].sprintf("%.3f", limit);
    }
    tip.sprintf("导出报表，进度%d%%", maxPer);
    emit sendTips(tip);
    return true;
}

bool CExeclThread::_getPowerPageCont(CPowerQuality &pow, int curPer, int maxPer)
{
    CStatDataTool tool;
    StatRecordInfo info;
    QString tip;
    QString infoFilePath = m_statRootPath + "stat.info";
    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
        return false;
    int idx = 0;
    // 频率
    QString itemPath = m_statRootPath + "frequency.dat";
    float max[5], min[5], avg[5], cp95[5];
    _getItemCont(itemPath, max, min, avg, cp95);
    pow.powerData[0][0].sprintf("%.3f", max[0]);
    pow.powerData[0][1].sprintf("%.3f", avg[0]);
    pow.powerData[0][2].sprintf("%.3f", min[0]);
    pow.powerData[0][3].sprintf("%.3f", cp95[0]);
    pow.powerData[0][4].sprintf("%.3f/%.3f", info.meaCfg.freqUpLimit, info.meaCfg.freqDownLimit);
    pow.powerData[0][5].sprintf((cp95[0] > info.meaCfg.freqUpLimit || cp95[0] < info.meaCfg.freqDownLimit) ?
                "不合格" : "合格");
    idx++;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)/9);
    emit sendTips(tip);
    // 电压负序不平衡
    itemPath = m_statRootPath + "neg_unbalance_vol.dat";
    _getItemCont(itemPath, max, min, avg, cp95);
    pow.powerData[1][0].sprintf("%.3f", max[0]);
    pow.powerData[1][1].sprintf("%.3f", avg[0]);
    pow.powerData[1][2].sprintf("%.3f", min[0]);
    pow.powerData[1][3].sprintf("%.3f", cp95[0]);
    pow.powerData[1][4].sprintf("%.3f", info.meaCfg.voltUnbaLimit);
    pow.powerData[1][5].sprintf((cp95[0] > info.meaCfg.voltUnbaLimit) ?
                "不合格" : "合格");
    idx++;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)*2/9);
    emit sendTips(tip);
    // 短闪变
    itemPath = m_statRootPath + "sflick.dat";
    if (_getItemCont(itemPath, max, min, avg, cp95)) {
        float pstLimit = info.meaCfg.pstLimit;
        for (int i = 0; i != 3; i++) {
            pow.powerData[idx + i][0].sprintf("%.3f", max[i]);
            pow.powerData[idx + i][1].sprintf("%.3f", avg[i]);
            pow.powerData[idx + i][2].sprintf("%.3f", min[i]);
            pow.powerData[idx + i][3].sprintf("%.3f", cp95[i]);
            pow.powerData[idx + i][4].sprintf("%.3f", pstLimit);
            pow.powerData[idx + i][5].sprintf((cp95[i] > pstLimit) ? "不合格" : "合格");
        }
    }
    else {
        for (int i = 0; i != 3; i++) {
            pow.powerData[idx + i][0] = DEFAULT_VAL;
            pow.powerData[idx + i][1] = DEFAULT_VAL;
            pow.powerData[idx + i][2] = DEFAULT_VAL;
            pow.powerData[idx + i][3] = DEFAULT_VAL;
            pow.powerData[idx + i][4] = DEFAULT_VAL;
            pow.powerData[idx + i][5] = DEFAULT_VAL;
        }
    }
    idx += 3;
    // 长闪变
    itemPath = m_statRootPath + "lflick.dat";
    if (_getItemCont(itemPath, max, min, avg, cp95)) {
        float pltLimit = info.meaCfg.pltLimit;
        for (int i = 0; i != 3; i++) {
            pow.powerData[idx + i][0].sprintf("%.3f", max[i]);
            pow.powerData[idx + i][1].sprintf("%.3f", avg[i]);
            pow.powerData[idx + i][2].sprintf("%.3f", min[i]);
            pow.powerData[idx + i][3].sprintf("%.3f", cp95[i]);
            pow.powerData[idx + i][4].sprintf("%.3f", pltLimit);
            pow.powerData[idx + i][5].sprintf((cp95[i] > pltLimit) ? "不合格" : "合格");
        }
    }
    else {
        for (int i = 0; i != 3; i++) {
            pow.powerData[idx + i][0] = DEFAULT_VAL;
            pow.powerData[idx + i][1] = DEFAULT_VAL;
            pow.powerData[idx + i][2] = DEFAULT_VAL;
            pow.powerData[idx + i][3] = DEFAULT_VAL;
            pow.powerData[idx + i][4] = DEFAULT_VAL;
            pow.powerData[idx + i][5] = DEFAULT_VAL;
        }
    }
    idx += 3;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)*4/9);
    emit sendTips(tip);
    // 电压偏差
    itemPath = m_statRootPath + "rms_vol.dat";
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i != 3; i++) {
        float volNominal = info.meaCfg.voltNominal*info.meaCfg.pt;
        float upVoltLimit = volNominal*info.meaCfg.voltUpLimit/100;
        float downVoltLimit = volNominal*info.meaCfg.voltDownLimit/100;
        pow.powerData[idx + i][0].sprintf("%.3f", max[i]/volNominal*100 - 100);
        pow.powerData[idx + i][1].sprintf("%.3f", avg[i]/volNominal*100 - 100);
        pow.powerData[idx + i][2].sprintf("%.3f", min[i]/volNominal*100 - 100);
        pow.powerData[idx + i][3].sprintf("%.3f", cp95[i]/volNominal*100 - 100);
        pow.powerData[idx + i][4].sprintf("%.3f/%.3f", info.meaCfg.voltUpLimit - 100, info.meaCfg.voltDownLimit - 100);
        pow.powerData[idx + i][5].sprintf((cp95[i] > upVoltLimit || cp95[i] < downVoltLimit) ?
                    "不合格" : "合格");
    }
    idx += 3;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)*5/9);
    emit sendTips(tip);
    // 有功功率
    itemPath = m_statRootPath + "active_power.dat";
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i < 4; i++) {
        pow.powerData[idx + i][0].sprintf("%.3f", max[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][1].sprintf("%.3f", avg[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][2].sprintf("%.3f", min[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][3].sprintf("%.3f", cp95[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][4] = DEFAULT_VAL;
        pow.powerData[idx + i][5] = DEFAULT_VAL;
    }
    idx += 4;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)*6/9);
    emit sendTips(tip);
    // 无功功率
    itemPath = m_statRootPath + "reactive_power.dat";
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i < 4; i++) {
        pow.powerData[idx + i][0].sprintf("%.3f", max[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][1].sprintf("%.3f", avg[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][2].sprintf("%.3f", min[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][3].sprintf("%.3f", cp95[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][4] = DEFAULT_VAL;
        pow.powerData[idx + i][5] = DEFAULT_VAL;
    }
    idx += 4;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)*7/9);
    emit sendTips(tip);
    // 视在功率
    itemPath = m_statRootPath + "apparent_power.dat";
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i < 4; i++) {
        pow.powerData[idx + i][0].sprintf("%.3f", max[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][1].sprintf("%.3f", avg[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][2].sprintf("%.3f", min[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][3].sprintf("%.3f", cp95[i == 3 ? i + 1 : i]/* * info.meaCfg.pt * info.meaCfg.ct*/);
        pow.powerData[idx + i][4] = DEFAULT_VAL;
        pow.powerData[idx + i][5] = DEFAULT_VAL;
    }
    idx += 4;
    tip.sprintf("导出报表，进度%d%%", curPer + (maxPer - curPer)*8/9);
    emit sendTips(tip);
    // 功率因数
    itemPath = m_statRootPath + "power_factor.dat";
    _getItemCont(itemPath, max, min, avg, cp95);
    for (int i = 0; i < 4; i++) {
        pow.powerData[idx + i][0].sprintf("%.3f", max[i == 3 ? i + 1 : i]);
        pow.powerData[idx + i][1].sprintf("%.3f", avg[i == 3 ? i + 1 : i]);
        pow.powerData[idx + i][2].sprintf("%.3f", min[i == 3 ? i + 1 : i]);
        pow.powerData[idx + i][3].sprintf("%.3f", cp95[i == 3 ? i + 1 : i]);
        pow.powerData[idx + i][4] = DEFAULT_VAL;
        pow.powerData[idx + i][5] = DEFAULT_VAL;
    }
    idx += 4;
    tip.sprintf("导出报表，进度%d%%", maxPer);
    emit sendTips(tip);
    return true;
}

int CExeclThread::_getStatPhaseCount(QString statFileName)
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

bool CExeclThread::_getItemCont(QString filePath, float *max, float *min, float *avg, float *cp95)
{
    CStatDataTool tool;
    StatRecordInfo info;
    int idx = filePath.lastIndexOf('/');
    int phaseCnt = _getStatPhaseCount(filePath);
    QString infoFilePath = filePath.left(idx + 1) + "stat.info";
    // 先读取统计配置信息,获取起始时间以及统计数据数量
    if (!tool.ReadInfoFromFile(infoFilePath.toStdString(), info))
        return false;
    if (!tool.InitReadFile(filePath.toStdString(), phaseCnt))
        return false;
    StatDatas dats;
    if (!tool.ReadDataFromFile(0, info.statTotalCnt - 1, dats))
        return false;
    for (int i = 0; i != phaseCnt; i++) {
        QList<float> avgTmp, cp95Tmp;
        for (int j = 0; j != (int)dats.size(); j++) {
            avgTmp.push_back(dats.at(j).avg_val.at(i));
            cp95Tmp.push_back(dats.at(j).cp95_val.at(i));
            if (j == 0) {
                max[i] = dats.at(j).max_val.at(i);
                min[i] = dats.at(j).min_val.at(i);
                continue;
            }
            max[i] = qMax(max[i], dats.at(j).max_val.at(i));
            min[i] = qMin(min[i], dats.at(j).min_val.at(i));
        }
        avg[i] = 0;
        for (int j = 0; j != avgTmp.size(); j++)
            avg[i] += avgTmp.at(j);
        avg[i] /= avgTmp.size();

        qSort(cp95Tmp.begin(), cp95Tmp.end());
        int idx = cp95Tmp.size()*0.95;
        cp95[i] = cp95Tmp.at(idx);
    }
    return true;
}
