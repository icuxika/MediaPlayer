#ifndef DEMUXTHREADUTIL_H
#define DEMUXTHREADUTIL_H
#include <mutex>
#include <QDebug>
#include <QThread>
#include "demuxutil.h"
#include "videothreadutil.h"
#include "audiothreadutil.h"
#include "videoplaywidget.h"

class DemuxThreadUtil : public QThread
{
public:
    DemuxThreadUtil();

    virtual ~DemuxThreadUtil() override;

    virtual bool Open(const char* url, VideoPlayWidget* widget);

    virtual void  StartThreads();

    virtual void run() override;

    virtual void Clear();

    virtual void Close();

    virtual void Seek(double pos);

    virtual void SetPause(bool isPause);
    bool isPause = false;

    bool isExit = false;

    long long pts = 0;

    long long totalMilliseconds = 0;
protected:
    // 互斥锁
    std::mutex lock;

    DemuxUtil* demuxUtil = nullptr;
    VideoThreadUtil* videoThreadUtil = nullptr;
    AudioThreadUtil* audioThreadUtil = nullptr;
};

#endif // DEMUXTHREADUTIL_H
