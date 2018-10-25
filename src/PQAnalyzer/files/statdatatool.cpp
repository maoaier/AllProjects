#include "statdatatool.h"
#include "cJSON.h"
#include <QFile>
#include <iostream>
#include <stdlib.h>
#include <QDebug>

using namespace std;

CStatDataTool::CStatDataTool()
{
    m_datCount = 0;
    m_funcType = Stat_Func_None;
}

CStatDataTool::~CStatDataTool()
{

}

bool CStatDataTool::InitWriteFile(const std::string &fileName)
{
    m_datFileName = fileName;
    m_datCount = 0;
    m_funcType = Stat_Func_Write;
    return true;
}

bool CStatDataTool::WriteDataToFile(const StatDatas &dat)
{
    if (m_funcType != Stat_Func_Write)
        return false;
    FILE *stafile = fopen(m_datFileName.c_str(), "ab+");
    if (stafile == NULL)
        return false;

    for (uint i = 0; i != dat.size(); i++) {
        StatOneData one = dat.at(i);
        uint datCnt = one.max_val.size();

        if (datCnt == one.min_val.size() &&
            datCnt == one.avg_val.size() &&
            datCnt == one.cp95_val.size()) {
            if (m_datCount == 0)
                m_datCount = datCnt;
            if (m_datCount != (int)datCnt) {
                fclose(stafile);
                Q_ASSERT(false);
                return false;
            }

            int datSize = datCnt * sizeof(float);
            char buf[datSize*4];
            memset(buf, 0, sizeof(buf));
            memcpy(buf, &one.max_val[0], datSize);
            memcpy(buf+datSize, &one.min_val[0], datSize);
            memcpy(buf+2*datSize, &one.avg_val[0], datSize);
            memcpy(buf+3*datSize, &one.cp95_val[0], datSize);
            fwrite(buf, datSize*4, 1, stafile);
        }
    }

    fclose(stafile);
    return true;
}

bool CStatDataTool::InitReadFile(const std::string &fileName, int datCount)
{
    if (datCount < 0 || datCount > 10)
        return false;
    int datSize = datCount * sizeof(float);
    QFile stafile(fileName.c_str());
    if (!stafile.open(QIODevice::ReadOnly))
        return false;
    int fileSize = stafile.size();
    if (fileSize % (4*datSize) != 0) {
        stafile.close();
        return false;
    }
    stafile.close();

    m_datFileName = fileName;
    m_datCount = datCount;
    m_funcType = Stat_Func_Read;
    return true;
}

bool CStatDataTool::ReadDataFromFile(int begIdx, int endIdx, StatDatas &dat)
{
    if (m_funcType != Stat_Func_Read)
        return false;

    int oneStaSize = m_datCount * sizeof(float) * 4;
    QFile stafile(m_datFileName.c_str());
    if (!stafile.open(QIODevice::ReadOnly))
        return false;

    int fileSize = stafile.size();
    int maxIdx = fileSize/oneStaSize - 1;
    if (begIdx > endIdx) {
        int tmp = begIdx;
        begIdx = endIdx;
        endIdx = tmp;
    }
    if (begIdx < 0)
        begIdx = 0;
    if (endIdx > maxIdx)
        endIdx = maxIdx;

    for (int i = begIdx; i != endIdx + 1; i++) {
        StatOneData oneSta;
        int pos = i * oneStaSize;
        stafile.seek(pos);
        char buf[oneStaSize];
        stafile.read(buf, oneStaSize);
        int bufIdx = 0;
        for (int j = 0; j != m_datCount; j++) {
            float tmp;
            memcpy(&tmp, &buf[bufIdx], sizeof(tmp));
            oneSta.max_val.push_back(tmp);
            bufIdx += sizeof(tmp);
        }

        for (int j = 0; j != m_datCount; j++) {
            float tmp;
            memcpy(&tmp, &buf[bufIdx], sizeof(tmp));
            oneSta.min_val.push_back(tmp);
            bufIdx += sizeof(tmp);
        }

        for (int j = 0; j != m_datCount; j++) {
            float tmp;
            memcpy(&tmp, &buf[bufIdx], sizeof(tmp));
            oneSta.avg_val.push_back(tmp);
            bufIdx += sizeof(tmp);
        }

        for (int j = 0; j != m_datCount; j++) {
            float tmp;
            memcpy(&tmp, &buf[bufIdx], sizeof(tmp));
            oneSta.cp95_val.push_back(tmp);
            bufIdx += sizeof(tmp);
        }

        dat.push_back(oneSta);
    }
    return true;
}

