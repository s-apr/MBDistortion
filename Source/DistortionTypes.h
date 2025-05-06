/*
  ==============================================================================

    DistortionTypes.h
    Created: 5 May 2025 9:10:53pm
    Author:  maxbu

  ==============================================================================
*/

#pragma once

enum class DistortionTypes {
    None,
    HardClip,
    SoftClip,
    ExpDistortion,
    CubicClip,
    Arctangent,
    Asymmetric,
    FullRectify,
    HalfRectify
};