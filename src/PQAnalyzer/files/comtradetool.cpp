#include "comtradetool.h"
#include "util.h"
#include <QFile>
#include <iostream>
#include <stdlib.h>

using namespace std;

CComtradeTool::CComtradeTool()
    :m_funcType(Com_Func_None)
    ,m_writeIdx(0)
{
}

CComtradeTool::~CComtradeTool()
{

}

bool CComtradeTool::InitWriteFile(const string &fileName, const ComtradeCfgInfo &cfg)
{
    if (m_funcType == Com_Func_Read)
        return false;
    if (!WriteInfoFile(fileName, cfg))
        return false;

    // 如果录波文件已存在,则先清空内容
    string datFilePath = fileName + ".dat";
    FILE *datfile = fopen(datFilePath.c_str(), "w");
    fclose(datfile);

    m_funcType = Com_Func_Write;
    m_cfgInfo = cfg;
    m_fileName = fileName;
    m_writeIdx = 0;
    return true;
}

bool CComtradeTool::WriteInfoFile(const string &fileName, const ComtradeCfgInfo &cfg)
{
    string cfgFilePath = fileName + ".cfg";
    FILE *cfgfile = fopen(cfgFilePath.c_str(), "w");
    if (cfgfile == NULL)
    {
        ErrorLog("WriteInfoFile file %s can not open",
            cfgFilePath.c_str());
        return false;
    }
    string writeBuf;
    ChangeCfgToString(cfg, writeBuf);
    fwrite(writeBuf.c_str(), writeBuf.size(), 1, cfgfile);
    fclose(cfgfile);
    return true;
}

bool CComtradeTool::WriteDataToFile(const ComtradeData &dat)
{
    if (m_funcType != Com_Func_Write)
        return false;
    string datFilePath = m_fileName + ".dat";
    FILE *datfile = fopen(datFilePath.c_str(), "ab+");
    if (datfile == NULL)
    {
        ErrorLog("WriteDataToFile file %s can not open",
            datFilePath.c_str());
        return false;
    }

    // 当前只有一种采样率的波形数据,因此直接更新cfg文件内容
    m_cfgInfo.sample_cfg_infos.at(0).endsamp += dat.size();
    WriteInfoFile(m_fileName, m_cfgInfo);

    int bufSize = 0;
    char *buf = ChangeDataToBuf(m_cfgInfo, m_writeIdx, dat, bufSize);
    int ret = fwrite(buf, bufSize, 1, datfile);
    m_writeIdx += dat.size();
    fclose(datfile);
    delete[] buf;
    return ret == 1;
}

bool CComtradeTool::InitReadFile(const string &fileName, ComtradeCfgInfo &cfg)
{
    if (m_funcType == Com_Func_Write)
        return false;
    string cfgFilePath = fileName + ".cfg";
    QFile cfgfile(cfgFilePath.c_str());
    if (!cfgfile.open(QIODevice::ReadOnly))
    {
        ErrorLog("InitReadFile file %s can not open",
            cfgFilePath.c_str());
        return false;
    }

    // 解析.cfg文件
    vector<string> lineBuf;
    char strBuf[1024];
    memset(strBuf, 0, sizeof(strBuf));
    while (-1 != cfgfile.readLine(strBuf, sizeof(strBuf)))
    {
        string tmp;
        tmp = strBuf;
        lineBuf.push_back(tmp);
        memset(strBuf, 0, sizeof(strBuf));
    }
    if (!AnalyseBufToCfgInfo(lineBuf, cfg))
    {
        ErrorLog("InitReadFile can not analyse cfg from %s",
            cfgFilePath.c_str());
        cfgfile.close();
        return false;
    }
    m_funcType = Com_Func_Read;
    m_cfgInfo = cfg;
    m_fileName = fileName;
    return true;
}

