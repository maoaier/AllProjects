#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "qcustomplot.h"
#include <math.h>
#define STANDARD_PHASE 0 //初始化相位
#define STANDARD_AMPLITUDE 220 //初始化幅值
#define SAMPLING_POINT 128 //初始化采样点
#define SHOW_WAVE_NUMBER 1;//显示波的条数

//谐波数据结构体
struct CHarmonic
{
    int m_iNumber;
    float m_fAmplitude;
    float m_fPhase;
    float m_fRange;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots:
    void on_pushBtn_show_clicked();
    void on_pushBuAdd_clicked();

    void on_pushBuClear_clicked();

private:
    void initParameter();
    void initInterface();
    void initTest();
    void samplingData(QVector<double> &x,QVector<double> &y);
private:
    Ui::MainWindow *ui;
    float m_fAmplitude;//幅值
    float m_fPhase;//相位
    int m_iSamplingPoint;//采样点
    int m_iWaveNumber;//显示几条波形
    QVector <CHarmonic> m_cHarmonicData;//保存增加的谐波
    bool m_bHarmonicState;//谐波的状态
    QCPGraph *m_pWaveLine;//主要的线
};

#endif // MAINWINDOW_H
