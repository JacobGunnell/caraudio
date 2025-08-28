#pragma once

#include <vector>
#include <cstddef>


class DelayBuffer {
public:
    DelayBuffer(unsigned int _sampleRate, unsigned int _chunkFrames, float _delayInSeconds, float _avgWindowInSeconds);
    DelayBuffer(size_t _chunkFrames, size_t _bufferChunks, size_t _avgWindowChunks);

    void delay(float *out, float *in, unsigned int nFrames);
    float getAvgLevel() { return avgWindowTotal / bufferSize; } // Get the average level of all frames in the averaging window
    float getLastInChunkLevel() { return chunkSums[(chunkSumsIdx + avgWindowChunks - 1) % avgWindowChunks] / chunkFrames; }
    float getNextOutChunkLevel() { return chunkSums[chunkSumsIdx] / chunkFrames; }
    bool isMusicPlaying(float threshold) { return getLastInChunkLevel() > threshold && getNextOutChunkLevel() > threshold; }

private:
    float total(float *samples, unsigned int nFrames);

    std::vector<float> buffer; // buffer to hold audio frames for delay
    size_t bufferSize; // total size of buffer (in frames)
    size_t chunkFrames; // frames per chunk
    size_t bufferIdx; // r/w index within the audio frame ringbuffer

    std::vector<float> chunkSums; // array holding sum of each chunk to be used in level averaging calculation
    size_t avgWindowChunks; // number of chunks to be used in averaging window
    size_t chunkSumsIdx; // r/w index within the chunkSums ringbuffer

    float avgWindowTotal; // total level of all frames in averaging window, computed at each delay() call
};
