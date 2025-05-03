/*
  ==============================================================================

    FilterClasses.h
    Created: 3 May 2025 1:16:08pm
    Author:  maxbu

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

//filter base interface
class Filter {
public:
    virtual double process(double input) = 0;
    virtual void updateCoefs() = 0;
    virtual void reset();
    virtual void setSampleRate(double sampleRate);
    virtual void setCutoff(double cutoff);

protected:
    double mSampleRate = 44100.0;
    double mCutoff = 1000;
};

//biquad bass class
class BiQuad : public Filter{
public:
    double process(double input) override;
    void reset() override;

protected:
    //coefficients
    double b0, b1, b2;
    double a0, a1, a2;
    //filter states
    double x1, x2; 
    double y1, y2;
};

//butterworth filters
class ButterworthLowPass : public BiQuad {
public:
    void updateCoefs() override;
}; 

class ButterworthHighPass : public BiQuad {
public:
    void updateCoefs() override;
};

//Linkwitz-Riley Filters
class LinkwitzRileyLowPass : public Filter {
public:
    void setSampleRate(double sampleRate) override;
    void setCutoff(double cutoff) override;
    double process(double input) override;
    void updateCoefs() override;
    void reset() override;

private:
    ButterworthLowPass filter1, filter2;
};

class LinkwitzRileyHighPass : public Filter {
public:
    void setSampleRate(double sampleRate) override;
    void setCutoff(double cutoff) override;
    double process(double input) override;
    void updateCoefs() override;
    void reset() override;

private:
    ButterworthHighPass filter1, filter2;
};