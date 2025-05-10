/*
  ==============================================================================

    OscilloscopeComponent.h
    Created: 10 May 2025 4:49:21pm
    Author:  maxbu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class RingBuffer; 

class OscilloscopeComponent : public juce::Component,
    private juce::Timer
{
public:
    OscilloscopeComponent(RingBuffer& bufferToUse)
        : ringBuffer(bufferToUse)
    {
        displayBuffer.resize(ringBuffer.size());
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        int height = getBounds().getHeight();
        int midHeight = height / 2;
        int scale = 90;
        int width = getBounds().getWidth();
        if (displayBuffer.size() > 0) {
            int step = static_cast<int>(displayBuffer.size() / width);
            int xPos = 0;
            int yPos = midHeight - displayBuffer[0] * scale;
            int nextX, nextY;
            for (int i = step; i < displayBuffer.size(); i += step) {
                nextX = xPos + 1;
                nextY = midHeight - displayBuffer[i] * scale;
                g.drawLine(xPos, yPos, nextX, nextY);
                xPos = nextX;
                yPos = nextY;
            }
        }
    }

private:
    void timerCallback() override
    {
        while (ringBuffer.hasNewData()) {
            displayBuffer[mPos] = ringBuffer.read();
            mPos++;
            if (mPos >= displayBuffer.size()) {
                repaint();
                mPos = 0;
                break;
            }
        }
    }

    int mPos = 0;
    RingBuffer& ringBuffer;
    std::vector<float> displayBuffer;
};