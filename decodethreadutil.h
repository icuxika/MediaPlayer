#ifndef DECODETHREADUTIL_H
#define DECODETHREADUTIL_H

#include <QDebug>
#include <mutex>
#include <QThread>
#include <list>

#include "decodeutil.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class DecodeThreadUtil : public QThread
{
public:
    DecodeThreadUtil();

    virtual ~DecodeThreadUtil();

    virtual void Push(AVPacket* pkt);

    virtual AVPacket* PopPacket();

    virtual void Clear();

    virtual void Close();

    int maxSize = 100;
    bool isExit = false;

protected:
    // 互斥锁
    std::mutex lock;

    std::list <AVPacket*> packets;

    DecodeUtil* decodeUtil = nullptr;
};

#endif // DECODETHREADUTIL_H
