#include <stdio.h>
#include "filter.h"
#include "fdacoefs.h"




double biquadSection(  double x,  double b0,  double b1,  double b2,
                                  double a0,  double a1,  double a2  ){
    static bool first=true;
    static double innerX[3];
    static double innerY[3];
    double y;  double w;

    if(first){
        first = false;
        for(int i=0;i<=2;i++){ innerX[0]=0; innerY[0]=0; }
    }

    y = b0*x + b1*innerX[1]+ b2*innerX[2] ;
    y-= a1*innerY[1] + a2*innerY[2] ;
    y/=a0;
    
    innerX[2] = innerX[1]; innerX[1] = innerX[0]; innerX[0] = x ;  
    innerY[2] = innerY[1]; innerY[1] = innerY[0]; innerY[0] = y ;  

    return y;
}

int main(void){
    double y,x;

    for(int i=0; i<100; i++){
      // y = biquadSection(1.,     1.,  2., 1. , 
      //                           0.000944691843840150748297379568185760945 ,   -1.911197067426,   0.9149758348014 );
        y=biquadSection(1.,         0.1, 0.0, 0.,
                                    1.0,  -0.9, 0.0);
       printf("%f ",y);
    }

    printf("Ok\n");
}

