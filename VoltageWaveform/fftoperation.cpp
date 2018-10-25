#include "fftoperation.h"

FFTOperation::FFTOperation()
{

}

void FFTOperation::run()
{
   fftwf_complex *in, *out;
   fftwf_plan p;
   int N= 8;
   int i;
   int j;

   in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
   out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
   for( i=0; i < N; i++)
   {
       in[i][0] = 1.0;
       in[i][1] = 0.0;
       printf("%6.2f ",in[i][0]);
   }
   printf("\n");
   p=fftwf_plan_dft_1d(N,in,out, FFTW_FORWARD, FFTW_ESTIMATE);
   fftwf_execute(p); /* repeat as needed*/
   for(j = 0;j < N;j++)
   {
       printf("%6.2f ",out[j][0]);
   }
   printf("\n");
   fftwf_destroy_plan(p);
   fftwf_free(in);
   fftwf_free(out);
   sleep(5);
   return ;

//    fftw_complex *in,*out;
//    fftw_plan p;
//    int N=8;
//    int i;
//    int j;
//    in=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
//    out=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
//    for(i=0;i<N;i++)
//    {
//        in[i][0]=1.0;
//        in[i][1]=0.0;
//        printf("%6.2f ",in[i][0]);
//    }
//    printf("\n");
//    p=fftw_plan_dft_1d(N,in,out,FFTW_FORWARD,FFTW_ESTIMATE);
//    fftw_execute(p);
//    for(j=0;j<N;j++)
//    {
//        printf("%6.2f ",out[j][0]);
//    }
//    printf("\n");
//    fftw_destroy_plan(p);
//    fftw_free(in);
//    fftw_free(out);
//    sleep(5);
//    return ;
}

