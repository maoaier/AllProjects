#ifndef CFFTCALCULATE_H
#define CFFTCALCULATE_H
#include <QVector>
#include "fftw3.h"

typedef struct _FFT_DATA
{
    double rms;
    double per;
    double ang;
}FFT_DATA;

typedef QVector<FFT_DATA> FFT_ONCE_DATA;

class CFftCalculate
{
public:
    CFftCalculate();
    virtual ~CFftCalculate();
    bool CalculateFFT(const QVector<double> &src, int sample, int cycle, FFT_ONCE_DATA &des);

protected:
    bool _calcOnceFFT(int datCnt, int sample, int cycle, FFT_ONCE_DATA &once);

protected:
    float *m_fftw_in;    // 计算FFT的输入点数
    fftwf_complex *m_fftw_out;   // 计算FFT的输出点数
    fftwf_plan m_fftw_plan;      // 计算FFT所必须的中间变量
};

#endif // CFFTCALCULATE_H