bool CStatDataTool::WriteInfoToFile(const string &fileName, const StatRecordInfo &info)
{
    QFile file(fileName.c_str());
    // 先清空文件内容
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.close();
    // 打开文件
    if (!file.open(QIODevice::WriteOnly))
        return false;
    // 保存统计信息
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "startTime", info.startTime.toString().c_str());
    cJSON_AddStringToObject(root, "endTime", info.endTime.toString().c_str());
    cJSON_AddStringToObject(root, "firstTime", info.firstTime.toString().c_str());
    cJSON_AddNumberToObject(root, "statTotalCnt", info.statTotalCnt);
    cJSON_AddNumberToObject(root, "statCycle", info.statCycle);
    cJSON_AddItemToObject(root, "recItem", _statRecItemToJson(info.recItem));
    cJSON_AddItemToObject(root, "measurecfg", _measureCfgToJsonObj(info.meaCfg));

    char *buf = cJSON_Print(root);
    file.write(buf);
    file.close();

    cJSON_Delete(root);
    delete[] buf;
    return true;
}

//读信息从文件中[修改取样点数来确定时间长度]
bool CStatDataTool::ReadInfoFromFile(const string &fileName, StatRecordInfo &info)
{
    QFile file(fileName.c_str());
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QByteArray content;
    content = file.readAll();
    file.close();

    cJSON *root = cJSON_Parse(content.data());
    if (root == NULL)
        return false;

    string tmp;
    cJSON_GetStringFromJson(root, "startTime", tmp);
    info.startTime.fromString(tmp.c_str());
    cJSON_GetStringFromJson(root, "endTime", tmp);
    info.endTime.fromString(tmp.c_str());
    cJSON_GetStringFromJson(root, "firstTime", tmp);
    info.firstTime.fromString(tmp.c_str());
    cJSON_GetIntFromJson(root, "statTotalCnt", info.statTotalCnt);//这里修改取样点的数量
    cJSON_GetIntFromJson(root, "statCycle", info.statCycle);
    _jsonObjToMeasureCfg(cJSON_GetObjectItem(root, "measurecfg"), info.meaCfg);
    _jsonTostatRecItem(cJSON_GetObjectItem(root, "recItem"), info.recItem);

    cJSON_Delete(root);
    return true;
}

#define AddRecItemToJson(node, str, val, cnt) do { \
    cJSON *json_tmp = cJSON_CreateArray(); \
    for (int z = 0; z != cnt; z++) \
        cJSON_AddBoolToObject(json_tmp, "", val[z]); \
    cJSON_AddItemToObject(node, str, json_tmp); \
}while(0);

#define AddOneRecItemToJson(node, str, val) do { \
    cJSON *json_tmp = cJSON_CreateArray(); \
    cJSON_AddBoolToObject(json_tmp, "", val); \
    cJSON_AddItemToObject(node, str, json_tmp); \
}while(0);

#define AddHarmRecItemToJson(node, str, val, cnt) do { \
    for (int y =0; y != cnt; y++) { \
        cJSON *json_tmp = cJSON_CreateArray(); \
        QString str_tmp; \
        str_tmp.sprintf(str, y+1); \
        for (int z = 0; z != 2; z++) \
            cJSON_AddBoolToObject(json_tmp, "", val[z][y]); \
        cJSON_AddItemToObject(node, str_tmp.toStdString().c_str(), json_tmp); \
    } \
}while(0);