bool CComtradeTool::ReadDataFromFile(int begIdx, int endIdx, ComtradeData &dat)
{
    if (m_funcType != Com_Func_Read)
        return false;
    int maxIdx = GetMaxIdx(m_cfgInfo);
    if (begIdx < 0 || endIdx < 0 ||
        endIdx >= maxIdx || begIdx > endIdx) {
        ErrorLog("ReadDataFromFile idx error, begIdx %d, endIdx %d, maxIdx %d",
               begIdx, endIdx, maxIdx);
        return false;
    }

    string datFilePath = m_fileName + ".dat";
    QFile datfile(datFilePath.c_str());
    if (!datfile.open(QIODevice::ReadOnly))
    {
        ErrorLog("ReadDataFromFile file %s can not open",
            m_fileName.c_str());
        return false;
    }
    int fsize = datfile.size();
    if (fsize != maxIdx * GetOneDataSize(m_cfgInfo))
    {
        ErrorLog("ReadDataFromFile file size %d is error, theory size is %d",
            fsize, maxIdx * GetOneDataSize(m_cfgInfo));
        datfile.close();
        return false;
    }

    // 从起始位置开始获取波形数据
    int begPos = begIdx * GetOneDataSize(m_cfgInfo);
    int bufSize = (endIdx - begIdx + 1) * GetOneDataSize(m_cfgInfo);
    datfile.seek(begPos);
    char *buf = new char[bufSize];
    datfile.read(buf, bufSize);
    AnalyseBufToData((unsigned char*)buf, bufSize, m_cfgInfo, dat);
    delete[] buf;
    datfile.close();
    return true;
}

bool CComtradeTool::WriteComtradeFile(const string &fileName,
    const ComtradeCfgInfo &cfg, const ComtradeData &dat)
{
    string cfgFilePath = fileName + ".cfg";
    string datFilePath = fileName + ".dat";
    FILE *cfgfile, *datfile;
    cfgfile = fopen(cfgFilePath.c_str(), "w");
    if (cfgfile == NULL)
    {
        ErrorLog("CComtradeTool write func, file %s can not open",
            cfgFilePath.c_str());
        return false;
    }
    datfile = fopen(datFilePath.c_str(), "wb");
    if (datfile == NULL)
    {
        fclose(cfgfile);
        ErrorLog("CComtradeTool write func, file %s can not open",
            datFilePath.c_str());
        return false;
    }

    string writeBuf;
    ChangeCfgToString(cfg, writeBuf);
    fwrite(writeBuf.c_str(), writeBuf.size(), 1, cfgfile);
    int idx = 0, bufSize = 0;
    char *buf = ChangeDataToBuf(cfg, 0, dat, bufSize);
    for (; idx + 1024 < bufSize; idx += 1024)
    {
        fwrite(&buf[idx], 1024, 1, datfile);
    }
    fwrite(&buf[idx], bufSize - idx, 1, datfile);

#ifndef WIN32
    // 清空一下文件cache
    system("echo 1 > /proc/sys/vm/drop_caches");
#endif

    delete[] buf;
    fclose(cfgfile);
    fclose(datfile);
    return true;
}

bool CComtradeTool::ChangeCfgToString(const ComtradeCfgInfo &info, string &buf)
{
    char tmp[1024] = { 0 };
    buf.clear();

    // 站点信息参数
    sprintf(tmp, "%s,%s,%d \n", info.station_name.c_str(),
        info.rec_dev_id.c_str(), info.rev_year);
    buf += tmp;

    // 通道信息参数
    sprintf(tmp, "%d,%dA,%dD \n", info.total_ch_cnt,
        info.analog_ch_cnt, info.digital_ch_cnt);
    buf += tmp;

    // 模拟量参数
    for (int i = 0; i < (int)info.analog_cfg_infos.size(); i++)
    {
        AnalogCfgInfo ainfo = info.analog_cfg_infos[i];
        sprintf(tmp, "%d,%s,%s,%s,%s,%f,%f,%f,%d,%d,%f,%f,%s \n", ainfo.an,
            ainfo.ch_id.c_str(), ainfo.ph.c_str(), ainfo.ccbm.c_str(),
            ainfo.uu.c_str(), ainfo.a, ainfo.b, ainfo.skew, ainfo.min, ainfo.max,
            ainfo.primary, ainfo.secondary, ainfo.ps.c_str());
        buf += tmp;
    }

    // 数字量参数
    for (int i = 0; i < (int)info.digital_cfg_infos.size(); i++)
    {
        DigitalCfgInfo dinfo = info.digital_cfg_infos[i];
        sprintf(tmp, "%d,%s,%s,%s,%d \n", dinfo.dn, dinfo.ch_id.c_str(),
            dinfo.ph.c_str(), dinfo.ccbm.c_str(), dinfo.y ? 1 : 0);
        buf += tmp;
    }

    // 频率参数
    sprintf(tmp, "%f \n", info.freq);
    buf += tmp;

    // 采样率参数
    sprintf(tmp, "%d \n", info.nrates);
    buf += tmp;
    for (int i = 0; i < (int)info.sample_cfg_infos.size(); i++)
    {
        SampleCfgInfo sinfo = info.sample_cfg_infos[i];
        sprintf(tmp, "%f,%d \n", sinfo.samp, sinfo.endsamp);
        buf += tmp;
    }

    // 时间参数
    DateTime dateTmp = info.begTime.toDateTime();
    sprintf(tmp, "%02d/%02d/%04d,%02d:%02d:%02d.%06ld \n", dateTmp.day, dateTmp.month,
            dateTmp.year, dateTmp.hour, dateTmp.minute, dateTmp.second, info.begTime.tv_usec);
    buf += tmp;

    dateTmp = info.triTime.toDateTime();
    sprintf(tmp, "%02d/%02d/%04d,%02d:%02d:%02d.%06ld \n", dateTmp.day, dateTmp.month,
            dateTmp.year, dateTmp.hour, dateTmp.minute, dateTmp.second, info.triTime.tv_usec);
    buf += tmp;

    // 数据格式参数
    sprintf(tmp, "%s \n", info.dataType == Com_Data_Ascii ? "ascii" : "binary");
    buf += tmp;

    // 时间标记系数
    sprintf(tmp, "%d \n", info.timemult);
    buf += tmp;
    return true;
}

