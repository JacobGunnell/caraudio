#include <iostream>
#include <cmath>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <fstream>
#include <rtaudio/RtAudio.h>

#include "config.h"
#include "delayBuffer.h"


typedef struct {
    float minDynamic;
    float dynamicRange;
    float maxVolume;
    float smoothing;
} dspcfg_t;
dspcfg_t dspCfg;

// Level control
void levelControl(float *samples, unsigned int nFrames, float level) {
    static float volume = 1.0f;
    volume = std::min(dspCfg.minDynamic/level + dspCfg.dynamicRange, dspCfg.maxVolume) * dspCfg.smoothing + volume * (1.0f-dspCfg.smoothing);

    for (unsigned int i = 0; i < nFrames * CHANNELS; ++i) {
        samples[i] *= volume;
    }

    //std::cout << volume << std::endl;
}

int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double /*streamTime*/, RtAudioStreamStatus status, void *userData) {
    if (status) std::cerr << "Stream underflow detected!" << std::endl;

    float *out = static_cast<float *>(outputBuffer);
    float *in = static_cast<float *>(inputBuffer);

    static DelayBuffer delayBuffer(SAMPLE_HZ, CHUNK_FRAMES, 1.0); // 1 second lookahead window
    delayBuffer.delay(out, in, nFrames);

    // DSP chain
    levelControl(out, nFrames, delayBuffer.getAvgLevel());

    return 0;
}

std::atomic<bool> run(true);
void signalHandler(int) {
    run = false;
}

int main() {
    // Read DSP config from file, if present
    std::ifstream dspCfgFile("/home/pi/caraudio/dsp.cfg", std::ifstream::in);
    if (dspCfgFile.is_open()) {
        std::cout << "Reading DSP config from dsp.cfg" << std::endl;
        dspCfgFile 
            >> dspCfg.minDynamic 
            >> dspCfg.dynamicRange 
            >> dspCfg.maxVolume 
            >> dspCfg.smoothing;
    }
    // If DSP config file is absent, use default values
    else {
        std::cout << "No dsp.cfg found, using defaults" << std::endl;
        dspCfg = {
            .minDynamic = 2e-2f,
            .dynamicRange = 0.5f,
            .maxVolume = 4.0f,
            .smoothing = 5e-3f
        };
    }
    dspCfgFile.close();

    std::cout << "minDynamic = " << dspCfg.minDynamic << ", dynamicRange = " << dspCfg.dynamicRange << ", maxVolume = " << dspCfg.maxVolume << ", smoothing = " << dspCfg.smoothing << std::endl;

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

    unsigned int sampleRate = SAMPLE_HZ;
    unsigned int chunkFrames = CHUNK_FRAMES;

    try {
        audio.openStream(
            &outParams, &inParams, RTAUDIO_FLOAT32,
            sampleRate, &chunkFrames, &audioCallback
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
