#ifndef GLOBAL
#define GLOBAL
#include <QString>

#define SHEET1 "电压报表"
#define SHEET2 "电流报表"
#define SHEET3 "电能质量报表"
#define TITLE1 "电能质量统计报表(电压)"
#define TITLE2 "电能质量统计报表(电流)"
#define TITLE3 "电能质量统计报表"

//现场工作状态记录结构图
struct CWorkingCondition
{
    QString monitoingPsoition;//监测位置
    QString monitoingTime;//监测时间
    QString monitoingLine;//监测线路
    QString fieldOperator;//现场操作员
    QString voltageLevel;//电压等级
    QString dataPT;//PT
    QString dataCT;//CT
    QString referenceShortCircuitCpy;//基准短路容量
    QString minimumShortCircuitCpy;//最小短路容量
    QString userAgreementCpy;//用户协议容量
    QString powerSuppluEquipmentCpy;//供电设备容量
    QString workCondition;//工况
};

//电压电能结构图
struct CVoltageQuality
{
    //现场工作状态记录结构图
    CWorkingCondition workingCondition;

    //26组第一维数组在数组分别代表着位置
    //基波电压（V）【0~0】
    //电压谐波含有率(%)【1~24】
    //电压谐波总畸变率(％)【25~25】

    //3组第二维数组在数组分别代表着位置
    //L1【0~0】
    //L2【1~1】
    //L3【2~2】

    //6组第三维别为，最大值，平均值，最小值，95%值，结论。
    QString voltageData[26][3][5];

    //26组的标准值
    //基波电压（V）【0~0】
    //电压谐波含有率(%)【1~24】
    //电压谐波总畸变率(％)【25~25】
    QString voltageDataStandard[26];
};


//电流质量结构图
struct CCurrentQuality
{
    //现场工作状态记录结构图
    CWorkingCondition workingCondition;

    //26组第一维数组在数组分别代表着位置
    //基波电流（A）【0~0】
    //电流谐波有效值(A)【1~24】

    //3组第二维数组在数组分别代表着位置
    //L1【0~0】
    //L2【1~1】
    //L3【2~2】
    QString currentData[25][3][5];

    //26组的标准值
    //基波电压（V）【0~0】
    //电压谐波含有率(%)【1~24】
    QString currentDataStandard[25];
};

//电能质量结构体
struct CPowerQuality
{
    //现场工作状态记录结构图
    CWorkingCondition workingCondition;

    //27组二维数组在数组分别代表着位置
    //频率（Hz）【0~0】
    //电压负序不平衡（％）【1~1】
    //短时间闪变【2~4】（L1~L3）
    //长时间闪变【5~7】（L1~L3）
    //电压偏差(%) (负号表示下偏差)【8~10】（L1~L3）
    //有功功率(kW)【11~14】（L1~总）
    //无功功率(kvar)【15~18】 （L1~总）
    //视在功率(kVA)【19~22】（L1~总）
    //功率因数【23~26】（L1~总）
    //第2二维依次表示（最大值，平均值，最小值，95％值，标准，结论）
    QString powerData[27][6];
};


#endif // GLOBAL

