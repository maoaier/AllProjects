#ifndef CEVENTDATATOOL_H
#define CEVENTDATATOOL_H

#include "module_base_def.h"
#include <deque>
#include <string>
#include <string.h>

/** @struct CInfoToolBase
 *  @brief 事件信息读写基类
 */
class CInfoToolBase
{
public:
    CInfoToolBase() {}
    virtual ~CInfoToolBase() {}

protected:
    const char* _readAllFileContent(std::string fileName);
    bool _writeToFile(std::string fileName, const char *buf, int len);
};

/////////////////////////////////////////////////////////////////////

/** @enum TransEventInfo
 *  @brief 暂态事件类型
 */
typedef enum _TransEventType
{
    TE_NONE = -1,           // 无暂态事件
    TE_VOLT_SWELL,          // 电压暂升
    TE_VOLT_SAG,            // 电压暂降
    TE_VOLT_INTERRUPT,      // 电压中断
    TE_CURR_SHOCK,          // 冲击电流

    MAX_TRANS_EVENT
}TransEventType;

/** @struct TransEventInfo
 *  @brief 暂态事件信息
 */
typedef struct _TransEventInfo
{
    _TransEventInfo() {
        eventType = TE_NONE;
        eventWaveCnt = 0;
        connType = 0;
        sample = 128;
        sysFreq = 50;
        eventLimit = 0;
        memset(paramK, 0, sizeof(paramK));
        memset(paramB, 0, sizeof(paramB));
        for (int i = PHASE_A; i != PHASE_N; i++) {
            eventVal[i] = 0;
            eventPhase[i] = false;
        }
    }
    std::string meapos;         // 测量地点
    UtcTime waveStaTime;        // 波形开始时间
    UtcTime waveEndTime;        // 波形结束时间
    UtcTime eventStaTime;       // 事件开始时间
    UtcTime eventEndTime;       // 事件结束时间
    int eventType;              // 事件类型
    int eventWaveCnt;           // 事件波形点数
    int sample;                 // 采样率,n点/周波
    int connType;               // 接线方式
    float sysFreq;              // 系统频率
    float eventLimit;           // 事件判断阈值
    float eventVal[PHASE_N];    // 事件特征值,事件幅度最大值
    float paramK[2][MAX_PHASE]; // 波形校准参数
    float paramB[2][MAX_PHASE]; // 波形校准参数
    bool eventPhase[PHASE_N];   // 事件发生相序
}TransEventInfo;
typedef std::deque<TransEventInfo> TransEventInfos;

/** @struct TransEventPoint
 *  @brief 暂态事件单点数据
 */
typedef struct _TransEventPoint
{
    float potWave[2][MAX_PHASE];
    UtcTime potTime;
}TransEventPoint;
typedef std::deque<TransEventPoint> TransEventWave;

/** @struct TransEventInfo
 *  @brief 暂态事件信息读写类
 */
class CTransInfoTool : public CInfoToolBase
{
public:
    CTransInfoTool() {}
    virtual ~CTransInfoTool() {}
    bool SaveEventInfo(std::string filePath, const TransEventInfos &infos);
    bool ReadEventInfo(std::string filePath, TransEventInfos &infos);
    bool SaveEventWave(std::string filePath, const TransEventInfo &info, const TransEventWave &wave);
    bool ReadEventWave(std::string filePath, const TransEventInfo &info, TransEventWave &wave);
};

/////////////////////////////////////////////////////////////////////

/** @enum SteadyEventType
 *  @brief 稳态越限事件类型
 */
typedef enum _SteadyEventType
{
    SE_NONE = -1,               // 无稳态事件
    SE_FREQ_UP_LIMIT,           // 频率上越限
    SE_FREQ_DOWN_LIMIT,         // 频率下越限
    SE_VOLT_UP_LIMIT,           // 电压上越限
    SE_VOLT_DOWN_LIMIT,         // 电压下越限
    SE_VOLT_UNBALANCE_LIMIT,    // 电压不平衡越限
    SE_PST_LIMIT,               // 短闪变越限
    SE_PLT_LIMIT,               // 长闪变越限
    SE_VOLT_THD_LIMIT,          // 电压THD越限
    SE_VOLT_HARM_LIMIT,         // 电压谐波越限
    SE_CURR_HARM_LIMIT,         // 电流谐波越限

    MAX_STEADY_EVENT
}SteadyEventType;

/** @struct SteadyEventInfo
 *  @brief 稳态越限事件信息
 */
typedef struct _SteadyEventInfo
{
    _SteadyEventInfo() {
        eventType = SE_NONE;
        eventPhase = -1;
        eventHarmIdx = -1;
        eventVal = eventLimit = 0;
    }
    UtcTime startTime;  // 事件开始时间
    UtcTime endTime;    // 事件结束时间
    int eventType;      // 事件类型
    int eventPhase;     // 事件发生相序,-1-无相序,0-A,1-B,2-C
    int eventHarmIdx;   // 第N次谐波事件,-1-无谐波,2~n次谐波
    float eventVal;     // 事件越限时值
    float eventLimit;   // 事件越限阈值
}SteadyEventInfo;
typedef std::deque<SteadyEventInfo> SteadyEventInfos;

/** @struct TransEventInfo
 *  @brief 稳态事件信息读写类
 */
class CSteadyInfoTool : public CInfoToolBase
{
public:
    CSteadyInfoTool() {}
    virtual ~CSteadyInfoTool() {}
    bool SaveEventInfo(std::string filePath, const SteadyEventInfos &infos);
    bool ReadEventInfo(std::string filePath, SteadyEventInfos &infos);
};

#endif // CEVENTDATATOOL_H
