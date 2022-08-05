#ifndef FILTER
#define FILTER


const int RES_OK = 0;
const int ERROR_PTR = -1;
const int ERROR_SIZE= -2;
const int ERROR_FILTER_A0= -3;



float filter(float sample, float alfa);

double biquadSection(  double x, double b0,double b1,double b2,
                                 double a0,double a1,double a2  );



#endif