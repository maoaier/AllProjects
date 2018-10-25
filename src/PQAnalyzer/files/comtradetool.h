/** @file
 *  @brief comtrade文件读写类
 *  @author 吴嘉明
 *  @date 2017-06-15
 *  @version 0.1
 */
#ifndef __COMTRADETOOL_H__
#define __COMTRADETOOL_H__

#include <vector>
#include <string>
#include "module_base_def.h"

typedef struct _AnalogCfgInfo
{
    int an;				// 模拟通道索引号
    std::string ch_id;	// 模拟通道识别号
    std::string ph;		// 模拟通道相位特征
    std::string ccbm;	// 被监视的回路元件
    std::string uu;		// 通道单位
    float a;			// 通道乘数
    float b;			// 通道偏移加数
    float skew;			// 通道时滞(us)
    int min;			// 通道数据值的最小值
    int max;			// 通道数据值的最大值
    float primary;		// 通道一次变比系数
    float secondary;	// 通道二次变比系数
    std::string ps;		// 通道通过转换系数(ax+b)得到的是一次值(P)还是二次值(S)
}AnalogCfgInfo;

typedef struct _DigitalCfgInfo
{
    int dn;				// 数字通道索引号
    std::string ch_id;	// 数字通道识别号
    std::string ph;		// 数字通道相位特征
    std::string ccbm;	// 被监视的回路元件
    bool y;				// 输入状态,0或1,true-1
}DigitalCfgInfo;

typedef struct _SampleCfgInfo
{
    float samp;			// 采样率(Hz)
    int endsamp;		// 当前采样率所采集的点数
}SampleCfgInfo;

typedef enum _ComtradeDataType
{
    Com_Data_Ascii = 0,
    Com_Data_Binary,
}ComtradeDataType;

typedef enum _ComtradeFuncType
{
    Com_Func_None = 0,
    Com_Func_Write,
    Com_Func_Read
}ComtradeFuncType;

typedef struct _ComtradeCfgInfo
{
    _ComtradeCfgInfo() {
        rev_year = 1999;
        total_ch_cnt = analog_ch_cnt = digital_ch_cnt = nrates = timemult = 0;
        freq = 0;
        dataType = Com_Data_Binary;
    }
    // 站名，记录装置的特征，COMTRADE标准的修改年份
    std::string station_name;	// 站位置名称
    std::string rec_dev_id;		// 装置的名称
    int rev_year;				// COMTRADE标准修改年份

    // 通道的数量和类型
    int total_ch_cnt;	// 通道的总数量，TT
    int analog_ch_cnt;	// 模拟通道数，##A
    int digital_ch_cnt;	// 数字通道数，##D

    // 模拟通道信息
    std::vector<AnalogCfgInfo> analog_cfg_infos;

    // 数字通道信息
    std::vector<DigitalCfgInfo> digital_cfg_infos;

    // 线路频率
    float freq;

    // 采样率信息
    int nrates;		// nrates个不同的采样率
    std::vector<SampleCfgInfo> sample_cfg_infos;


    // 日期/时间标记
    UtcTime begTime;	// 第一个数据点的时间
    UtcTime triTime;	// 触发点的时间

    // 数据文件类型
    ComtradeDataType dataType;

    // 时间标记乘数系数,us
    int timemult;
}ComtradeCfgInfo;

//XIUGAI 录播需求
typedef struct _ComtradeOneData
{
    std::vector<short> analog_ch_datas;	// 模拟通道数据,补码数据
    std::vector<bool> digital_ch_datas;	// 数字通道数据,0或1,true-1
    int timestamp;						// 时间标记,补码数据
}ComtradeOneData;

typedef std::vector<ComtradeOneData> ComtradeData;

/** @class
 *  @brief COMTRADE文件格式读写类
 */
class CComtradeTool
{
public:
    CComtradeTool();
    virtual ~CComtradeTool();