cJSON* CStatDataTool::_statRecItemToJson(const StatRecordItem &item)
{
    cJSON *node = cJSON_CreateObject();
    AddRecItemToJson(node, "rms", item.rms, 2);
    AddRecItemToJson(node, "ang", item.ang, 2);
    AddRecItemToJson(node, "pos_peak", item.pos_peak, 2);
    AddRecItemToJson(node, "neg_peak", item.neg_peak, 2);
    AddRecItemToJson(node, "harm_dc_rms", item.harm_dc_rms, 2);
    AddRecItemToJson(node, "harm_dc_per", item.harm_dc_per, 2);
    AddRecItemToJson(node, "harm_thd_rms", item.harm_thd_rms, 2);
    AddRecItemToJson(node, "harm_thd_per", item.harm_thd_per, 2);
    AddRecItemToJson(node, "pos_value", item.pos_value, 2);
    AddRecItemToJson(node, "neg_value", item.neg_value, 2);
    AddRecItemToJson(node, "zero_value", item.zero_value, 2);
    AddRecItemToJson(node, "neg_unbalance", item.neg_unbalance, 2);
    AddRecItemToJson(node, "zero_unbalance", item.zero_unbalance, 2);
    AddRecItemToJson(node, "harm_power", item.harm_power, HARM_MAX);

    AddOneRecItemToJson(node, "active_power", item.active_power);
    AddOneRecItemToJson(node, "reactive_power", item.reactive_power);
    AddOneRecItemToJson(node, "appearent_power", item.apparent_power);
    AddOneRecItemToJson(node, "power_factor", item.power_factor);
    AddOneRecItemToJson(node, "frequency", item.frequency);

    AddHarmRecItemToJson(node, "harm_rms_%d", item.harm_rms, HARM_MAX);
    AddHarmRecItemToJson(node, "harm_per_%d", item.harm_per, HARM_MAX);
    AddHarmRecItemToJson(node, "harm_ang_%d", item.harm_ang, HARM_MAX);
    AddHarmRecItemToJson(node, "inter_harm_rms_%d", item.inter_harm_rms, INTER_HARM_MAX);
    AddHarmRecItemToJson(node, "inter_harm_per_%d", item.inter_harm_per, INTER_HARM_MAX);
    AddHarmRecItemToJson(node, "high_harm_rms_%d", item.high_harm_rms, HIGH_HARM_MAX);
    AddHarmRecItemToJson(node, "high_harm_per_%d", item.high_harm_per, HIGH_HARM_MAX);
    return node;
}

#define ReadRecItemFromJson(node, str, val, cnt) do { \
    cJSON *json_arr = cJSON_GetObjectItem(node, str); \
    if (json_arr != NULL) { \
        for (int z = 0; z != cnt; z++) { \
            cJSON *json_item = cJSON_GetArrayItem(json_arr, z); \
            if (json_item) val[z] = (json_item->type == cJSON_True); \
        } \
    } \
}while(0);

#define ReadOneRecItemFromJson(node, str, val) do { \
    cJSON *json_arr = cJSON_GetObjectItem(node, str); \
    cJSON *json_item = cJSON_GetArrayItem(json_arr, 0); \
    if (json_item) val = (json_item->type == cJSON_True); \
}while(0);

#define ReadHarmRecItemFromJson(node, str, val, cnt) do { \
    for (int y = 0; y != cnt; y++) { \
        QString str_tmp; \
        str_tmp.sprintf(str, y+1); \
        cJSON *json_arr = cJSON_GetObjectItem(node, str_tmp.toStdString().c_str()); \
        for (int z = 0; z != 2; z++) { \
            cJSON *json_item = cJSON_GetArrayItem(json_arr, z); \
            if (json_item) val[z][y] = (json_item->type == cJSON_True); \
        } \
    } \
}while(0);

