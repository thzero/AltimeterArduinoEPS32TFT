#include "ewma.h"

ewma::ewma(double alpha) {
	this->alpha = alpha;
}

ewma::ewma(double alpha, double initialOutput) {
	this->alpha = alpha;
	this->output = initialOutput;
	this->hasInitial = true;
}

void ewma::reset() {
	this->hasInitial = false;
}

double ewma::filter(double input) {
	if (hasInitial) {
		output = alpha * (input - output) + output;
	} else {
		output = input;
		hasInitial = true;
	}
	return output;
}