char* CComtradeTool::ChangeDataToBuf(const ComtradeCfgInfo &info,
                                     int startIdx, const ComtradeData &dat, int &bufSize)
{
    if (info.dataType == Com_Data_Ascii)
        return ChangeDataToBufByAscii(info, startIdx, dat, bufSize);
    return ChangeDataToBufByBinary(info, startIdx, dat, bufSize);
}

char* CComtradeTool::ChangeDataToBufByAscii(const ComtradeCfgInfo &/*info*/,
                                            int /*startIdx*/, const ComtradeData &/*dat*/, int &bufSize)
{
    // 暂不支持ASCII文件
    bufSize = 0;
    return NULL;
}

char* CComtradeTool::ChangeDataToBufByBinary(const ComtradeCfgInfo &info,
                                             int startIdx, const ComtradeData &dat, int &bufSize)
{
    int bufIdx = 0;
    bufSize = dat.size() * GetOneDataSize(info);
    char *buf = new char[bufSize];
    for (int i = 0; i != (int)dat.size(); i++)
    {
        bufIdx += PushData(startIdx + i + 1, &buf[bufIdx]);
        bufIdx += PushData(dat[i].timestamp, &buf[bufIdx]);
        for (int k = 0; k != (int)dat[i].analog_ch_datas.size(); k++)
            bufIdx += PushData(dat[i].analog_ch_datas[k], &buf[bufIdx]);
        bufIdx += PushData(dat[i].digital_ch_datas, &buf[bufIdx]);
    }
    return buf;
}

int CComtradeTool::PushData(int idata, char *buf)
{
    if (idata < 0)
    {
        idata = -idata;
        idata = ~idata + 1;
    }
    int idx = 0;
    buf[idx++] = idata;
    buf[idx++] = idata >> 8;
    buf[idx++] = idata >> 16;
    buf[idx++] = idata >> 24;
    return 4;
}

int CComtradeTool::PushData(short sdata, char *buf)
{
    if (sdata < 0)
    {
        sdata = -sdata;
        sdata = ~sdata + 1;
    }
    int idx = 0;
    buf[idx++] = sdata;
    buf[idx++] = sdata >> 8;
    return 2;
}

int CComtradeTool::PushData(vector<bool> bdata, char *buf)
{
    char tmp = 0;
    vector<char> bufTmp;
    for (int i = 0; i != (int)bdata.size(); i++)
    {
        if (i != 0 && i % 8 == 0)
        {
            bufTmp.push_back(tmp);
            tmp = 0;
        }
        tmp |= (bdata[i] ? 1 : 0) << (i % 8);
    }
    if (bufTmp.size() % 2 == 1)
        bufTmp.push_back(0);
    int idx = 0;
    for (int i = 0; i != (int)bufTmp.size(); i++)
        buf[idx++] = bufTmp[i];
    return bufTmp.size();
}

