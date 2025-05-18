/*
  ==============================================================================

    DistortionProcessor.h
    Created: 5 May 2025 9:10:44pm
    Author:  maxbu

  ==============================================================================
*/

#pragma once
#include "DistortionTypes.h"

class DistortionProcessor {
public:
    DistortionProcessor() = default;
    void setDistortionType(DistortionTypes newType);
    float processSample(float input);
    void reset();
private:
    //dc removal
    float dcEstimate = 0.0f;
    float dcAlpha = 0.999f;


    //initaliser
    DistortionTypes type = DistortionTypes::SoftClip;

    float hardClip(float input);
    float softClip(float input);
    float expDistortion(float input);
    float cubicSoftClip(float input);
    float arctangentClip(float input);
    float asymmetricClip(float input);
    float fullRectify(float input);
    float halfRectify(float input);
    float removeDC(float input);

    //states for other types of dist
};