    /** @brief 初始化写文件功能
     *  @param[in]      fileName    COMTRADE文件名,要具体路径,不需后缀
     *  @param[in]      cfg         录波配置信息
     *  @return true -- 初始化成功
     */
    bool InitWriteFile(const std::string &fileName, const ComtradeCfgInfo &cfg);

    /** @brief 将波形数据写入COMTRADE的数据文件当中
     *  @param[in]      dat         录波数据
     *  @return true -- 写入成功
     */
    bool WriteDataToFile(const ComtradeData &dat);

    /** @brief 初始化读文件功能
     *  @param[in]      fileName    COMTRADE文件名,要具体路径,不需后缀
     *  @param[out]     cfg         录波配置信息
     *  @return true -- 读文件成功
     */
    bool InitReadFile(const std::string &fileName, ComtradeCfgInfo &cfg);

    /** @brief 将波形数据从COMTRADE的数据文件中读取出来
     *  @param[in]      begIdx      COMTRADE文件名,要具体路径,不需后缀
     *  @param[in]      endIdx      COMTRADE文件名,要具体路径,不需后缀
     *  @param[out]     cfg         录波配置信息
     *  @return true -- 读文件成功
     */
    bool ReadDataFromFile(int begIdx, int endIdx, ComtradeData &dat);

protected:
    /** @brief 将配置信息写入到cfg文件中
     *  @param[in]      fileName    COMTRADE文件名,要具体路径,不需后缀
     *  @param[in]      cfg         录波配置信息
     *  @return true -- 写入成功
     */
    bool WriteInfoFile(const std::string &fileName, const ComtradeCfgInfo &cfg);

protected:
    // 由于10分钟1024点8通道数据过大,不能直接将10分钟数据进行读写,因此先不开放以下两读写接口
    /** @brief 写COMTRADE录波数据文件,会产生.cfg(配置)以及.dat(数据)两个文件
     *  @param[in]      fileName    COMTRADE文件名,要具体路径,不需后缀
     *  @param[in]      cfg         录波配置信息
     *  @param[in]      dat         录波数据
     *  @return true -- 写文件成功
     */
    bool WriteComtradeFile(const std::string &fileName, const ComtradeCfgInfo &cfg, const ComtradeData &dat);

    /** @brief 从COMTRADE录波数据文件获取配置信息以及波形数据
     *  @param[in]      fileName    COMTRADE文件名,要具体路径,不需后缀
     *  @param[out]     cfg         录波配置信息
     *  @param[out]     dat         录波数据
     *  @return true -- 读文件成功
     */
    bool ReadComtradeFile(const std::string &fileName, ComtradeCfgInfo &cfg, ComtradeData &dat);

protected:
    bool ChangeCfgToString(const ComtradeCfgInfo &info, std::string &buf);
    char* ChangeDataToBuf(const ComtradeCfgInfo &info, int startIdx, const ComtradeData &dat, int &bufSize);
    char* ChangeDataToBufByAscii(const ComtradeCfgInfo &info, int startIdx, const ComtradeData &dat, int &bufSize);
    char* ChangeDataToBufByBinary(const ComtradeCfgInfo &info, int startIdx, const ComtradeData &dat, int &bufSize);

protected:
    bool AnalyseBufToCfgInfo(const std::vector<std::string> &buf, ComtradeCfgInfo &info);
    bool AnalyseBufToData(const unsigned char *buf, int bufSize, const ComtradeCfgInfo &info, ComtradeData &dat);

protected:
    int PushData(int idata, char *buf);
    int PushData(short sdata, char *buf);
    int PushData(std::vector<bool> bdata, char *buf);

protected:
    bool GetSubString(std::string src, char separator, int &lastPos, int &curPos, std::string &des);
    void GetData(const unsigned char *buf, int &idata);
    void GetData(const unsigned char *buf, short &sdata);

protected:
    int GetMaxIdx(const ComtradeCfgInfo &info);
    int GetOneDataSize(const ComtradeCfgInfo &info);

protected:
    int m_funcType;
    int m_writeIdx;
    std::string m_fileName;
    ComtradeCfgInfo m_cfgInfo;
};

#endif
