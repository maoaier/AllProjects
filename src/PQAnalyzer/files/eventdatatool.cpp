#include "eventdatatool.h"
#include "comtradetool.h"
#include "cJSON.h"
#include "util.h"
#include <QFile>
#include <string.h>

using namespace std;

const char* CInfoToolBase::_readAllFileContent(string fileName)
{
    QFile file(fileName.c_str());
    char *readbuf = NULL;
    if (file.open(QIODevice::ReadOnly)) {
        file.seek(0);
        int fsize = file.size();
        if (fsize < 0) {
            file.close();
            return readbuf;
        }
        readbuf = new char[fsize];
        file.seek(0);
        file.read(readbuf, fsize);
        file.close();
    }
    return readbuf;
}

bool CInfoToolBase::_writeToFile(string fileName, const char *buf, int len)
{
    // 先清空文件
    QFile file(fileName.c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    file.write(buf, len);
    file.close();
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////

bool CTransInfoTool::SaveEventInfo(string filePath, const TransEventInfos &infos)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "event_cnt", infos.size());
    int i = 0;
    for (TransEventInfos::const_iterator ite = infos.begin();
         ite != infos.end(); ++ite, ++i) {
        TransEventInfo info = *ite;
        // 添加新的一项事件信息
        char tmp[100];
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "meapos", info.meapos.c_str());
        cJSON_AddStringToObject(item, "waveStaTime",
            info.waveStaTime.toDateTime().toString().c_str());
        cJSON_AddStringToObject(item, "eventStaTime",
            info.eventStaTime.toDateTime().toString().c_str());
        cJSON_AddStringToObject(item, "eventEndTime",
            info.eventEndTime.toDateTime().toString().c_str());
        cJSON_AddStringToObject(item, "waveEndTime",
            info.waveEndTime.toDateTime().toString().c_str());
        cJSON_AddNumberToObject(item, "eventType", info.eventType);
        cJSON_AddNumberToObject(item, "eventWaveCnt", info.eventWaveCnt);
        cJSON_AddNumberToObject(item, "sample", info.sample);
        cJSON_AddNumberToObject(item, "connType", info.connType);
        cJSON_AddNumberToObject(item, "sysFreq", info.sysFreq);
        cJSON_AddNumberToObject(item, "eventLimit", info.eventLimit);
        cJSON *json_event_val = cJSON_CreateArray();
        cJSON *json_event_pha = cJSON_CreateArray();
        for (int j = PHASE_A; j != PHASE_N; j++) {
            cJSON_AddNumberToObject(json_event_val, "", info.eventVal[j]);
            cJSON_AddBoolToObject(json_event_pha, "", info.eventPhase[j]);
        }
        cJSON_AddItemToObject(item, "eventVal", json_event_val);
        cJSON_AddItemToObject(item, "eventPhase", json_event_pha);
        cJSON *json_paramK = cJSON_CreateArray();
        cJSON *json_paramB = cJSON_CreateArray();
        for (int j = 0; j != 2; j++) {
            cJSON *json_paramK_item = cJSON_CreateArray();
            cJSON *json_paramB_item = cJSON_CreateArray();
            for (int k = PHASE_A; k != MAX_PHASE; k++) {
                cJSON_AddNumberToObject(json_paramK_item, "", info.paramK[j][k]);
                cJSON_AddNumberToObject(json_paramB_item, "", info.paramB[j][k]);
            }
            cJSON_AddItemToObject(json_paramK, "", json_paramK_item);
            cJSON_AddItemToObject(json_paramB, "", json_paramB_item);
        }
        cJSON_AddItemToObject(item, "paramK", json_paramK);
        cJSON_AddItemToObject(item, "paramB", json_paramB);
        sprintf(tmp, "event_item%d", i);
        cJSON_AddItemToObject(root, tmp, item);
    }
    // 写入文件
    char *writebuf = cJSON_Print(root);
    string fileName = filePath + "trans_event.info";
    _writeToFile(fileName, writebuf, strlen(writebuf));
    cJSON_Delete(root);
    delete[] writebuf;
    return true;
}

