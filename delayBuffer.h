#pragma once

#include <vector>
#include <cstddef>


class DelayBuffer {
public:
    DelayBuffer(size_t _bufferSize);

    void delay(float *out, float *in, unsigned int nFrames);
    float avgLevel(); // Get the average level of all frames in the delay buffer

private:
    std::vector<float> buffer;
    size_t bufferSize;
    size_t idx;
};