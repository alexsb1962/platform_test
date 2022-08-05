#include "filter.h"
#include "math.h"


double biquadSection(  double x, double b0,double b1,double b2,
                                 double a0,double a1,double a2  ){
    static bool first=true;
    static double innerX[3]={0,0,0}, innerY[3]={0,0,0};
    double y;
    if(first){
      // только для фнч !!!!!!
      first = false;
      innerX[0]=x; innerX[1]=x; innerX[2]=x; 
      innerY[0]=x; innerY[1]=x; innerY[2]=x; 
    }

    y = b0*x + b1*innerX[1]  + b2*innerX[2] - a1*innerY[1] -a2*innerY[2];
    y/=a0;

    innerX[2]=innerX[1]; innerX[1]=innerX[0]; innerX[0]=x;
    innerY[2]=innerY[1]; innerY[1]=innerY[0]; innerY[0]=y;
    
    return y;
}

int filter_iir(double *b, double *a, int ord, double *x, int n, double *y){
    double *buf = nullptr;    double *an = nullptr;
    double u;
    int k;    int m;    int count;

    if (!b || !x || !y)   return ERROR_PTR;
    if (ord < 1 || n < 1) return ERROR_SIZE;
    if (a && a[0] == 0.0) return ERROR_FILTER_A0;

    count = ord + 1;
    buf = new double[count];
    an  = new double[count];
    for(int i=0;i<count;i++)   buf[i]=0;
    
    if (!a) for(int i=0;i<count;i++)   an[i]=0;
    else
        for (k = 0; k < count; k++)  an[k] = a[k] / a[0];

    for (k = 0; k < n; k++)  {
        for (m = ord; m > 0; m--)  buf[m] = buf[m - 1];
        u = 0.0;
        for (m = ord; m > 0; m--)  u += buf[m] * an[m];

        buf[0] = x[k] - u;
        y[k] = 0.0;
        for (m = 0; m < count; m++)  y[k] += buf[m] * b[m];
    }
    
    delete[] buf;
    delete[] an;

    return RES_OK;
}

float filter(float sample, float alfa){
  static float old_y=0;
  static bool first=true;
  float y;

  if(first){
    old_y=sample;
    y=sample; first = false;
    return y;
  }
  else{
    y = alfa * sample + (1.0 - alfa) * old_y;
    old_y = y;
    return y;
  }
}
