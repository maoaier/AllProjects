#ifndef CCONFIGFILE_H
#define CCONFIGFILE_H

#include "module_base_def.h"
#include <QString>

/** @enum
 *  @brief 功率算法选项
 */
typedef enum _POWER_ALGORITHM
{
    BUDEANUM,       // 传统的功率立方法
    IEEE_1459,      // 1459新功率算法
    MAX_ALGORITHM
}POWER_ALGORITHM;

/** @enum
 *  @brief 接线方式选项
 */
typedef enum _CONNECT_MODE
{
    ONE_PHASE_TWO_LINE,                 // 一相两线
    THREE_PHASE_THREE_LINE,             // 三相三线
    THREE_PHASE_THREE_LINE_TWO_CURRENT, // 三相三线两电流接线
    THREE_PHASE_FOUR_LINE,              // 三相四线

    MAX_CONNECT_MODE,
}CONNECT_MODE;

/** @enum
 *  @brief 测量系统频率选项
 */
typedef enum _SYS_FREQ_MODE
{
    FREQ_50HZ,      // 50Hz系统
    FREQ_60HZ,      // 60Hz系统
    MAX_FREQ_MODE
}SYS_FREQ_MODE;

/** @enum
 *  @brief 电压量程选型
 */
typedef enum _VOLTAGE_RANGE
{
    VOLT_RANGE_10V,
    VOLT_RANGE_100V,
    VOLT_RANGE_400V,
    VOLT_RANGE_1000V,
    MAX_VOLT_RANGE,
}VOLTAGE_RANGE;

/** @enum
 *  @brief 电流钳型号
 */
typedef enum _CLAMP_TYPE
{
    CLAMP_5A,
    CLAMP_500A,
    CLAMP_3000A,
    MAX_CLAMP_TYPE,
}CLAMP_TYPE;

/** @enum
 *  @brief 系统电压等级
 */
typedef enum _VOLT_LEVEL
{
    VOLT_LEVEL_380V,
    VOLT_LEVEL_3KV,
    VOLT_LEVEL_6KV,
    VOLT_LEVEL_10KV,
    VOLT_LEVEL_20KV,
    VOLT_LEVEL_35KV,
    VOLT_LEVEL_66KV,
    VOLT_LEVEL_110KV,
    VOLT_LEVEL_220KV,
    VOLT_LEVEL_330KV,
    VOLT_LEVEL_500KV,
    VOLT_LEVEL_750KV,
    VOLT_LEVEL_1000KV,
    MAX_VOLT_LEVEL,
}VOLT_LEVEL;

/** @enum
 *  @brief 后台通信方式
 */
typedef enum _COMM_TYPE
{
    COMM_TYPE_INTERNET,
    COMM_TYPE_4G,
    MAX_COMM_TYPE
}COMM_TYPE;

/** @class
 *  @brief 测量配置参数
 */
typedef struct _MeasureConfig
{
public:
    /** @brief 配置默认的事件参数
     *  @param[in]      fq          系统频率
     *  @param[in]      volnor      标称电压
     *  @param[in]      curnor      标称电流
     *  @param[in]      vl          系统电压等级
     *  @param[in]      mc          最小短路容量
     *  @return true -- 配置成功
     */
    bool ToDefaultEventParam(int fq, float volnor, float curnor, int vl, float mc);
    bool IsDiffEventParam(const _MeasureConfig &other);
    QString VoltLevelToString(int voltLevel);

public:
    QString measurePos; // 测量点信息
    int freqMode;       // 系统频率
    int connectMode;    // 接线方式
    int powerAlgorithm; // 功率算法

    // 测量电压电流参数
    float voltNominal;  // 标称电压
    int voltRange;      // 电压量程,根据标称电压自动识别
    float currNominal;  // 标称电流
    int clampType;      // 电流钳型号
    float pt;           // A~C相电压变比
    float npt;          // N相电压变比
    float ct;           // A~C相电流变比
    float nct;          // N相电流变比

    // 稳态事件判断参数
    int voltLevel;          // 电压等级
    float miniCapacity;     // 最小短路容量,MVA
    float freqUpLimit;      // 频率上偏差阈值,Hz
    float freqDownLimit;    // 频率下偏差阈值,Hz
    float voltUpLimit;      // 电压上偏差阈值,%
    float voltDownLimit;    // 电压下偏差阈值,%
    float voltUnbaLimit;    // 电压不平衡越限阈值,%
    float pstLimit;         // 短闪变越限阈值,无单位
    float pltLimit;         // 长闪变越限阈值,无单位
    float voltThdLimit;     // 总谐波畸变率越限阈值,%
    float voltOddHarmLimit;     // 奇次谐波电压含有率越限阈值,%
    float voltEvenHarmLimit;    // 偶次谐波电压含有率越限阈值,%
    float currHarmLimit[24];    // 2~25次谐波电流幅值越限阈值,A

    // 暂态事件判断参数
    float voltSwell;        // 电压暂升阈值,%
    float voltSwellLag;     // 电压暂升迟滞值,%
    float voltSag;          // 电压暂降阈值,%
    float voltSagLag;       // 电压暂降迟滞值,%
    float voltInterrupt;    // 电压中断阈值,%
    float voltInterruptLag; // 电压中断迟滞值,%
    float currShock;        // 冲击电流阈值,%
    float currShockLag;     // 冲击电流迟滞值,%

    // 校准参数
    float voltCalcK[MAX_VOLT_RANGE][MAX_PHASE];     // 不同量程的电压校准k系数
    float voltCalcB[MAX_VOLT_RANGE][MAX_PHASE];     // 不同量程的电压校准b系数
    float currCalcK[MAX_PHASE];                     // 电流校准k系数
    float currCalcB[MAX_PHASE];                     // 电流校准b系数
}MeasureConfig;

#endif // CCONFIGFILE_H
