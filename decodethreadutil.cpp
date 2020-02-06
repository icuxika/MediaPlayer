#include "decodethreadutil.h"

DecodeThreadUtil::DecodeThreadUtil()
{
    if (!decodeUtil)
    {
        decodeUtil = new DecodeUtil();
    }
}

DecodeThreadUtil::~DecodeThreadUtil()
{
    isExit = true;
    wait();
}

void DecodeThreadUtil::Push(AVPacket* pkt)
{
    if (!pkt)
    {
        return;
    }
    // 阻塞
    while (!isExit)
    {
        lock.lock();
//        qDebug() << QString::fromLocal8Bit("packets.size()的值") << packets.size();
        if (packets.size() < maxSize)
        {
            packets.push_back(pkt);
            lock.unlock();
            break;
        }
        lock.unlock();
        msleep(1);
    }
}

AVPacket* DecodeThreadUtil::PopPacket()
{
    lock.lock();
    if (packets.empty())
    {
        lock.unlock();
        return nullptr;
    }
    AVPacket* pkt = packets.front();
    packets.pop_front();
    lock.unlock();
    return pkt;
}

void DecodeThreadUtil::Clear()
{
    lock.lock();
    decodeUtil->Clear();
    while (!packets.empty())
    {
        AVPacket* pkt = packets.front();
        av_packet_free(&pkt);
        packets.pop_front();
    }
    lock.unlock();
}

void DecodeThreadUtil::Close()
{
    Clear();
    isExit = true;
    wait();
    decodeUtil->Close();
    lock.lock();
    delete decodeUtil;
    decodeUtil = nullptr;
    lock.unlock();
}

