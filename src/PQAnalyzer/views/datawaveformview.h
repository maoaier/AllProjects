#ifndef DATAWAVEFORMVIEW_H
#define DATAWAVEFORMVIEW_H

#include "viewbase.h"
#include "qcustomplot.h"
#include "statdatatool.h"

#define SAMPLING_RATE 128
//下拉框类型
typedef enum _WAVEFORM_TYPE
{
    VOLTAGE_WAVEFORM=10000,
    CURRENT_WAVEFORM
}WAVEFORM_TYPE;

//数据波形显示
class CDataWaveformView:public QFrame, IViewBase
{
    Q_OBJECT
public:
    explicit CDataWaveformView(QWidget *parent = 0);
    virtual ~CDataWaveformView();

public slots:
    //鼠标滚动事件
     virtual void on_plot_mouseWheel(QWheelEvent *event);
    void run();
    //相位改变
    void on_phaseType_checkStateChanged(int);
protected:
    //获取波形点
    void _getWavePoint();
    //读取波形数据
    //初始化线条
    virtual void _initGraph();
    //初始化数据
    void setRootPath(QString path);
    virtual void _initCtrls();
    //获取统计数据从文件中
    bool _getStatDataFromFile(QString statFilePath, StatDataPoints *points);
    //绘制波形数据
    void _plotWaveData(const StatDataPoints &points);
protected:

    QCustomPlot *m_pCustomPlot;  // 画图控件
    QLabel *m_pLabDataSel;          // 数据项选择标题
    QComboBox *m_pCombDataSel;      // 数据项选择下拉框
    QCheckBox *m_pChecPhaseType[4]; // ABCN总勾选框
    int m_iWaveNumber;//周波数
    StatDataPoints m_curWaveDataPoints[2];//波形数据点，电压波形，电流波形
    QMap<QString,int> m_mapItemType;//项目类型
};

#endif // DATAWAVEFORMVIEW_H