bool CTransInfoTool::ReadEventInfo(string filePath, TransEventInfos &infos)
{
    infos.clear();
    // 读取事件信息
    string fileName = filePath + "trans_event.info";
    const char *readbuf = _readAllFileContent(fileName);
    // 解析Json文件
    cJSON *root = cJSON_Parse(readbuf);
    if (root == NULL)
        return false;
    int eventCnt = 0;
    cJSON_GetIntFromJson(root, "event_cnt", eventCnt);
    for (int i = 0; i != eventCnt; i++) {
        char tmp[100];
        string strVal;
        TransEventInfo info;
        DateTime date;
        sprintf(tmp, "event_item%d", i);
        cJSON *pItem = cJSON_GetObjectItem(root, tmp);
        if (pItem == NULL)
            continue;
        cJSON_GetStringFromJson(pItem, "meapos", info.meapos);

        cJSON_GetStringFromJson(pItem, "waveStaTime", strVal);
        date.fromString(strVal.c_str());
        info.waveStaTime = date.toUtcTime();

        cJSON_GetStringFromJson(pItem, "waveEndTime", strVal);
        date.fromString(strVal.c_str());
        info.waveEndTime = date.toUtcTime();

        cJSON_GetStringFromJson(pItem, "eventStaTime", strVal);
        date.fromString(strVal.c_str());
        info.eventStaTime = date.toUtcTime();

        cJSON_GetStringFromJson(pItem, "eventEndTime", strVal);
        date.fromString(strVal.c_str());
        info.eventEndTime = date.toUtcTime();

        cJSON_GetIntFromJson(pItem, "eventType", info.eventType);
        cJSON_GetIntFromJson(pItem, "eventWaveCnt", info.eventWaveCnt);
        cJSON_GetIntFromJson(pItem, "sample", info.sample);
        cJSON_GetIntFromJson(pItem, "connType", info.connType);
        cJSON_GetFloatFromJson(pItem, "sysFreq", info.sysFreq);
        cJSON_GetFloatFromJson(pItem, "eventLimit", info.eventLimit);
        cJSON *json_event_val = cJSON_GetObjectItem(pItem, "eventVal");
        cJSON *json_event_pha = cJSON_GetObjectItem(pItem, "eventPhase");
        for (int j = PHASE_A; j != PHASE_N; j++) {
            if (json_event_val)
                info.eventVal[j] = cJSON_GetArrayItem(json_event_val, j)->valuedouble;
            if (json_event_pha)
                info.eventPhase[j] = cJSON_GetArrayItem(json_event_pha, j)->type == cJSON_True;
        }
        cJSON *json_paramk = cJSON_GetObjectItem(pItem, "paramK");
        cJSON *json_paramb = cJSON_GetObjectItem(pItem, "paramB");
        for (int j = 0; j != 2; j++) {
            cJSON *json_paramk_item = cJSON_GetArrayItem(json_paramk, j);
            cJSON *json_paramb_item = cJSON_GetArrayItem(json_paramb, j);
            for (int k = PHASE_A; k != MAX_PHASE; k++) {
                info.paramK[j][k] = cJSON_GetArrayItem(json_paramk_item, k)->valuedouble;
                info.paramB[j][k] = cJSON_GetArrayItem(json_paramb_item, k)->valuedouble;
            }
        }
        infos.push_back(info);
    }
    delete[] readbuf;
    return true;
}

bool CTransInfoTool::SaveEventWave(std::string filePath, const TransEventInfo &info, const TransEventWave &wave)
{
    ComtradeCfgInfo comInfo;
    const char phaTitle[] = { 'A', 'B', 'C', 'N' };
    const char *chnTitle[][4] = { {"Ua", "Ub", "Uc", "Un"},
                                  {"Ia", "Ib", "Ic", "In"}};
    comInfo.station_name = info.meapos;
    comInfo.analog_ch_cnt = 2*MAX_PHASE;
    comInfo.begTime = info.waveStaTime;
    comInfo.triTime = info.eventStaTime;
    for (int i = 0; i != 2; i++) {
        for (int j = PHASE_A; j != MAX_PHASE; j++) {
            AnalogCfgInfo aInfo;
            aInfo.an = i * MAX_PHASE + j;
            aInfo.ch_id = chnTitle[i][j];
            aInfo.ph = phaTitle[j];
            aInfo.uu = i == 0 ? "V" : "A";
            aInfo.a = info.paramK[i][j];
            aInfo.b = info.paramB[i][j];
            aInfo.skew = 0;
            aInfo.max = 32768;
            aInfo.min = -32768;
            aInfo.primary = aInfo.secondary = 1;
            aInfo.ps = "S";
            comInfo.analog_cfg_infos.push_back(aInfo);
        }
    }
    comInfo.digital_ch_cnt = 0;
    comInfo.total_ch_cnt = comInfo.analog_ch_cnt + comInfo.digital_ch_cnt;
    comInfo.freq = info.sysFreq;
    comInfo.dataType = Com_Data_Binary;
    comInfo.nrates = 1;
    SampleCfgInfo sInfo;
    sInfo.samp = info.sample*info.sysFreq;
    sInfo.endsamp = 0;
    comInfo.sample_cfg_infos.push_back(sInfo);
    comInfo.timemult = 1;

    UtcTime waveStaTime;
    ComtradeData comData;
    comData.reserve(wave.size());
    int i = 0;
    for (TransEventWave::const_iterator ite = wave.begin();
         ite != wave.end(); ++ite, ++i) {
        if (i == 0)
            waveStaTime = ite->potTime;
        ComtradeOneData one;
        for (int j = 0; j != 2; j++) {
            for (int k = PHASE_A; k != MAX_PHASE; k++) {
                short tmp = (ite->potWave[j][k] - info.paramB[j][k])/info.paramK[j][k];
                one.analog_ch_datas.push_back(tmp);
            }
        }
        one.timestamp = (ite->potTime.tv_sec - waveStaTime.tv_sec)*1000*1000 +
                (ite->potTime.tv_usec - waveStaTime.tv_usec);
        comData.push_back(one);
    }

    CComtradeTool comTool;
    char cTmp[100] = { 0 };
    DateTime startTime = info.eventStaTime.toDateTime();
    sprintf(cTmp, "%02d%02d%02d_%03d", startTime.hour, startTime.minute,
            startTime.second, startTime.ms);
    string fileName = filePath + cTmp;
    comTool.InitWriteFile(fileName, comInfo);
    comTool.WriteDataToFile(comData);
    return true;
}

