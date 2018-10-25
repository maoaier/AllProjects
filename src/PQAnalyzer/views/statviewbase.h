#ifndef CSTATVIEWBASE_H
#define CSTATVIEWBASE_H

#include <QMap>
#include "viewbase.h"

class QLabel;
class QCheckBox;
class QComboBox;

class CStatViewBase : public QFrame, IViewBase
{
    Q_OBJECT
public:
    friend class CReadDataThread;

public:
    explicit CStatViewBase(QWidget *parent = 0);
    virtual ~CStatViewBase();

    /** @brief  设置统计文件根目录路径,并解析当前数据,会阻塞
     *  @param  [in]    path        统计文件根目录路径
     */
    virtual void setRootPath(QString path);

    /** @brief  重载事件函数,实现鼠标移动显示标签功能
     *  @param  [in]    event       系统事件
     *  @return true/false
     */
    virtual bool event(QEvent *event);

    /** @brief  重载加载第一项数据并显示
     */
    virtual void refreshStatData();

signals:
    /** @brief  更新界面消息
     */
    void updateCurStatData();

public slots:
    /** @brief  鼠标滚轮消息函数
     *  @param  [in]    event       鼠标滚轮消息
     */
    virtual void on_plot_mouseWheel(QWheelEvent *event);

    /** @brief  数据项选择下拉框,选择消息函数
     *  @param  [in]    index       下拉框最新选择的行号
     */
    virtual void on_datSel_combIndexChanged(int index);

    /** @brief  数据类型选择下拉框,选择消息函数
     *  @param  [in]    index       下拉框最新选择的行号
     */
    virtual void on_datType_combIndexChanged(int index);

    /** @brief  勾选框,选择消息函数
     *  @param  [in]    state       勾选框选中状态
     */
    virtual void on_phaseType_checkStateChanged(int state);

    /** @brief  更新界面函数
     */
    virtual void on_update_cur_statdata();

protected:
    /** @brief  初始化控件
     */
    virtual void _initCtrls();

    /** @brief  选中不同的数据项,重新缓存当前数据
     *  @param  [in]    index       数据项下拉框最新选择的行号,对应m_mapFileName的序号
     */
    virtual void _updateDiffStatData(int index);

    /** @brief  在控件中画数据
     *  @param  [in]    infos       线条的信息,如名称,颜色...
     *  @param  [in]    points      线条每个点的位置
     */
    virtual void _plotStatData(const StatGraphInfos &infos, const StatDataPoints &points);

    /** @brief  根据统计文件的名称来返回有多少项Phase数据,ABCN总,最多5项
     *  @param  [in]    statFileName    统计文件名称
     *  @return phase数据的个数
     */
    virtual int _getStatPhaseCount(QString statFileName);

    /** @brief  从统计文件中获取统计数据
     *  @param  [in]    statFileName    统计文件名称
     *  @param  [out]   points          统计数据
     *  @return true/false
     */
    virtual bool _getStatDataFromFile(QString statFilePath, StatDataPoints *points);

    /** @brief  将统计数据点的信息转换为字符串,用于标签显示内容
     *  @param  [in]    infos       线条的信息,如名称,颜色...
     *  @param  [out]   point       统计数据点信息
     *  @return 返回统计数据点的信息字符串
     */
    virtual QString _getStatPointInfo(const StatGraphInfos &infos, const StatOnePoint &point);

protected:
    QCustomPlot *m_pCustomPlot;     // 画图控件
    QLabel *m_pLabDataSel;          // 数据项选择标题
    QComboBox *m_pCombDataSel;      // 数据项选择下拉框
    QLabel *m_pLabDataType;         // 数据类型选择标题
    QComboBox *m_pCombDataType;     // 数据类型选择下拉框
    QCheckBox *m_pChecPhaseType[5]; // ABCN总勾选框

    StatDataPoints m_curStatDataPoints[4];  // 最大,最小,平均,cp95
    QMap<QString, StatItemInfo> m_mapFileName;   // 统计文件项对应的中文
};

#endif // CSTATVIEWBASE_H