bool CStatDataTool::_jsonTostatRecItem(cJSON *node, StatRecordItem &item)
{
    if (node == NULL)
        return false;
    ReadRecItemFromJson(node, "rms", item.rms, 2);
    ReadRecItemFromJson(node, "ang", item.ang, 2);
    ReadRecItemFromJson(node, "pos_peak", item.pos_peak, 2);
    ReadRecItemFromJson(node, "neg_peak", item.neg_peak, 2);
    ReadRecItemFromJson(node, "harm_dc_rms", item.harm_dc_rms, 2);
    ReadRecItemFromJson(node, "harm_dc_per", item.harm_dc_per, 2);
    ReadRecItemFromJson(node, "harm_thd_rms", item.harm_thd_rms, 2);
    ReadRecItemFromJson(node, "harm_thd_per", item.harm_thd_per, 2);
    ReadRecItemFromJson(node, "pos_value", item.pos_value, 2);
    ReadRecItemFromJson(node, "neg_value", item.neg_value, 2);
    ReadRecItemFromJson(node, "zero_value", item.zero_value, 2);
    ReadRecItemFromJson(node, "neg_unbalance", item.neg_unbalance, 2);
    ReadRecItemFromJson(node, "zero_unbalance", item.zero_unbalance, 2);
    ReadRecItemFromJson(node, "harm_power", item.harm_power, HARM_MAX);

    ReadOneRecItemFromJson(node, "active_power", item.active_power);
    ReadOneRecItemFromJson(node, "reactive_power", item.reactive_power);
    ReadOneRecItemFromJson(node, "appearent_power", item.apparent_power);
    ReadOneRecItemFromJson(node, "power_factor", item.power_factor);
    ReadOneRecItemFromJson(node, "frequency", item.frequency);

    ReadHarmRecItemFromJson(node, "harm_rms_%d", item.harm_rms, HARM_MAX);
    ReadHarmRecItemFromJson(node, "harm_per_%d", item.harm_per, HARM_MAX);
    ReadHarmRecItemFromJson(node, "harm_ang_%d", item.harm_ang, HARM_MAX);
    ReadHarmRecItemFromJson(node, "inter_harm_rms_%d", item.inter_harm_rms, INTER_HARM_MAX);
    ReadHarmRecItemFromJson(node, "inter_harm_per_%d", item.inter_harm_per, INTER_HARM_MAX);
    ReadHarmRecItemFromJson(node, "high_harm_rms_%d", item.high_harm_rms, HIGH_HARM_MAX);
    ReadHarmRecItemFromJson(node, "high_harm_per_%d", item.high_harm_per, HIGH_HARM_MAX);
    return true;
}

