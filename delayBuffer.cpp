#include "delayBuffer.h"


DelayBuffer::DelayBuffer(size_t _bufferSize) : bufferSize(_bufferSize) {
    buffer.reserve(bufferSize);
    idx = 0;
}

void DelayBuffer::delay(float *out, float *in, unsigned int nFrames) {
    std::vector<float>::iterator begin = buffer.begin() + idx;
    std::vector<float>::iterator end = buffer.begin() + idx + nFrames;

    std::copy(begin, end, out);
    std::copy(in, in + nFrames, begin);

    idx = (idx + nFrames) % bufferSize;
}