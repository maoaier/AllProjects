#ifndef MODULE_BASE_DEF
#define MODULE_BASE_DEF

#include <string>

/** @class
 *  @brief fifo,先进先出队列模板
 */
template<typename T>
class C_Fifo
{
public:
    C_Fifo(unsigned int fifoSize){
        m_fifoSize = fifoSize;
        m_writeIdx = m_readIdx = 0;
        m_pFifoArray = new T[m_fifoSize];
    }
    ~C_Fifo(){
        if (m_pFifoArray) {
            delete[] m_pFifoArray;
        }
    }

    bool WriteFifo(T *p) {
        if (m_writeIdx - m_readIdx >= m_fifoSize)
            return false;
        memcpy(&m_pFifoArray[m_writeIdx%m_fifoSize], p, sizeof(T));
        m_writeIdx++;
        return true;
    }

    bool ReadFifo(T *p) {
        if (m_readIdx == m_writeIdx)
            return false;
        memcpy(p, &m_pFifoArray[m_readIdx%m_fifoSize], sizeof(T));
        m_readIdx++;
        return true;
    }

    bool ClearFifo() {
        m_writeIdx = m_readIdx;
        return true;
    }

public:
    T *m_pFifoArray;
    unsigned int m_fifoSize;
    unsigned int m_writeIdx;
    unsigned int m_readIdx;
};

/** @class
 *  @brief 消息观察者模式
 */
class C_Observer
{
public:
    C_Observer(){}
    virtual ~C_Observer(){}
    virtual void HandleMessage(void *msgParam) = 0;
};

/** @class
 *  @brief 打印时间戳
 */
class TickTime
{
public:
    TickTime();
    ~TickTime();

protected:
    double freq;
    long long int begTick;
};

/** @class
 *  @brief 日期数据结构
 */
struct _UtcTime;
typedef enum _TOSTR_TYPE {
    TS_DATETIME,
    TS_DATE,
    TS_DATE_NODAY,
    TS_TIME,
    TS_TIME_NOMS
}TOSTR_TYPE;

typedef struct _DateTime
{
    _DateTime() {
        year = 0;
        month = 0;
        day = 0;
        hour = 0;
        minute = 0;
        second = 0;
        ms = 0;
    }

    bool operator ==(const _DateTime &rhs) {
        return (year == rhs.year && month == rhs.month && day == rhs.day &&
            hour == rhs.hour && minute == rhs.minute && second == rhs.second &&
            ms == rhs.ms);
    }
    _UtcTime toUtcTime() const;
    std::string toString(int tsType = TS_DATETIME) const;
    bool fromString(const char *strTime, int tsType = TS_DATETIME);
    bool toNowDateTime();

    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int ms;
}DateTime;

/** @class
 *  @brief UTC时间数据结构
 */
typedef struct _UtcTime
{
    _UtcTime(){
        tv_sec = 0;
        tv_usec = 0;
    }
    bool operator ==(const _UtcTime &rhs) {
        return (tv_sec == rhs.tv_sec && tv_usec == rhs.tv_usec);
    }
    DateTime toDateTime(bool bUtc = false) const;

    // utc时间,1970年1月1日至今
    long tv_sec;	// 秒数
    long tv_usec;	// 微秒数
}UtcTime;

/** @class
 *  @brief AD波形数据
 */
#define AD_SAMPLE   1024
typedef enum _PHASE_TYPE
{
    PHASE_A,
    PHASE_B,
    PHASE_C,
    PHASE_N,
    MAX_PHASE
}PHASE_TYPE;

typedef enum _BOARD_TYPE
{
    BOARD_0,
    MAX_BOARD
}BOARD_TYPE;
#define IS_BOARD_VALID(idx) (idx >= BOARD_0 && idx < MAX_BOARD)

typedef struct _AD_ORIGINAL_DATA
{
    float freq;
    short vol[MAX_PHASE][AD_SAMPLE];
    short cur[MAX_PHASE][AD_SAMPLE];
    int boardIdx;
    UtcTime endTime;
}AD_ORIGINAL_DATA;

