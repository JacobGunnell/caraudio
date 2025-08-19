#include <iostream>
#include <cmath>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <rtaudio/RtAudio.h>

#define CHANNELS 2


// Level control
void levelControl(float *samples, unsigned int nFrames) {
    const float target = 2e-2f;
    const float smoothing = 5e-6f;
    const float maxVolume = 8.0f;
    static float volume = 1.0f;
    static float level = target;

    for (unsigned int i = 0; i < nFrames * CHANNELS; ++i) {
        level = smoothing*std::abs(samples[i]) + (1.0f-smoothing)*level;
        volume = target / std::max(level, target/maxVolume); // don't let volume go above 10
        samples[i] *= volume;
    }

    //std::cout << volume << "," << level << std::endl;
}

int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double /*streamTime*/, RtAudioStreamStatus status, void *userData) {
    if (status) std::cerr << "Stream underflow detected!" << std::endl;

    float *out = static_cast<float *>(outputBuffer);
    float *in = static_cast<float *>(inputBuffer);

    std::copy(in, in + nFrames * CHANNELS, out);

    // DSP chain
    levelControl(out, nFrames);

    return 0;
}

std::atomic<bool> run(true);
void signalHandler(int) {
    run = false;
}

int main() {
    RtAudio audio;
    if (audio.getDeviceCount() < 1) {
        std::cerr << "No audio devices found!" << std::endl;
        return 1;
    }

    // Find bluetooth loopback and line out devices
    int loopbackId = -1, outputId = -1;
    RtAudio::DeviceInfo info;
    for (int i = 0; i < audio.getDeviceCount(); i++) {
        info = audio.getDeviceInfo(i);
        if (info.probed) {
            std::cout << "Device " << i << ": " << info.name << std::endl;
            if (info.name == "Monitor of Built-in Audio Analog Stereo") loopbackId = i;
            if (info.name == "Built-in Audio Stereo") outputId = i;
        }
    }

    //loopbackId = 2; //audio.getDefaultInputDevice();
    //outputId = 1; //audio.getDefaultOutputDevice();

    if (loopbackId == -1 || outputId == -1) {
        std::cerr << "Could not find loopback and/or output device." << std::endl;
        return 1;
    }

    std::cout << "Connecting to input device " << loopbackId << ", output device " << outputId << std::endl;

    RtAudio::StreamParameters inParams, outParams;
    inParams.deviceId = loopbackId;
    inParams.nChannels = CHANNELS; // stereo
    outParams.deviceId = outputId; // USB DAC
    outParams.nChannels = CHANNELS;

    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256;

    try {
        audio.openStream(
            &outParams, &inParams, RTAUDIO_FLOAT32,
            sampleRate, &bufferFrames, &audioCallback
        );
        audio.startStream();
    }
    catch (RtAudioError& e) {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }

    std::cout << "Streaming audio... Press Ctrl-C to quit." << std::endl;
    while (run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Shutting down DSP..." << std::endl;

    try {
        audio.stopStream();
    }
    catch (RtAudioError& e) {
        std::cerr << e.getMessage() << std::endl;
    }

    if (audio.isStreamOpen()) audio.closeStream();
    return 0;
}
