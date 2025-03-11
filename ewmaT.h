#ifndef _EWMAT_H
#define _EWMAT_H

/*
 * Filter template that allows restriction to only one specific data type, such as uint32_t. Avoiding floating point arithmetics 
 * can significantly decrease code footprint, especially in embeded devices, such as Arduino or STM32.
 * 
 * If you want to create a filter for integers, with an alpha value of 0.03, you will just do the following:
 *
 *    ewmaT <int> filter(3, 100)
 * 
 */
template <typename T>
class ewmaT {
  public:
      /*
      * Creates a filter with a defined initial output.
      */
      ewmaT(T alpha, unsigned int alphaScale);
      /*
      * Creates a filter with a defined initial output.
      */
      ewmaT(T alpha, unsigned int alphaScale, T initialOutput);

      void reset();

      T output();

      /*
      * Specifies a reading value.
      * @returns current output
      */
      T filter(T input);

  private:
      void init(T alpha, unsigned int alphaScale, T initialOutput);

      /*
      * Smoothing factor, in range [0,alphaScale]. Higher the value - less smoothing (higher the latest reading impact).
      */
      T alpha;
      T outputScaled;
      unsigned int alphaScale;
      bool hasInitial;
};

#endif