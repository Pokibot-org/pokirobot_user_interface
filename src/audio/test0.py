import math
from coeff import *


def writeWAV(fpath, audioData, frequency, bytePerSample, channelNumber):
    fSize = 11*4 + len(audioData)
    bytePerSecond = bytePerSample*channelNumber*frequency
    bytePerBloc = channelNumber*bytePerSample
    bitPerSample = bytePerSample*8
    dataSize = len(audioData)

    fdata = []
    fdata += [0x52, 0x49, 0x46, 0x46] # RIFF
    fdata += [(fSize>>0)&0xFF, (fSize>>8)&0xFF, (fSize>>16)&0xFF, (fSize>>24)&0xFF] # Size
    fdata += [0x57, 0x41, 0x56, 0x45] # WAVE

    fdata += [0x66, 0x6D, 0x74, 0x20] # fmt_
    fdata += [0x10, 0x00, 0x00, 0x00] # Block size - 16
    fdata += [0x01, 0x00] # Integer PCM
    fdata += [channelNumber, 0x00] # Channels
    fdata += [(frequency>>0)&0xFF, (frequency>>8)&0xFF, (frequency>>16)&0xFF, (frequency>>24)&0xFF] # Frequency
    fdata += [(bytePerSecond>>0)&0xFF, (bytePerSecond>>8)&0xFF, (bytePerSecond>>16)&0xFF, (bytePerSecond>>24)&0xFF] # Byte per second to read
    fdata += [(bytePerBloc>>0)&0xFF, (bytePerBloc>>8)&0xFF]
    fdata += [(bitPerSample>>0)&0xFF, (bitPerSample>>8)&0xFF]

    fdata += [0x64, 0x61, 0x74, 0x61] # data
    fdata += [(dataSize>>0)&0xFF, (dataSize>>8)&0xFF, (dataSize>>16)&0xFF, (dataSize>>24)&0xFF]

    fdata += audioData

    f = open(fpath, "wb")
    f.write(bytes(fdata))
    f.close()


def getBipData(frequency, bipFreq, bipLenth):
    samples = int(bipLenth*frequency)
    audioData = [int(32767*math.sin((i%(frequency/bipFreq))*2*math.pi/(frequency/bipFreq))) for i in range(samples)]
    audioData2 = []
    for i in audioData:
        j = i
        if j < 0:
            j = 65536+j
        audioData2 += [j&0xFF, (j>>8)&0xFF]
    return audioData2


att = 0.999
clampH = 32767
clampL = -32767



def pdm2pcm(bytesIn):
    bits = []
    for i in range(len(bytesIn)):
        b = bytesIn[i]
        bits+= [1 if (b>>(7-2*i))&1 else -1 for i in range(4)]
    data = []
    for i in range(len(coeff), len(bits), 16):
        if i%(len(bits)//100)==0:
            print(100*i//len(bits), end="\r")
        s = 0
        for j in range(len(coeff)):
            s+=coeff[j]*bits[i-j]
        data+=[s]

    # v = 0
    # data = []
    # for i in range(len(bits)):
    #     v = att*v + bits[i]
    #     # if v > clampH:
    #     #     v = clampH
    #     # elif v < clampL:
    #     #     v = clampL
    #     if i%16 == 0:
    #         w = int(v)
    #         # data+=[w&0xFF, (w>>8)&0xFF]
    #         data+=[v]

    (mini, maxi) = (min(data), max(data))
    etrem = max(-mini, maxi)
    print(mini, maxi, etrem)
    coef = 32767/etrem
    data = [int(i*coef) for i in data]
    print(min(data), max(data))
    data2 = []
    for i in data:
        j = i
        if j < 0:
            j = 65536+j
        data2 += [j&0xFF, (j>>8)&0xFF]
    return data2

frequency = 48000

audioData = []
audioData += getBipData(frequency, 262, 1)
audioData += getBipData(frequency, 293, 1)
audioData += getBipData(frequency, 330, 1)
audioData += getBipData(frequency, 349, 1)
audioData += getBipData(frequency, 392, 1)
audioData += getBipData(frequency, 440, 1)
audioData += getBipData(frequency, 494, 1)
audioData += getBipData(frequency, 523, 1)

writeWAV("test.wav", audioData, frequency, 2, 1)

# f = open("./../../sd/MIC3.BIN", "rb")
# data = [i for i in f.read()]
# f.close()
# data = pdm2pcm(data)

# writeWAV("test.wav", data, frequency, 2, 1)