bool CTransInfoTool::ReadEventWave(string filePath, const TransEventInfo &info, TransEventWave &wave)
{
    wave.clear();
    CComtradeTool comTool;
    ComtradeCfgInfo comInfo;
    char cTmp[100] = { 0 };
    DateTime startTime = info.eventStaTime.toDateTime();
    sprintf(cTmp, "%02d%02d%02d_%03d", startTime.hour, startTime.minute,
            startTime.second, startTime.ms);
    string fileName = filePath + cTmp;
    if (!comTool.InitReadFile(fileName, comInfo))
        return false;
    ComtradeData comData;
    if (!comTool.ReadDataFromFile(0, comInfo.sample_cfg_infos.front().endsamp - 1, comData))
        return false;
    for (int i = 0; i != (int)comData.size(); i++) {
        TransEventPoint point;
        point.potTime.tv_sec = comInfo.begTime.tv_sec + comData.at(i).timestamp/(1000*1000);
        point.potTime.tv_usec = comInfo.begTime.tv_usec + comData.at(i).timestamp%(1000*1000);
        if (point.potTime.tv_usec >= 1000*1000) {
            point.potTime.tv_sec += 1;
            point.potTime.tv_usec -= 1000*1000;
        }
        for (int j = 0; j != (int)comData.at(i).analog_ch_datas.size(); j++) {
            float wavePt = comData.at(i).analog_ch_datas.at(j)*comInfo.analog_cfg_infos.at(j).a +
                    comInfo.analog_cfg_infos.at(j).b;
            point.potWave[j/MAX_PHASE][j%MAX_PHASE] = wavePt;
        }
        wave.push_back(point);
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////

bool CSteadyInfoTool::SaveEventInfo(string filePath, const SteadyEventInfos &infos)
{
    string fileName = filePath += "steady_event.info";
    // 创建json内容
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "event_cnt", infos.size());
    for (int i = 0; i != (int)infos.size(); i++) {
        SteadyEventInfo info = infos.at(i);
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "startTime", info.startTime.toDateTime().toString().c_str());
        cJSON_AddStringToObject(item, "endTime", info.endTime.toDateTime().toString().c_str());
        cJSON_AddNumberToObject(item, "eventType", info.eventType);
        cJSON_AddNumberToObject(item, "eventPhase", info.eventPhase);
        cJSON_AddNumberToObject(item, "eventHarmIdx", info.eventHarmIdx);
        cJSON_AddNumberToObject(item, "eventVal", info.eventVal);
        cJSON_AddNumberToObject(item, "eventLimit", info.eventLimit);
        char tmp[20];
        sprintf(tmp, "event_item%d", i + 1);
        cJSON_AddItemToObject(root, tmp, item);
    }
    // 写入文件
    char *writebuf = cJSON_Print(root);
    _writeToFile(fileName, writebuf, strlen(writebuf));
    cJSON_Delete(root);
    delete[] writebuf;
    return true;
}

bool CSteadyInfoTool::ReadEventInfo(string filePath, SteadyEventInfos &infos)
{
    infos.clear();
    // 读取事件信息
    string fileName = filePath += "steady_event.info";
    const char *readbuf = _readAllFileContent(fileName);
    // 解析Json文件
    cJSON *root = cJSON_Parse(readbuf);
    if (root == NULL)
        return false;
    int eventCnt = 0;
    cJSON_GetIntFromJson(root, "event_cnt", eventCnt);
    for (int i = 0; i != eventCnt; i++) {
        char tmp[100];
        string strVal;
        DateTime date;
        SteadyEventInfo info;
        sprintf(tmp, "event_item%d", i + 1);
        cJSON *pItem = cJSON_GetObjectItem(root, tmp);
        if (pItem == NULL)
            continue;
        cJSON_GetStringFromJson(pItem, "startTime", strVal);
        sscanf(strVal.c_str(), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            &date.year, &date.month, &date.day,
            &date.hour, &date.minute, &date.second, &date.ms);
        info.startTime = date.toUtcTime();
        cJSON_GetStringFromJson(pItem, "endTime", strVal);
        if (!strVal.empty()) {
            sscanf(strVal.c_str(), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                &date.year, &date.month, &date.day,
                &date.hour, &date.minute, &date.second, &date.ms);
            info.endTime = date.toUtcTime();
        }
        cJSON_GetIntFromJson(pItem, "eventType", info.eventType);
        cJSON_GetIntFromJson(pItem, "eventPhase", info.eventPhase);
        cJSON_GetIntFromJson(pItem, "eventHarmIdx", info.eventHarmIdx);
        cJSON_GetFloatFromJson(pItem, "eventVal", info.eventVal);
        cJSON_GetFloatFromJson(pItem, "eventLimit", info.eventLimit);
        infos.push_back(info);
    }
    return true;
}
