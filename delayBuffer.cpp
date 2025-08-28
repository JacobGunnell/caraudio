#include "delayBuffer.h"
#include "config.h"

#include <vector>
#include <cstddef>
#include <cmath>
#include <stdexcept>


/**
* Initialize a DelayBuffer object.
*
* @param _sampleRate Sample rate in Hz (usually 44100)
* @param _chunkFrames Number of frames per chunk (usually 256)
* @param _delayInSeconds Desired delay in seconds
* @param _avgWindowInSeconds Desired averaging window length in seconds
*
*/
DelayBuffer::DelayBuffer(unsigned int _sampleRate, unsigned int _chunkFrames, float _delayInSeconds, float _avgWindowInSeconds)
: DelayBuffer(_chunkFrames, std::floor(_delayInSeconds*_sampleRate/_chunkFrames), std::floor(_avgWindowInSeconds*_sampleRate/_chunkFrames)) {
    if (_delayInSeconds <= 0.0f)
        throw std::invalid_argument("Delay must be greater than 0.");
    if (_avgWindowInSeconds <= 0.0f)
        throw std::invalid_argument("Averaging window length must be greater than 0.");
}

/**
* Initialize a DelayBuffer object.
*
* @param _chunkFrames Number of frames per chunk (usually 256)
* @param _bufferChunks Number of chunks to include in the buffer per channel (1 second delay = 44100/256 ~ 172)
* @param _avgWindowChunks Number of chunks long to make the averaging window
*
*/
DelayBuffer::DelayBuffer(size_t _chunkFrames, size_t _bufferChunks, size_t _avgWindowChunks) {
    bufferSize = _chunkFrames * _bufferChunks * CHANNELS; // total buffer size is frames per chunk times number of chunks

    buffer.reserve(bufferSize);
    chunkFrames = _chunkFrames;
    bufferIdx = 0;

    chunkSums.reserve(_avgWindowChunks); // used to store average level of each chunk to avoid recomputing
    avgWindowChunks = _avgWindowChunks;
    chunkSumsIdx = 0;

    avgWindowTotal = 0;
}

void DelayBuffer::delay(float *out, float *in, unsigned int nFrames) {
    std::vector<float>::iterator begin = buffer.begin() + bufferIdx;
    std::vector<float>::iterator end = buffer.begin() + bufferIdx + nFrames*CHANNELS;

    std::copy(begin, end, out);
    std::copy(in, in + nFrames*CHANNELS, begin);

    float outTotal = chunkSums[chunkSumsIdx];
    float inTotal = total(in, nFrames);
    chunkSums[chunkSumsIdx] = inTotal;

    avgWindowTotal += inTotal - outTotal;
    if (std::signbit(avgWindowTotal)) avgWindowTotal = 0.0f; // don't allow level to go negative

    bufferIdx = (bufferIdx + chunkFrames*CHANNELS) % bufferSize;
    chunkSumsIdx = (chunkSumsIdx + 1) % avgWindowChunks;
}

float DelayBuffer::total(float *samples, unsigned int nFrames) {
    float total = 0.0f;
    for (int i = 0; i < nFrames*CHANNELS; ++i) {
        total += std::fabs(samples[i]);
    }
    return total;
}
