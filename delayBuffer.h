#pragma once

#include <vector>
#include <cstddef>


class DelayBuffer {
public:
    DelayBuffer(unsigned int _sampleRate, unsigned int _chunkFrames, float _delayInSeconds);
    DelayBuffer(size_t _chunkFrames, size_t _bufferChunks);

    void delay(float *out, float *in, unsigned int nFrames);
    float getAvgLevel() { return avgLevel; } // Get the average level of all frames in the delay buffer

private:
    float total(float *samples, unsigned int nFrames);

    std::vector<float> buffer;
    size_t bufferSize; // total size of buffer (in frames)
    size_t chunkFrames; // frames per chunk
    size_t bufferIdx; // r/w index within the ringbuffer

    std::vector<float> levels;
    size_t bufferChunks;
    size_t levelIdx;

    float avgLevel; // average level, computed at each delay() call and retrievable via getAvgLevel()
};
