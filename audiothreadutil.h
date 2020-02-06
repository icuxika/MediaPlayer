#ifndef AUDIOTHREADUTIL_H
#define AUDIOTHREADUTIL_H
#include <mutex>
#include <QDebug>
#include "decodethreadutil.h"
#include "decodeutil.h"
#include "resampleutil.h"
#include "audioplay.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class AudioThreadUtil : public DecodeThreadUtil
{
public:
    AudioThreadUtil();

    virtual ~AudioThreadUtil() override;

    void run() override;

    virtual bool Open(AVCodecParameters* codec_param);

    virtual void Clear() override;

    virtual void Close() override;

    virtual void SetPause(bool isPause);
    bool isPause = false;
    // 作为音画同步值
    long long standard_pts = 0;
protected:
    // 互斥锁
    std::mutex audioLock;
    AudioPlay* audioPlay = nullptr;
    ResampleUtil* resampleUtil = nullptr;
};

#endif // AUDIOTHREADUTIL_H