bool CComtradeTool::ReadComtradeFile(const string &fileName,
    ComtradeCfgInfo &cfg, ComtradeData &dat)
{
    // 打开.cfg以及.dat文件
    string cfgFilePath = fileName + ".cfg";
    string datFilePath = fileName + ".dat";
    QFile cfgfile(cfgFilePath.c_str());
    if (!cfgfile.open(QIODevice::ReadOnly))
    {
        ErrorLog("CComtradeTool read func, file %s can not open",
            cfgFilePath.c_str());
        return false;
    }
    QFile datfile(datFilePath.c_str());
    if (!datfile.open(QIODevice::ReadOnly))
    {
        cfgfile.close();
        ErrorLog("CComtradeTool read func, file %s can not open",
            datFilePath.c_str());
        return false;
    }

    // 解析.cfg文件
    vector<string> lineBuf;
    char strBuf[1024];
    memset(strBuf, 0, sizeof(strBuf));
    while (-1 != cfgfile.readLine(strBuf, sizeof(strBuf)))
    {
        string tmp;
        tmp = strBuf;
        lineBuf.push_back(tmp);
        memset(strBuf, 0, sizeof(strBuf));
    }
    if (!AnalyseBufToCfgInfo(lineBuf, cfg))
    {
        cfgfile.close();
        datfile.close();
        return false;
    }

    // 解析.dat文件
    int fsize = datfile.size();
    if (fsize <= 0)
    {
        cfgfile.close();
        datfile.close();
        return false;
    }
    char *buf = new char[fsize];
    datfile.seek(0);
    datfile.read(buf, fsize);
    AnalyseBufToData((unsigned char*)buf, fsize, cfg, dat);
    delete[] buf;
    cfgfile.close();
    datfile.close();
    return true;
}

