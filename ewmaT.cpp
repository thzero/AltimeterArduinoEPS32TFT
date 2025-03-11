#include "ewmaT.h"

template <typename T>
ewmaT<T>::ewmaT(T alpha, unsigned int alphaScale) {
    init(alpha, alphaScale, 0);
    this->hasInitial = false;
}

template <typename T>
ewmaT<T>::ewmaT(T alpha, unsigned int alphaScale, T initialOutput) {
    init(alpha, alphaScale, initialOutput);
}

template <typename T>
void ewmaT<T>::init(T alpha, unsigned int alphaScale, T initialOutput) {
    this->alpha = alpha;
    this->alphaScale = alphaScale;
    this->outputScaled = initialOutput * alphaScale;
    this->hasInitial = true;
}

template <typename T>
void ewmaT<T>::reset() {
    this->hasInitial = false;
}

template <typename T>
T ewmaT<T>::filter(T input) {
    if (hasInitial) {
        outputScaled = alpha * input + (alphaScale - alpha) * outputScaled / alphaScale;
    } else {
        outputScaled = input * alphaScale;
        hasInitial = true;
    }
    return output();
}

template <typename T>
T ewmaT<T>::output() {
    return (outputScaled + alphaScale / 2) / alphaScale;
}