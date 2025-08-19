#include "delayBuffer.h"
#include "config.h"

#include <vector>
#include <cstddef>
#include <cmath>
#include <iostream>


DelayBuffer::DelayBuffer(size_t _bufferSize) : bufferSize(_bufferSize) {
    buffer.reserve(bufferSize);
    bufferIdx = 0;
    avgLevel = 0;
}

void DelayBuffer::delay(float *out, float *in, unsigned int nFrames) {
    std::vector<float>::iterator begin = buffer.begin() + bufferIdx;
    std::vector<float>::iterator end = buffer.begin() + bufferIdx + nFrames*CHANNELS;

    std::copy(begin, end, out);
    std::copy(in, in + nFrames*CHANNELS, begin);

    avgLevel += (total(in, nFrames) - total(out, nFrames)) / bufferSize;
    if (std::signbit(avgLevel)) avgLevel = 0.0f; // don't allow level to go negative

    bufferIdx = (bufferIdx + nFrames*CHANNELS) % bufferSize;
}

float DelayBuffer::total(float *samples, unsigned int nFrames) {
    float total = 0.0f;
    for (int i = 0; i < nFrames*CHANNELS; ++i) {
        total += std::fabs(samples[i]);
    }
    return total;
}