bool CComtradeTool::AnalyseBufToCfgInfo(const vector<string> &buf,
    ComtradeCfgInfo &info)
{
    // 解析站名
    if (buf.empty())
        return false;
    unsigned int idx = 0;
    string tmp;
    int curIdx = 0, lastIdx = 0;
    GetSubString(buf[idx], ',', lastIdx, curIdx, info.station_name);
    GetSubString(buf[idx], ',', lastIdx, curIdx, info.rec_dev_id);
    GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
    info.rev_year = atoi(tmp.c_str());
    idx++;

    // 解析通道数
    if (buf.size() < idx + 1)
        return false;
    curIdx = lastIdx = 0;
    GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
    info.total_ch_cnt = atoi(tmp.c_str());
    GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
    info.analog_ch_cnt = atoi(tmp.c_str());
    GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
    info.digital_ch_cnt = atoi(tmp.c_str());
    idx++;

    // 解析模拟通道信息
    if (buf.size() < idx + 1 + info.analog_ch_cnt)
        return false;
    for (int i = 0; i < info.analog_ch_cnt; i++)
    {
        AnalogCfgInfo aInfo;
        curIdx = lastIdx = 0;
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.an = atoi(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, aInfo.ch_id);
        GetSubString(buf[idx], ',', lastIdx, curIdx, aInfo.ph);
        GetSubString(buf[idx], ',', lastIdx, curIdx, aInfo.ccbm);
        GetSubString(buf[idx], ',', lastIdx, curIdx, aInfo.uu);
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.a = (float)atof(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.b = (float)atof(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.skew = (float)atof(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.min = atoi(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.max = atoi(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.primary = (float)atof(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        aInfo.secondary = (float)atof(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, aInfo.ps);
        info.analog_cfg_infos.push_back(aInfo);
        idx++;
    }

    // 解析数字通道信息
    if (buf.size() < idx + 1 + info.digital_ch_cnt)
        return false;
    for (int i = 0; i < info.digital_ch_cnt; i++)
    {
        curIdx = lastIdx = 0;
        DigitalCfgInfo dInfo;
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        dInfo.dn = atoi(tmp.c_str());
        GetSubString(buf[idx], ',', lastIdx, curIdx, dInfo.ch_id);
        GetSubString(buf[idx], ',', lastIdx, curIdx, dInfo.ph);
        GetSubString(buf[idx], ',', lastIdx, curIdx, dInfo.ccbm);
        GetSubString(buf[idx], ',', lastIdx, curIdx, tmp);
        dInfo.y = (atoi(tmp.c_str()) == 1);
        info.digital_cfg_infos.push_back(dInfo);
        idx++;
    }

    // 解析频率信息
    if (buf.size() < idx + 1)
        return false;
    sscanf(buf[idx++].c_str(), "%f", &info.freq);

    // 解析采样率信息
    if (buf.size() < idx + 1)
        return false;
    sscanf(buf[idx++].c_str(), "%d", &info.nrates);
    if (buf.size() < idx + 1 + info.nrates)
        return false;
    for (int i = 0; i < info.nrates; i++)
    {
        SampleCfgInfo sInfo;
        sscanf(buf[idx++].c_str(), "%f,%d", &sInfo.samp, &sInfo.endsamp);
        info.sample_cfg_infos.push_back(sInfo);
    }

    // 解析时间信息
    if (buf.size() < idx + 1)
        return false;
    int us = 0;
    DateTime tt;
    sscanf(buf[idx++].c_str(), "%02d/%02d/%04d,%02d:%02d:%02d.%06d",
        &tt.day, &tt.month, &tt.year,
        &tt.hour, &tt.minute, &tt.second,
        &us);
    info.begTime = tt.toUtcTime();
    info.begTime.tv_usec = us;
    if (buf.size() < idx + 1)
        return false;
    sscanf(buf[idx++].c_str(), "%02d/%02d/%04d,%02d:%02d:%02d.%06d",
        &tt.day, &tt.month, &tt.year,
        &tt.hour, &tt.minute, &tt.second,
        &us);
    info.triTime = tt.toUtcTime();
    info.triTime.tv_usec = us;

    // 解析数据类型信息
    if (buf.size() < idx + 1)
        return false;
    if (buf[idx].compare("binary") == 0 || buf[idx].compare("BINARY") == 0)
        info.dataType = Com_Data_Binary;
    else
        info.dataType = Com_Data_Ascii;
    idx++;

    // 解析时间系统信息
    if (buf.size() < idx + 1)
        return false;
    sscanf(buf[idx++].c_str(), "%d", &info.timemult);
    return true;
}

bool CComtradeTool::AnalyseBufToData(const unsigned char *buf, int bufSize,
    const ComtradeCfgInfo &info, ComtradeData &dat)
{
    int standCnt = GetOneDataSize(info);
    if ((bufSize % standCnt) != 0)
        return false;
    // 如果直接用push_back函数,文件大时占用时间过久
    ComtradeOneData oneData;
    oneData.analog_ch_datas.resize(info.analog_ch_cnt);
    oneData.digital_ch_datas.resize(info.digital_ch_cnt);
    dat.resize(bufSize / standCnt);
    for (int i = 0; i < bufSize; )
    {
        int idx = 0;
        GetData(&buf[i], idx);
        i += 4;
        GetData(&buf[i], oneData.timestamp);
        i += 4;
        for (int j = 0; j < info.analog_ch_cnt; j++)
        {
            short sdata;
            GetData(&buf[i], sdata);
            i += 2;
            oneData.analog_ch_datas[j] = sdata;
        }
        for (int j = 0; j < info.digital_ch_cnt; j++)
        {
            if (j != 0 && j % 8 == 0)
                i++;
            bool ddata = ((buf[i] >> (j % 8)) & 0x01) == 1;
            oneData.digital_ch_datas[j] = ddata;
        }
        while (i % standCnt != 0)
            i++;
        dat[i / standCnt - 1] = oneData;
    }
    return true;
}

void CComtradeTool::GetData(const unsigned char *buf, int &idata)
{
    idata = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

void CComtradeTool::GetData(const unsigned  char *buf, short &sdata)
{
    sdata = buf[0] | (buf[1] << 8);
}

bool CComtradeTool::GetSubString(std::string src, char separator, int &lastPos,
    int &curPos, std::string &des)
{
    curPos = src.find(separator, lastPos);
    if (curPos == (int)string::npos)
        curPos = src.length();
    des = src.substr(lastPos, curPos - lastPos);
    lastPos = curPos + 1;
    return true;
}

int CComtradeTool::GetMaxIdx(const ComtradeCfgInfo &info)
{
    int maxIdx = 0;
    for (int i = 0; i != (int)info.sample_cfg_infos.size(); i++) {
        maxIdx += info.sample_cfg_infos.at(i).endsamp;
    }
    return maxIdx;
}

int CComtradeTool::GetOneDataSize(const ComtradeCfgInfo &info)
{
    // 编号(4字节) + 时标(4字节) + 模拟通道(2 * 通道数) + 状态通道(2 * n, 状态量不足偶数字节时需补0)
    return (4 + 4 + 2 * info.analog_ch_cnt +
            ((info.digital_ch_cnt % 8) == 0 ? info.digital_ch_cnt / 8 : info.digital_ch_cnt / 8 + 1));
}
