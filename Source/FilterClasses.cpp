/*
  ==============================================================================

    FilterClasses.cpp
    Created: 3 May 2025 1:16:01pm
    Author:  maxbu

  ==============================================================================
*/


#include "FilterClasses.h"

//=================filter base=================
void Filter::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
}

void Filter::setCutoff(double cutoff) {
    mCutoff = cutoff;
}

void Filter::reset() {};

//=================biquad=================
double BiQuad::process(double input) {
    //diff eq
    double y = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = y;
    return y;
}

void BiQuad::reset() {
    x1 = x2 = y1 = y2 = 0.0;
}

//=================butter worth filters=================
void ButterworthLowPass::updateCoefs() {
    double c = 1.0 / tan(M_PI * mCutoff / mSampleRate);
    double c2 = c * c;

    b0 = 1.0 / (1.0 + sqrt(2.0) * c + c2);
    b1 = 2.0 * b0;
    b2 = b0;
    a0 = 1.0;
    a1 = 2.0 * b0 * (1.0 - c2);
    a2 = b0 * (1.0 - sqrt(2.0) * c + c2);
}

void ButterworthHighPass::updateCoefs() {
    double c = tan(M_PI * mCutoff / mSampleRate);
    double c2 = c * c;

    b0 = 1.0 / (1.0 + sqrt(2.0) * c + c2);
    b1 = -2.0 * b0;
    b2 = b0;
    a0 = 1.0;
    a1 = 2.0 * b0 * (c2 - 1.0);
    a2 = b0 * (1.0 - sqrt(2.0) * c + c2);
}

//=================Linkwitz-Riley LowPass=================
void LinkwitzRileyLowPass::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    filter1.setSampleRate(sampleRate);
    filter2.setSampleRate(sampleRate);
    updateCoefs();
}

void LinkwitzRileyLowPass::setCutoff(double cutoff) {
    mCutoff = cutoff;
    filter1.setCutoff(cutoff);
    filter2.setCutoff(cutoff);
    updateCoefs();
}

double LinkwitzRileyLowPass::process(double input) {
    return filter2.process(filter1.process(input));
}

void LinkwitzRileyLowPass::updateCoefs() {
    filter1.updateCoefs();
    filter2.updateCoefs();
}

void LinkwitzRileyLowPass::reset() {
    filter1.reset();
    filter2.reset();
}

//=================Linkwitz-Riley HighPass=================
void LinkwitzRileyHighPass::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    filter1.setSampleRate(sampleRate);
    filter2.setSampleRate(sampleRate);
    updateCoefs();
}

void LinkwitzRileyHighPass::setCutoff(double cutoff) {
    mCutoff = cutoff;
    filter1.setCutoff(cutoff);
    filter2.setCutoff(cutoff);
    updateCoefs();
}

double LinkwitzRileyHighPass::process(double input) {
    return filter2.process(filter1.process(input));
}

void LinkwitzRileyHighPass::updateCoefs() {
    filter1.updateCoefs();
    filter2.updateCoefs();
}

void LinkwitzRileyHighPass::reset() {
    filter1.reset();
    filter2.reset();
}