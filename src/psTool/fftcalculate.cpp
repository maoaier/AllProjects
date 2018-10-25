#include "fftcalculate.h"
#include <complex>

CFftCalculate::CFftCalculate()
{

}

CFftCalculate::~CFftCalculate()
{

}

bool CFftCalculate::CalculateFFT(const QVector<double> &src, int sample, int cycle, FFT_ONCE_DATA &des)
{
    int fftCnt = sample*cycle;
    if (src.size() < fftCnt)
        return false;
    m_fftw_in = new float[fftCnt];
    m_fftw_out = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*fftCnt);
    m_fftw_plan = fftwf_plan_dft_r2c_1d(fftCnt, m_fftw_in, m_fftw_out, FFTW_ESTIMATE);

    for (int i = 0; i < src.size(); i++) {
        if (i != 0 && (i % fftCnt == 0)) {
            // 计算一次fft
            fftwf_execute(m_fftw_plan);
            _calcOnceFFT(50, sample, cycle, des);
            break;
        }
        m_fftw_in[i%fftCnt] = src.at(i);
    }

    if (m_fftw_plan)
        fftwf_destroy_plan(m_fftw_plan);
    if (m_fftw_in)
        delete[] m_fftw_in;
    if (m_fftw_out)
        fftwf_free(m_fftw_out);
    return true;
}

bool CFftCalculate::_calcOnceFFT(int datCnt, int sample, int cycle, FFT_ONCE_DATA &once)
{
    once.clear();
    float fftCnt = sample*cycle;
    float base_ang = 0;
    for (int i = 0; i != datCnt; i++) {
        int idx = i*cycle;
        std::complex<float> o(m_fftw_out[idx][0], m_fftw_out[idx][1]);
        FFT_DATA tmp;
        tmp.rms = std::abs(o)/(fftCnt/2.0)*M_SQRT1_2;
        tmp.ang = (std::arg(o) + M_PI_2)*180/M_PI;
        if (i == 1)
            base_ang = tmp.ang;
        tmp.ang -= base_ang*i;
        tmp.ang = tmp.ang - (int)(tmp.ang/360)*360;
        if (tmp.ang > 180)
            tmp.ang -= 360;
        else if (tmp.ang < -180)
            tmp.ang += 360;
        once.push_back(tmp);
    }
    return true;
}
