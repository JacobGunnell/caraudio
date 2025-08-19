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
    float in1[FRAMES] = {1,2,3,4,5,6,7,8,9,10};
    float in2[FRAMES] = {11,12,13,14,15,16,17,18,19,20};
    float in3[FRAMES] = {21,22,23,24,25,26,27,28,29,30};
    float in4[FRAMES] = {31,32,33,34,35,36,37,38,39,40};
    float out[FRAMES];

    delayBuffer.delay(out, in1, FRAMES);
    print(in1, out, FRAMES);
    delayBuffer.delay(out, in2, FRAMES);
    print(in2, out, FRAMES);
    delayBuffer.delay(out, in3, FRAMES);
    print(in3, out, FRAMES);
    delayBuffer.delay(out, in4, FRAMES);
    print(in4, out, FRAMES);
    delayBuffer.delay(out, in1, FRAMES);
    print(in1, out, FRAMES);
    delayBuffer.delay(out, in2, FRAMES);
    print(in2, out, FRAMES);

    return 0;
}