typedef struct _AD_FLOAT_DATA
{
    float freq;
    float vol[MAX_PHASE][AD_SAMPLE];
    float cur[MAX_PHASE][AD_SAMPLE];
    int boardIdx;
    UtcTime endTime;
}AD_FLOAT_DATA;

/** @class
 *  @brief 产生模拟AD数据结构体
 */
#define HARM_MAX 63
#define INTER_HARM_MAX  HARM_MAX
#define HIGH_HARM_MAX   35
typedef struct _SimHarmInfo
{
    _SimHarmInfo(){
        dc = 0;
        fund = 0;
        for (int i = 0; i != HARM_MAX; i++)
        {
            per[i] = i == 0 ? 100.0f : 0.0f;
            ang[i] = 0;
        }
    }
    float dc;               // 直流分量
    float fund;				// 基波有效值
    float per[HARM_MAX];	// 1~n次谐波含有率
    float ang[HARM_MAX];	// 1~n次谐波相位角
}SimHarmInfo;

/** @struct
 *  @brief 实时数据结构体,十周波数据
 */
typedef struct _RealData
{
    float rms[2][MAX_PHASE];                // 有效值,0-电压,1-电流
    float ang[2][MAX_PHASE];                // 相位角
    float pos_peak[2][MAX_PHASE];           // 正峰值
    float neg_peak[2][MAX_PHASE];           // 负峰值

    float harm_dc_rms[2][MAX_PHASE];        // 直流分量幅值,0-电压,1-电流
    float harm_dc_per[2][MAX_PHASE];        // 直流分量,%
    float harm_thd_rms[2][MAX_PHASE];       // 总谐波畸变幅值
    float harm_thd_per[2][MAX_PHASE];       // 总谐波畸变,%

    float harm_rms[2][MAX_PHASE][HARM_MAX]; // 幅值
    float harm_per[2][MAX_PHASE][HARM_MAX]; // 含有率,%
    float harm_ang[2][MAX_PHASE][HARM_MAX]; // 相位角
    float harm_power[MAX_PHASE][HARM_MAX];  // 有功功率

    float inter_harm_rms[2][MAX_PHASE][INTER_HARM_MAX]; // 间谐波幅值
    float inter_harm_per[2][MAX_PHASE][INTER_HARM_MAX]; // 间谐波含有率,%

    float high_harm_rms[2][MAX_PHASE][HIGH_HARM_MAX];   // 高次谐波幅值
    float high_harm_per[2][MAX_PHASE][HIGH_HARM_MAX];   // 高次谐波含有率,%

    float active_power[5];      // ABCN总有功功率
    float reactive_power[5];    // 无功功率
    float apparent_power[5];    // 视在功率
    float power_factor[5];      // 功率因数,%

    float pos_value[2];         // 正序值,0-电压,1-电流
    float pos_ang[2];           // 正序相位
    float neg_value[2];         // 负序值
    float neg_ang[2];           // 负序相位
    float zero_value[2];        // 零序值
    float zero_ang[2];          // 零序相位
    float neg_unbalance[2];     // 负序不平衡度,%
    float zero_unbalance[2];    // 零序不平衡度,%
    float frequency;            // 频率

    int boardIdx;
    UtcTime endTime;
}RealData;

/** @class
 *  @brief 统计数据结构体
 */
typedef struct _StatisticsData
{
    RealData stat_max;  // 最大值
    RealData stat_min;  // 最小值
    RealData stat_avg;  // 平均值
    RealData stat_cp95; // cp95值
}StatisticsData;

/** @class
 *  @brief 短闪变结构图,10min
 */
typedef struct _SFlickData
{
    float pst[MAX_PHASE];

    int boardIdx;
    UtcTime endTime;
}SFlickData;

/** @class
 *  @brief 长闪变结构图,2hour
 */
typedef struct _LFlickData
{
    float plt[MAX_PHASE];

    int boardIdx;
    UtcTime endTime;
}LFlickData;

#endif // MODULE_BASE_DEF