bool CStatDataTool::_jsonObjToMeasureCfg(cJSON *obj, MeasureConfig &cfg)
{
    if (obj == NULL)
        return false;
    cJSON_GetStringFromJson(obj, "measurePos", cfg.measurePos);
    cJSON_GetIntFromJson(obj, "freqMode", cfg.freqMode);
    cJSON_GetIntFromJson(obj, "connectMode", cfg.connectMode);
    cJSON_GetIntFromJson(obj, "powerAlgorithm", cfg.powerAlgorithm);
    cJSON_GetIntFromJson(obj, "voltRange", cfg.voltRange);
    cJSON_GetIntFromJson(obj, "clampType", cfg.clampType);
    cJSON_GetIntFromJson(obj, "voltLevel", cfg.voltLevel);

    cJSON_GetFloatFromJson(obj, "voltNominal", cfg.voltNominal);
    cJSON_GetFloatFromJson(obj, "currNominal", cfg.currNominal);
    cJSON_GetFloatFromJson(obj, "pt", cfg.pt);
    cJSON_GetFloatFromJson(obj, "npt", cfg.npt);
    cJSON_GetFloatFromJson(obj, "ct", cfg.ct);
    cJSON_GetFloatFromJson(obj, "nct", cfg.nct);
    cJSON_GetFloatFromJson(obj, "miniCapacity", cfg.miniCapacity);
    cJSON_GetFloatFromJson(obj, "freqUpLimit", cfg.freqUpLimit);
    cJSON_GetFloatFromJson(obj, "freqDownLimit", cfg.freqDownLimit);
    cJSON_GetFloatFromJson(obj, "voltUpLimit", cfg.voltUpLimit);
    cJSON_GetFloatFromJson(obj, "voltDownLimit", cfg.voltDownLimit);
    cJSON_GetFloatFromJson(obj, "voltUnbaLimit", cfg.voltUnbaLimit);
    cJSON_GetFloatFromJson(obj, "pstLimit", cfg.pstLimit);
    cJSON_GetFloatFromJson(obj, "pltLimit", cfg.pltLimit);
    cJSON_GetFloatFromJson(obj, "voltThdLimit", cfg.voltThdLimit);
    cJSON_GetFloatFromJson(obj, "voltOddHarmLimit", cfg.voltOddHarmLimit);
    cJSON_GetFloatFromJson(obj, "voltEvenHarmLimit", cfg.voltEvenHarmLimit);
    cJSON_GetFloatFromJson(obj, "voltSwell", cfg.voltSwell);
    cJSON_GetFloatFromJson(obj, "voltSwellLag", cfg.voltSwellLag);
    cJSON_GetFloatFromJson(obj, "voltSag", cfg.voltSag);
    cJSON_GetFloatFromJson(obj, "voltSagLag", cfg.voltSagLag);
    cJSON_GetFloatFromJson(obj, "voltInterrupt", cfg.voltInterrupt);
    cJSON_GetFloatFromJson(obj, "voltInterruptLag", cfg.voltInterruptLag);
    cJSON_GetFloatFromJson(obj, "currShock", cfg.currShock);
    cJSON_GetFloatFromJson(obj, "currShockLag", cfg.currShockLag);

    cJSON *json_curr = cJSON_GetObjectItem(obj, "currHarmLimit");
    if (json_curr) {
        for (int i = 0; i != 24; i++)
            cfg.currHarmLimit[i] = cJSON_GetArrayItem(json_curr, i)->valuedouble;
    }

    cJSON *json_voltCalcK = cJSON_GetObjectItem(obj, "voltCalcK");
    cJSON *json_voltCalcB = cJSON_GetObjectItem(obj, "voltCalcB");
    if (json_voltCalcK && json_voltCalcB) {
        for (int i = 0; i != MAX_VOLT_RANGE; i++) {
            cJSON *json_voltCalckItem = cJSON_GetArrayItem(json_voltCalcK, i);
            cJSON *json_voltCalcbItem = cJSON_GetArrayItem(json_voltCalcB, i);
            for (int j = 0; j != MAX_PHASE; j++) {
                cfg.voltCalcK[i][j] = cJSON_GetArrayItem(json_voltCalckItem, j)->valuedouble;
                cfg.voltCalcB[i][j] = cJSON_GetArrayItem(json_voltCalcbItem, j)->valuedouble;
            }
        }
    }

    cJSON *json_currCalcK = cJSON_GetObjectItem(obj, "currCalcK");
    cJSON *json_currCalcB = cJSON_GetObjectItem(obj, "currCalcB");
    if (json_currCalcK && json_currCalcB) {
        for (int i = 0; i != MAX_PHASE; i++) {
            if (json_currCalcK)
                cfg.currCalcK[i] = cJSON_GetArrayItem(json_currCalcK, i)->valuedouble;
            if (json_currCalcB)
                cfg.currCalcB[i] = cJSON_GetArrayItem(json_currCalcB, i)->valuedouble;
        }
    }

    return true;
}

