#include "demuxutil.h"

static double fraction2double(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / r.den;
}

DemuxUtil::DemuxUtil()
{
    static bool isFirst = true;
    lock.lock();
    if (isFirst)
    {
        // 初始化网络库
        avformat_network_init();
        isFirst = false;
    }
    lock.unlock();
}

DemuxUtil::~DemuxUtil() {}

// 打开本地媒体文件或者流媒体（rtmp HTTP rstp）
bool DemuxUtil::Open(const char* url)
{
    Close();
    int result;
    // 参数设置
    AVDictionary* options = nullptr;
    // 设置rtsp流以tcp协议打开
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    // 设置网络延时
    av_dict_set(&options, "max_delay", "500", 0);
    lock.lock();
    result = avformat_open_input(&fmt_ctx, url, nullptr, &options);
    if (result != 0)
    {
        lock.unlock();
        char errbuf[64] = {0};
        qDebug() << "Can't open the file!" << av_make_error_string(errbuf, 64, result);
        return false;
    }
    // 获取流信息
    result = avformat_find_stream_info(fmt_ctx, nullptr);
    // 获取总时长（毫秒）
    totalMilliseconds = fmt_ctx->duration / (AV_TIME_BASE / 1000);
    // 获取总时长（秒）
    int totalSeconds = fmt_ctx->duration / AV_TIME_BASE;
    qDebug() << QString::fromLocal8Bit("获取媒体总时长（秒）") << totalSeconds;
    // 打印视频流信息
    av_dump_format(fmt_ctx, 0, url, 0);
    // 获取音视频流信息
    videoStream = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    AVStream* vStream = fmt_ctx->streams[videoStream];
    qDebug() << QString::fromLocal8Bit("------------------------------------------------------------");
    qDebug() << QString::fromLocal8Bit("视频信息（videoStream）") << videoStream;
    qDebug() << QString::fromLocal8Bit("视频宽度") << vStream->codecpar->width;
    qDebug() << QString::fromLocal8Bit("视频高度") << vStream->codecpar->height;
    qDebug() << QString::fromLocal8Bit("视频帧率FPS") << fraction2double(vStream->avg_frame_rate);
    qDebug() << QString::fromLocal8Bit("视频像素格式") << vStream->codecpar->format;
    qDebug() << QString::fromLocal8Bit("------------------------------------------------------------");
    audioStream = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    AVStream* aStream = fmt_ctx->streams[audioStream];
    qDebug() << QString::fromLocal8Bit("音频信息（audioStream）") << audioStream;
    qDebug() << QString::fromLocal8Bit("样本率") << aStream->codecpar->sample_rate;
    qDebug() << QString::fromLocal8Bit("音频采样格式") << aStream->codecpar->format;
    qDebug() << QString::fromLocal8Bit("声道数") << aStream->codecpar->channels;
    qDebug() << QString::fromLocal8Bit("编码格式") << aStream->codecpar->codec_id;
    qDebug() << QString::fromLocal8Bit("音频FPS") << fraction2double(aStream->avg_frame_rate);
    qDebug() << QString::fromLocal8Bit("一帧音频的大小") << aStream->codecpar->frame_size;
    qDebug() << QString::fromLocal8Bit("------------------------------------------------------------");
    lock.unlock();
    return true;
}

// 调用者使用av_packet_free()释放对象空间和数据空间
AVPacket* DemuxUtil::Read()
{
    lock.lock();
    if(!fmt_ctx)
    {
        lock.unlock();
        return nullptr;
    }
    AVPacket* pkt = av_packet_alloc();
    // 读取一帧并分配空间
    int result = av_read_frame(fmt_ctx, pkt);
    if (result != 0)
    {
        lock.unlock();
        av_packet_free(&pkt);
        char errbuf[64] = {0};
        qDebug() << QString::fromLocal8Bit("读取AVPacket失败") << av_make_error_string(errbuf, 64, result);
        return nullptr;
    }
    // 转换为毫秒
    pkt->pts = pkt->pts * (1000 * (fraction2double(fmt_ctx->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts * (1000 * (fraction2double(fmt_ctx->streams[pkt->stream_index]->time_base)));
//    qDebug() << QString::fromLocal8Bit("当前stream_index") << pkt->stream_index << QString::fromLocal8Bit("pts") << pkt->pts;
    lock.unlock();
    return pkt;
}

// 只读视频，释放音频
AVPacket* DemuxUtil::ReadOnlyVideo()
{
    lock.lock();
    if (!fmt_ctx)
    {
        lock.unlock();
        return nullptr;
    }
    lock.unlock();
    AVPacket* pkt = nullptr;
    for (int i = 0; i < 40; i++)
    {
        pkt = Read();
        if (!pkt)
        {
            break;
        }
        if (pkt->stream_index == videoStream)
        {
            break;
        }
        av_packet_free(&pkt);
    }
    return pkt;
}

// 判断是否是音频
bool DemuxUtil::IsAudio(AVPacket* pkt)
{
    if (!pkt)
    {
        return false;
    }
    if (pkt->stream_index == videoStream)
    {
        return false;
    }
    return true;
}

// 获取视频参数，使用avcodec_parameters_free()清理空间
AVCodecParameters* DemuxUtil::getVideoParameters()
{
    lock.lock();
    if (!fmt_ctx)
    {
        lock.unlock();
        return nullptr;
    }
    AVCodecParameters* videoParameters = avcodec_parameters_alloc();
    avcodec_parameters_copy(videoParameters, fmt_ctx->streams[videoStream]->codecpar);
    lock.unlock();
    return videoParameters;
}

// 获取音频参数，使用avcodec_parameters_free()清理空间
AVCodecParameters* DemuxUtil::getAudioParameters()
{
    lock.lock();
    if (!fmt_ctx)
    {
        lock.unlock();
        return nullptr;
    }
    AVCodecParameters* audioParameters = avcodec_parameters_alloc();
    avcodec_parameters_copy(audioParameters, fmt_ctx->streams[audioStream]->codecpar);
    lock.unlock();
    return audioParameters;
}

// seek 位置 pos 0.0
bool DemuxUtil::Seek(double pos)
{
    lock.lock();
    if (!fmt_ctx)
    {
        lock.unlock();
        return false;
    }
    avformat_flush(fmt_ctx);
    long long seekPos = 0;
    seekPos = fmt_ctx->streams[videoStream]->duration * pos;
    int result = av_seek_frame(fmt_ctx, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    lock.unlock();
    if (result < 0)
    {
        char errbuf[64] = {0};
        qDebug() << QString::fromLocal8Bit("Seek失败") << av_make_error_string(errbuf, 64, result);
        return false;
    }
    return true;
}

// 清空读取缓存
void DemuxUtil::Clear()
{
    lock.lock();
    if (!fmt_ctx)
    {
        lock.unlock();
        return;
    }
    // 清理读取缓存
    avformat_flush(fmt_ctx);
    lock.unlock();
}

// 关闭
void DemuxUtil::Close()
{
    lock.lock();
    if (!fmt_ctx)
    {
        lock.unlock();
        return;
    }
    avformat_close_input(&fmt_ctx);
    totalMilliseconds = 0;
    lock.unlock();
}
