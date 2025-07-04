/*
  ==============================================================================

    DistortionProcessor.cpp
    Created: 5 May 2025 9:10:31pm
    Author:  maxbu

  ==============================================================================
*/

#define _USE_MATH_DEFINES

#include "DistortionProcessor.h"
#include <cmath>
#include <algorithm>

void DistortionProcessor::setDistortionType(DistortionTypes newType) {
    type = newType;
    reset();
}

void DistortionProcessor::reset() {
    //empty but maybe used later for state types
}

float DistortionProcessor::processSample(float input) {
    switch (type) {
    case DistortionTypes::None: return input;
    case DistortionTypes::HardClip: return hardClip(input);
    case DistortionTypes::SoftClip: return softClip(input);
    case DistortionTypes::ExpDistortion: return expDistortion(input);
    case DistortionTypes::CubicClip: return cubicSoftClip(input);
    case DistortionTypes::Arctangent: return arctangentClip(input);
    case DistortionTypes::Asymmetric: return asymmetricClip(input);
    case DistortionTypes::FullRectify: return fullRectify(input);
    case DistortionTypes::HalfRectify: return halfRectify(input);
    default: return input;
    }
}

//DAFx distortion algorithms
float DistortionProcessor::hardClip(float input) {
    const float threshold = 1.0f;
    return std::clamp(input, -threshold, threshold);
}

float DistortionProcessor::softClip(float input) {
    return input / (1.0f + std::abs(input));
}

float DistortionProcessor::expDistortion(float input) {
    const float G = 5.0f;
    return std::copysign(1.0f - std::exp(-G * std::abs(input)), input);
}

float DistortionProcessor::cubicSoftClip(float input) {
    return std::clamp(1.5f * input - 0.5f * input * input * input, -1.0f, 1.0f);
}

float DistortionProcessor::arctangentClip(float input) {
    const float G = 5.0f;
    return std::atan(G * input) / std::atan(G);
}

float DistortionProcessor::asymmetricClip(float input) {
    const float G = 5.0f;
    const float H = 2.0f;
    if (input >= 0.0f)
        return removeDC(std::atan(G * input) / std::atan(G));
    else
        return removeDC(std::atan(G * H * input) / std::atan(G * H));
}

float DistortionProcessor::fullRectify(float input) {
    return removeDC(std::abs(input));
}

float DistortionProcessor::halfRectify(float input) {
    return removeDC(std::max(0.0f, input));
}

float DistortionProcessor::removeDC(float input){
    dcEstimate = dcAlpha * dcEstimate + (1.0f - dcAlpha) * input;
    return input - dcEstimate;
}