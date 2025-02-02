#ifndef _KALMAN_H
#define _KALMAN_H

extern void KalmanInit();
extern float kalmanCalc (float altitude);

class kalman {
  public:
    kalman();
    float kalmanCalc(float);
    void setQ(float);
    void setR(float);
    
  private:
    // Kalman Variables
    float f_1 = 1.00000; // cast as float
    float kalman_x;
    float kalman_x_last;
    float kalman_p;
    float kalman_p_last;
    float kalman_k;
    float kalman_q;
    float kalman_r;
    float kalman_x_temp;
    float kalman_p_temp;
};

#endif