#ifndef FFTOPERATION_H
#define FFTOPERATION_H
#include "fftw3.h"
#include <unistd.h>
#include <QDebug>
class FFTOperation
{
public:
    FFTOperation();
    void run();
};

#endif // FFTOPERATION_H
