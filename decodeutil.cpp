#include "decodeutil.h"

DecodeUtil::DecodeUtil()
{
}

DecodeUtil::~DecodeUtil() {}

// 打开解码器，不管成功与否都释放空间
bool DecodeUtil::Open(AVCodecParameters* codec_param)
{
    if (!codec_param)
    {
        return false;
    }
    Close();
    // 找到解码器
    AVCodec* codec = avcodec_find_decoder(codec_param->codec_id);
    if (!codec)
    {
        avcodec_parameters_free(&codec_param);
        return false;
    }
    lock.lock();
    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_param);
    codec_ctx->thread_count = 8;
    int result = avcodec_open2(codec_ctx, nullptr, nullptr);
    if (result != 0)
    {
        avcodec_free_context(&codec_ctx);
        lock.unlock();
        char errbuf[64] = {0};
        qDebug() << QString::fromLocal8Bit("打开解码器失败") << av_make_error_string(errbuf, 64, result);
        return false;
    }
    lock.unlock();
    qDebug() << QString::fromLocal8Bit("查找到的解码器名称") << codec->name;
//    avcodec_parameters_free(&codec_param);
    return true;
}

// 发送到解码线程，不管成功与否都释放空间
bool DecodeUtil::Send(AVPacket* pkt)
{
    // 容错处理
    if (!pkt || pkt->size <= 0 || !pkt->data)
    {
        return false;
    }
    lock.lock();
    if (!codec_ctx)
    {
        lock.unlock();
        return false;
    }
    int result = avcodec_send_packet(codec_ctx, pkt);
    lock.unlock();
    av_packet_free(&pkt);
    if (result != 0)
    {
        char errbuf[64] = {0};
        qDebug() << QString::fromLocal8Bit("发送到解码线程失败") << av_make_error_string(errbuf, 64, result);
        return false;
    }
    return true;
}

// 获取解码后数据，一次send可能需要多次receive，获取缓冲中的数据Send NULL，再Receive多次
// 每次复制一份，由调用者释放空间 av_frame_free()
AVFrame* DecodeUtil::Receive()
{
    lock.lock();
    if (!codec_ctx)
    {
        lock.unlock();
        return nullptr;
    }
    AVFrame* frame = av_frame_alloc();
    int result = avcodec_receive_frame(codec_ctx, frame);
    lock.unlock();
    if (result != 0)
    {
        av_frame_free(&frame);
//        char errbuf[64] = {0};
//        qDebug() << QString::fromLocal8Bit("获取frame失败") << av_make_error_string(errbuf, 64, result);
        return nullptr;
    }
//    qDebug() << QString::fromLocal8Bit("每行数据大小") << frame->linesize[0];
    standard_pts = frame->pts;
//    qDebug() << QString::fromLocal8Bit("DecodeUtil::Receive standard_pts") <<  standard_pts;
    return frame;
}

// 清空缓存
void DecodeUtil::Clear()
{
    lock.lock();
    // 清理解码缓存
    if (codec_ctx)
    {
        avcodec_flush_buffers(codec_ctx);
    }
    lock.unlock();
}

// 关闭
void DecodeUtil::Close()
{
    lock.lock();
    if (codec_ctx)
    {
        avcodec_close(codec_ctx);
        avcodec_free_context(&codec_ctx);
    }
    standard_pts = 0;
    lock.unlock();
}
