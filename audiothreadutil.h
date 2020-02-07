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

    // 音频播放
    void run() override;

    // 初始化
    virtual bool Open(AVCodecParameters* codec_param);

    // 清理
    virtual void Clear() override;

    // 关闭
    virtual void Close() override;

    // 设置暂停
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
