#ifndef AUDIOPLAY_H
#define AUDIOPLAY_H
#include <mutex>
#include <QDebug>

#include <QAudioFormat>
#include <QAudioOutput>

class AudioPlay
{
public:
    AudioPlay();

    virtual ~AudioPlay();

    virtual bool Open(int sampleRate, int sampleSize, int channels) = 0;
    virtual void Clear() = 0;
    virtual void Close() = 0;
    virtual bool Write(const unsigned char* data, int dataSize) = 0;
    virtual int GetFree() = 0;

    virtual long long GetPtsDifference() = 0;

    static AudioPlay* Get();

    virtual void SetPause(bool isPause) = 0;

protected:
    int sampleRate = 0;
    int sampleSize = 0;
    int channels = 0;

};

#endif // AUDIOPLAY_H
