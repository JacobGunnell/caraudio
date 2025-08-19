#pragma once

#include <vector>
#include <cstddef>


class DelayBuffer {
public:
    DelayBuffer(size_t _bufferSize);

    void delay(float *out, float *in, unsigned int nFrames);
    float getAvgLevel() { return avgLevel; } // Get the average level of all frames in the delay buffer

private:
    float total(float *samples, unsigned int nFrames);

    std::vector<float> buffer;
    size_t bufferSize;
    size_t bufferIdx;

    float avgLevel; // average level, computed at each delay() call and retrievable via getAvgLevel()
};