cJSON* CStatDataTool::_measureCfgToJsonObj(const MeasureConfig &cfg)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "measurePos", cfg.measurePos.toStdString().c_str());

    cJSON_AddNumberToObject(root, "freqMode", cfg.freqMode);
    cJSON_AddNumberToObject(root, "connectMode", cfg.connectMode);
    cJSON_AddNumberToObject(root, "powerAlgorithm", cfg.powerAlgorithm);
    cJSON_AddNumberToObject(root, "voltNominal", cfg.voltNominal);
    cJSON_AddNumberToObject(root, "voltRange", cfg.voltRange);
    cJSON_AddNumberToObject(root, "currNominal", cfg.currNominal);
    cJSON_AddNumberToObject(root, "clampType", cfg.clampType);
    cJSON_AddNumberToObject(root, "pt", cfg.pt);
    cJSON_AddNumberToObject(root, "npt", cfg.npt);
    cJSON_AddNumberToObject(root, "ct", cfg.ct);
    cJSON_AddNumberToObject(root, "nct", cfg.nct);
    cJSON_AddNumberToObject(root, "voltLevel", cfg.voltLevel);
    cJSON_AddNumberToObject(root, "miniCapacity", cfg.miniCapacity);
    cJSON_AddNumberToObject(root, "freqUpLimit", cfg.freqUpLimit);
    cJSON_AddNumberToObject(root, "freqDownLimit", cfg.freqDownLimit);
    cJSON_AddNumberToObject(root, "voltUpLimit", cfg.voltUpLimit);
    cJSON_AddNumberToObject(root, "voltDownLimit", cfg.voltDownLimit);
    cJSON_AddNumberToObject(root, "voltUnbaLimit", cfg.voltUnbaLimit);
    cJSON_AddNumberToObject(root, "pstLimit", cfg.pstLimit);
    cJSON_AddNumberToObject(root, "pltLimit", cfg.pltLimit);
    cJSON_AddNumberToObject(root, "voltThdLimit", cfg.voltThdLimit);
    cJSON_AddNumberToObject(root, "voltOddHarmLimit", cfg.voltOddHarmLimit);
    cJSON_AddNumberToObject(root, "voltEvenHarmLimit", cfg.voltEvenHarmLimit);
    cJSON_AddNumberToObject(root, "voltSwell", cfg.voltSwell);
    cJSON_AddNumberToObject(root, "voltSwellLag", cfg.voltSwellLag);
    cJSON_AddNumberToObject(root, "voltSag", cfg.voltSag);
    cJSON_AddNumberToObject(root, "voltSagLag", cfg.voltSagLag);
    cJSON_AddNumberToObject(root, "voltInterrupt", cfg.voltInterrupt);
    cJSON_AddNumberToObject(root, "voltInterruptLag", cfg.voltInterruptLag);
    cJSON_AddNumberToObject(root, "currShock", cfg.currShock);
    cJSON_AddNumberToObject(root, "currShockLag", cfg.currShockLag);

    cJSON *json_curr = cJSON_CreateArray();
    for (int i = 0; i != 24; i++)
        cJSON_AddNumberToObject(json_curr, "", cfg.currHarmLimit[i]);
    cJSON_AddItemToObject(root, "currHarmLimit", json_curr);

    cJSON *json_voltCalcK = cJSON_CreateArray();
    cJSON *json_voltCalcB = cJSON_CreateArray();
    cJSON *json_currCalcK = cJSON_CreateArray();
    cJSON *json_currCalcB = cJSON_CreateArray();
    for (int i = 0; i != MAX_VOLT_RANGE; i++) {
        cJSON *json_voltCalckItem = cJSON_CreateArray();
        cJSON *json_voltCalcbItem = cJSON_CreateArray();
        for (int j = 0; j != MAX_PHASE; j++) {
            cJSON_AddNumberToObject(json_voltCalckItem, "", cfg.voltCalcK[i][j]);
            cJSON_AddNumberToObject(json_voltCalcbItem, "", cfg.voltCalcB[i][j]);
        }
        cJSON_AddItemToObject(json_voltCalcK, "", json_voltCalckItem);
        cJSON_AddItemToObject(json_voltCalcB, "", json_voltCalcbItem);
    }
    cJSON_AddItemToObject(root, "voltCalcK", json_voltCalcK);
    cJSON_AddItemToObject(root, "voltCalcB", json_voltCalcB);

    for (int i = 0; i != MAX_PHASE; i++) {
        cJSON_AddNumberToObject(json_currCalcK, "", cfg.currCalcK[i]);
        cJSON_AddNumberToObject(json_currCalcB, "", cfg.currCalcB[i]);
    }
    cJSON_AddItemToObject(root, "currCalcK", json_currCalcK);
    cJSON_AddItemToObject(root, "currCalcB", json_currCalcB);
    return root;
}
