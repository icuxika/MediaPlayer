#ifndef DECODEUTIL_H
#define DECODEUTIL_H

#include <mutex>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class DecodeUtil
{
public:
    DecodeUtil();

    virtual ~DecodeUtil();

    // 打开解码器，不管成功与否都释放空间
    virtual bool Open(AVCodecParameters* codec_param);

    // 发送到解码线程，不管成功与否都释放空间
    virtual bool Send(AVPacket* pkt);

    // 获取解码后数据，一次send可能需要多次receive，获取缓冲中的数据Send NULL，再Receive多次
    // 每次复制一份，由调用者释放空间 av_frame_free()
    virtual AVFrame* Receive();

    // 清空缓存
    virtual void Clear();

    // 关闭
    virtual void Close();

    // 作为音画同步值
    long long standard_pts = 0;

protected:
    // 互斥锁
    std::mutex lock;

    // 解码上下文
    AVCodecContext* codec_ctx = nullptr;
};

#endif // DECODEUTIL_H
