#ifndef EXECL_H
#define EXECL_H
#include <QAxObject>
#include <QDebug>
#include <unistd.h>
#include <QFileDialog>
#include <QString>
#include <time.h>

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

    //25组第一维数组在数组分别代表着位置
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


/*************************************返回值代表的情况**********************************************
bool createATemplate(QString name="C:\\Users\\xuantong\\Desktop\\Template");
return -2 表格已经打开，或者是否覆盖表格按了否，无法导出表格。
return -1 初始化COM库错误
return 0 创建表格成功


int exportForm(CVoltageQuality &voltageData, CCurrentQuality &currentData,
CPowerQuality &powerData, QString name="C:\\Users\\xuantong\\Desktop\\PowerQuality");
return -1 模板不存在
return -2 模板格式不对p
return -3 表格已经打开，或者是否覆盖表格按了否，表格创建失败
return -4;初始化COM库错误
return 0 创建表格成功

bool exportFormless(CVoltageQuality &voltageData, CCurrentQuality &currentData,
                CPowerQuality &powerData, QString name="C:\\Users\\xuantong\\Desktop\\PowerQuality");
return -2 表格已经打开，或者是否覆盖表格按了否，无法导出表格。
return -1 初始化COM库错误
return 0 创建表格成功
**************************************************************************************************/




//电能execl报表导出类
class CExecl
{
public:
    //关闭execl
    ~CExecl();
    //打开execl
    CExecl();
    //导出报表,使用模板，时间只要2~5s,平均3s
    int exportForm(CVoltageQuality &voltageData, CCurrentQuality &currentData,
                    CPowerQuality &powerData, QString name="C:\\Users\\xuantong\\Desktop\\PowerQuality");

    //创建模板，同时设置模板名
    int createATemplate(QString name="C:\\Users\\xuantong\\Desktop\\Template");

    //导出报表，不用模板，时间5~7s
    int exportFormless(CVoltageQuality &voltageData, CCurrentQuality &currentData,
                        CPowerQuality &powerData, QString name="C:\\Users\\xuantong\\Desktop\\PowerQuality");
    //存在模板，设置模板名
    void setTemplateName(QString name);
    //测试数据
    CVoltageQuality test_1();
    CCurrentQuality test_2();
    CPowerQuality test_3();

private:
    //用模板init
    int templateInit();
    //设置自动适应宽度
    void setWidthHeight();
    //结尾配置execl表格
    bool EndConfiguration(QString strName,bool adapt=true);
    //初始化execl表格固定数据
    int initialization();

    //写入数据操作
    void writeDataControl_1(CVoltageQuality &testData);
    void writeDataControl_2(CCurrentQuality &currentData);
    void writeDataControl_3(CPowerQuality &powerData);
    //固定化数据操作
    void initDataControl_1();
    void initDataControl_2();
    void initDataControl_3();

    void initDataRed(QAxObject * &workSheet, int row, int column);              //初始化数据
    void initData(QAxObject * &workSheet,int row, int column, const char *data);              //初始化数据

    //表格操作
    void addSheet(QAxObject * &pSheet,const char *sheetName);           //增加表格
    void sheetControl();//表格控制
    void sheetConfiguration();//表格配置

    //合并表格
    void mergeCellControl_1();                                              //合并表格控制
    void mergeCellControl_2();
    void mergeCellControl_3();

    void mergeTitle(QAxObject * &workSheet, const char* title, int column=18);//合并标题
    void mergeCellOPeration(QAxObject * &workSheet, int row1, int column1, int row2, int column2,
        const char *data, bool middle );                          //合并表格操作
    void mergeCellOPeration(QAxObject * &workSheet, int row1, int column1, int row2, int column2,
         const char *data);
    void mergeCellOPeration(QAxObject * &workSheet, int row1, int column1, int row2, int column2);

    void strToCharText(char ch[], QString &str);                //Qstring转换为char型
    void strToChar(char ch[], QString &str);

    void listAdd(QList<QVariant> &rowData, const char *data);
    //    int close();关闭表格

    QAxObject *m_cWorkBooks;
    QAxObject *m_cExcel;//
    QAxObject *m_cWorkSheets;//总表格
    QAxObject *m_cWorkSheet;//表格1
    QAxObject *m_cWorkSheet2;//表格2
    QAxObject *m_cWorkSheet3;//表格3
    QAxObject *m_cWorkBook;
    QAxObject *m_cTmpWorkBook;
    QString m_cTemplateName;//模板名字
};

#endif // EXECL_H



