#ifndef CSTATDATATOOL_H
#define CSTATDATATOOL_H

#include <vector>
#include <string>
#include "module_base_def.h"
#include "configfile.h"

struct cJSON;

/** @struct
 *  @brief 统计选择项,是否进行统计保存
 */
typedef struct _StatRecordItem
{
public:
    _StatRecordItem() {
        rms[0] = rms[1] = true;
        ang[0] = ang[1] = true;

        pos_peak[0] = pos_peak[1] = true;
        neg_peak[0] = neg_peak[1] = true;

        harm_dc_rms[0] = harm_dc_rms[1] = true;
        harm_dc_per[0] = harm_dc_per[1] = true;
        harm_thd_rms[0] = harm_thd_rms[1] = true;
        harm_thd_per[0] = harm_thd_per[1] = true;

        for (int i = 0; i != HARM_MAX; i++) {
            harm_rms[0][i] = harm_rms[1][i] = true;
            harm_per[0][i] = harm_per[1][i] = true;
            harm_ang[0][i] = harm_ang[1][i] = true;
            harm_power[i] = true;
        }

        for (int i = 0; i != INTER_HARM_MAX; i++) {
            inter_harm_rms[0][i] = inter_harm_rms[1][i] = false;
            inter_harm_per[0][i] = inter_harm_per[1][i] = false;
        }

        for (int i = 0; i != HIGH_HARM_MAX; i++) {
            high_harm_rms[0][i] = high_harm_rms[1][i] = false;
            high_harm_per[0][i] = high_harm_per[1][i] = false;
        }

        active_power = reactive_power = \
                apparent_power = power_factor = true;

        pos_value[0] = pos_value[1] = true;
        neg_value[0] = neg_value[1] = true;
        zero_value[0] = zero_value[1] = true;
        neg_unbalance[0] = neg_unbalance[1] = true;
        zero_unbalance[0] = zero_unbalance[1] = true;

        frequency = true;
    }

    bool rms[2];
    bool ang[2];
    bool pos_peak[2];
    bool neg_peak[2];

    bool harm_dc_rms[2];
    bool harm_dc_per[2];
    bool harm_thd_rms[2];
    bool harm_thd_per[2];

    bool harm_rms[2][HARM_MAX];
    bool harm_per[2][HARM_MAX];
    bool harm_ang[2][HARM_MAX];
    bool harm_power[HARM_MAX];

    bool inter_harm_rms[2][INTER_HARM_MAX];
    bool inter_harm_per[2][INTER_HARM_MAX];

    bool high_harm_rms[2][HIGH_HARM_MAX];
    bool high_harm_per[2][HIGH_HARM_MAX];

    bool active_power;
    bool reactive_power;
    bool apparent_power;
    bool power_factor;

    bool pos_value[2];
    bool neg_value[2];
    bool zero_value[2];
    bool neg_unbalance[2];
    bool zero_unbalance[2];

    bool frequency;
}StatRecordItem;

/** @struct
 *  @brief 统计保存信息
 */
typedef struct _StatRecordInfo
{
    DateTime startTime;     // 统计开始时间
    DateTime endTime;       // 统计结束时间
    DateTime firstTime;     // 第一个统计数据的时间
    int statCycle;          // 统计间隔,秒数,-1位十周波统计间隔
    int statTotalCnt;       // 总统计点数
    StatRecordItem recItem; // 统计选择项
    MeasureConfig meaCfg;   // 测量参数
}StatRecordInfo;

typedef enum _StatFuncType
{
    Stat_Func_None = 0,
    Stat_Func_Write,
    Stat_Func_Read
}StatFuncType;

typedef struct _StatOneData
{
    std::vector<float> max_val;     // 最大值
    std::vector<float> min_val;     // 最小值
    std::vector<float> avg_val;     // 平均值
    std::vector<float> cp95_val;    // 95%概率大值
}StatOneData;
typedef std::vector<StatOneData> StatDatas;

class CStatDataTool
{
public:
    CStatDataTool();
    virtual ~CStatDataTool();

public:
    bool InitWriteFile(const std::string &fileName);
    bool WriteDataToFile(const StatDatas &dat);
    bool InitReadFile(const std::string &fileName, int datCount);
    bool ReadDataFromFile(int begIdx, int endIdx, StatDatas &dat);

public:
    bool WriteInfoToFile(const std::string &fileName, const StatRecordInfo &info);
    bool ReadInfoFromFile(const std::string &fileName, StatRecordInfo &info);

protected:
    /** @brief 保存统计信息文件
     *  @param[in]      item        统计选择项
     *  @return Json格式对象
     */
    cJSON* _statRecItemToJson(const StatRecordItem &item);
    /** @brief 解析统计信息文件
     *  @param[in]      node        json节点
     *  @param[out]     item        统计选择项
     *  @return Json格式对象
     */
    bool _jsonTostatRecItem(cJSON *node, StatRecordItem &item);

    /** @brief 从Json格式对象中获取测量参数
     *  @param[in]      obj     Json格式对象
     *  @param[out]     cfg     测量参数
     *  @return true -- 获取成功
     */
    bool _jsonObjToMeasureCfg(cJSON *obj, MeasureConfig &cfg);

    /** @brief 将测量参数转为Json格式对象
     *  @param[in]      cfg     需转化的测量参数
     *  @return Json格式对象
     */
    cJSON* _measureCfgToJsonObj(const MeasureConfig &cfg);

protected:
    int m_funcType;     // 读写StatDatas功能
    int m_datCount;     // StatOneData数据中vector的数组个数
    std::string m_datFileName;
};

#endif // CSTATDATATOOL_H
