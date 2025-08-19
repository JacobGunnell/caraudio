#include "delayBuffer.h"
#include <iostream>

#define FRAMES 10


void print(float *in, float *out, unsigned int nFrames) {
    std::cout << "in: ";
    for (int i = 0; i < nFrames; ++i) {
        std::cout << in[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "out: ";
    for (int i = 0; i < nFrames; ++i) {
        std::cout << out[i] << ", ";
    }
    std::cout << std::endl;
}

int main() {
    DelayBuffer delayBuffer(30);
    float in1[FRAMES] = {1,1,1,1,1,1,1,1,1,1};
    float in2[FRAMES] = {2,2,2,2,2,2,2,2,2,2};
    float in3[FRAMES] = {3,3,3,3,3,3,3,3,3,3};
    float in4[FRAMES] = {4,4,4,4,4,4,4,4,4,4};
    float out[FRAMES];

    delayBuffer.delay(out, in1, FRAMES);
    print(in1, out, FRAMES);
    std::cout << "avg level: " << delayBuffer.getAvgLevel() << std::endl;
    delayBuffer.delay(out, in2, FRAMES);
    print(in2, out, FRAMES);
    std::cout << "avg level: " << delayBuffer.getAvgLevel() << std::endl;
    delayBuffer.delay(out, in3, FRAMES);
    print(in3, out, FRAMES);
    std::cout << "avg level: " << delayBuffer.getAvgLevel() << std::endl;
    delayBuffer.delay(out, in4, FRAMES);
    print(in4, out, FRAMES);
    std::cout << "avg level: " << delayBuffer.getAvgLevel() << std::endl;
    delayBuffer.delay(out, in1, FRAMES);
    print(in1, out, FRAMES);
    std::cout << "avg level: " << delayBuffer.getAvgLevel() << std::endl;
    delayBuffer.delay(out, in2, FRAMES);
    print(in2, out, FRAMES);
    std::cout << "avg level: " << delayBuffer.getAvgLevel() << std::endl;

    return 0;
}