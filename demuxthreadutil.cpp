#include "demuxthreadutil.h"

DemuxThreadUtil::DemuxThreadUtil()
{
}

DemuxThreadUtil::~DemuxThreadUtil()
{
    isExit = true;
    wait();
}

// 打开媒体文件，初始化视频显示组件
bool DemuxThreadUtil::Open(const char* url, VideoPlayWidget* widget)
{
    if (url == nullptr || url[0] == '\0')
    {
        return false;
    }
    lock.lock();
    if (!demuxUtil)
    {
        demuxUtil = new DemuxUtil();
    }
    if (!videoThreadUtil)
    {
        videoThreadUtil = new VideoThreadUtil();
    }
    if (!audioThreadUtil)
    {
        audioThreadUtil = new AudioThreadUtil();
    }
    bool result = demuxUtil->Open(url);
    if (!result)
    {
        lock.unlock();
        qDebug() << QString::fromLocal8Bit("解封装Open失败");
        return false;
    }
    if (!videoThreadUtil->Open(demuxUtil->getVideoParameters(), widget, demuxUtil->getVideoParameters()->width, demuxUtil->getVideoParameters()->height))
    {
        result = false;
        qDebug() << QString::fromLocal8Bit("打开视频解码器和处理线程失败");
    }
    if (!audioThreadUtil->Open(demuxUtil->getAudioParameters()))
    {
        result = false;
        qDebug() << QString::fromLocal8Bit("打开音频解码器和处理线程失败");
    }
    lock.unlock();
    // 用来计算播放进度
    totalMilliseconds = demuxUtil->totalMilliseconds;
    return result;
}

// 启动音视频线程
void DemuxThreadUtil::StartThreads()
{
    lock.lock();
    if (!demuxUtil)
    {
        demuxUtil = new DemuxUtil();
    }
    if (!videoThreadUtil)
    {
        videoThreadUtil = new VideoThreadUtil();
    }
    if (!audioThreadUtil)
    {
        audioThreadUtil = new AudioThreadUtil();
    }
    QThread::start();
    if (videoThreadUtil)
    {
        videoThreadUtil->start();
    }
    if (audioThreadUtil)
    {
        audioThreadUtil->start();
    }
    lock.unlock();
}

// 负责推送AVPacket* pkt
void DemuxThreadUtil::run()
{
    while (!isExit)
    {
        lock.lock();
        if(isPause)
        {
            lock.unlock();
            msleep(5);
            continue;
        }
        if (!demuxUtil)
        {
            lock.unlock();
            msleep(5);
            continue;
        }
        if (videoThreadUtil && audioThreadUtil)
        {
            // 用来计算播放进度
            pts = audioThreadUtil->standard_pts;
            // 用来音画同步
            videoThreadUtil->sync_pts = audioThreadUtil->standard_pts;
        }
        AVPacket* pkt = demuxUtil->Read();
        if (!pkt)
        {
            lock.unlock();
            msleep(5);
            continue;
        }
        if (demuxUtil->IsAudio(pkt))
        {
            if (audioThreadUtil)
            {
                audioThreadUtil->Push(pkt);
            }
        }
        else
        {
            if (videoThreadUtil)
            {
                videoThreadUtil->Push(pkt);
            }
        }
        lock.unlock();
        msleep(1);
    }
}

// 清理
void DemuxThreadUtil::Clear()
{
    lock.lock();
    if (demuxUtil)
    {
        demuxUtil->Clear();
        qDebug() << QString::fromLocal8Bit("demuxUtil清理结束");
    }
    if (videoThreadUtil)
    {
        videoThreadUtil->Clear();
        qDebug() << QString::fromLocal8Bit("videoThreadUtil清理结束");
    }
    if (audioThreadUtil)
    {
        audioThreadUtil->Clear();
        qDebug() << QString::fromLocal8Bit("audioThreadUtil清理结束");
    }
    lock.unlock();
}

// 关闭
void DemuxThreadUtil::Close()
{
    isExit = true;
    wait();
    if (videoThreadUtil)
    {
        videoThreadUtil->Close();
    }
    if (audioThreadUtil)
    {
        audioThreadUtil->Close();
    }
    lock.lock();
    delete videoThreadUtil;
    delete audioThreadUtil;
    videoThreadUtil = nullptr;
    audioThreadUtil = nullptr;
    lock.unlock();
}

// 快进
void DemuxThreadUtil::Seek(double pos)
{
    qDebug() << QString::fromLocal8Bit("进入快进");
    Clear();
    qDebug() << QString::fromLocal8Bit("设置暂停");
    lock.lock();
    bool pauseStatus = this->isPause;
    qDebug() << QString::fromLocal8Bit("pauseStatus") << pauseStatus;
    lock.unlock();
    // 设置暂停
    SetPause(true);
    qDebug() << QString::fromLocal8Bit("设置暂停成功");
    lock.lock();
    if (demuxUtil)
    {
        qDebug() << QString::fromLocal8Bit("开始跳转");
        demuxUtil->Seek(pos);
    }
    long long seekPts = pos * demuxUtil->totalMilliseconds;
    qDebug() << QString::fromLocal8Bit("进度条目标位置pts") << seekPts;
    while (!isExit)
    {
        AVPacket* pkt = demuxUtil->ReadOnlyVideo();
        if (!pkt)
        {
            break;
        }
        if (videoThreadUtil->RepaintSeekPts(pkt, seekPts))
        {
            this->pts = seekPts;
            break;
        }
    }
    qDebug() << QString::fromLocal8Bit("进度条跳转完成");
    lock.unlock();
    if (!pauseStatus)
    {
        SetPause(false);
    }
}

// 设置暂停
void DemuxThreadUtil::SetPause(bool isPause)
{
    lock.lock();
    this->isPause = isPause;
    if (videoThreadUtil)
    {
        videoThreadUtil->SetPause(isPause);
    }
    if (audioThreadUtil)
    {
        audioThreadUtil->SetPause(isPause);
    }
    lock.unlock();
}
