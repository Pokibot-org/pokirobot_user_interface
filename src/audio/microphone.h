#ifndef MICROPHONE_H
#define MICROPHONE_H

int MIC_record(uint8_t* buffer, int bufferSize, int* bufferWp, int samples);

#endif