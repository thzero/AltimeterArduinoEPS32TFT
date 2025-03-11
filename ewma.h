#ifndef _EWMA_H
#define _EWMA_H

class ewma {
  public:
    /*
    * Current data output
    */
    double output = 0;
    /*
    * Smoothing factor, in range [0,1]. Higher the value - less smoothing (higher the latest reading impact).
    */
    double alpha = 0;

    /*
    * Creates a filter without a defined initial output. The first output will be equal to the first input.
    */
    ewma(double alpha);

    /*
    * Creates a filter with a defined initial output.
    */
    ewma(double alpha, double initialOutput);

    void reset();

    /*
    * Specifies a reading value.
    * @returns current output
    */
    double filter(double input);

  private:
    bool hasInitial = false;
};

#endif