#ifndef VIDEOTHREADUTIL_H
#define VIDEOTHREADUTIL_H
#include <mutex>
#include <QDebug>
#include "decodethreadutil.h"
#include "decodeutil.h"
#include "videoplaywidget.h"

class VideoThreadUtil : public DecodeThreadUtil
{
public:
    VideoThreadUtil();

    virtual ~VideoThreadUtil();

    void run() override;

    virtual bool Open(AVCodecParameters* codec_param, VideoPlayWidget* widget, int width, int height);

    virtual bool RepaintSeekPts(AVPacket* pkt, long long seekPts);

    virtual void SetPause(bool isPause);
    bool isPause = false;

    long long sync_pts = 0;
protected:
    // 互斥锁
    std::mutex videoLock;

    VideoPlayWidget* widget = nullptr;
};

#endif // VIDEOTHREADUTIL_H
