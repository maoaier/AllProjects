#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "qcustomplot.h"
#include <math.h>
#include "fftw3.h"
#include "fftoperation.h"

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
public slots:
    void on_plot_mouseWheel(QWheelEvent *event);

private slots:
    void on_pushBtn_show_clicked();

    void on_pushBuAdd_clicked();

    void on_pushBuClear_clicked();

    void on_actionFFTW_triggered();

    void on_pushButton_clicked();

    void on_customPlot_3_destroyed();

    void on_helloworld_clicked();

private:
    void initParameter();//初始化参数
    void initInterface();//初始化界面
    void endProcessing(int state=0);//结尾处理
    void initTest();
    void samplingData();//采样数据
    void showInterface();//显示界面
private:
    Ui::MainWindow *ui;
    enum
    {
        M_WAVE_FOEM=10010,
        M_SPECTRUM
    };
    QVector <double> m_dXData,m_dYData;//波形数据
    QVector <double> m_dXSpectrum,m_dYSpectrum;
    float m_fAmplitude;//幅值
    float m_fPhase;//相位
    int m_iSamplingRate;//采样率
    int m_iWaveNumber;//显示几条波形
    QVector <CHarmonic> m_cHarmonicData;//保存增加的谐波
    bool m_bHarmonicState;//谐波的状态
    QCPGraph *m_pWaveLine;//主要的线
    FFTOperation fftOperation;//FFT操作
};

#endif // MAINWINDOW_H
