#include "audiothreadutil.h"

AudioThreadUtil::AudioThreadUtil()
{
    if (!resampleUtil)
    {
        resampleUtil = new ResampleUtil();
    }
    if (!audioPlay)
    {
        audioPlay = AudioPlay::Get();
    }
}

AudioThreadUtil::~AudioThreadUtil()
{
    isExit = true;
    wait();
}

void AudioThreadUtil::run()
{
    unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        audioLock.lock();
        if (isPause)
        {
            audioLock.unlock();
            msleep(5);
            continue;
        }
        AVPacket* pkt = PopPacket();
        bool result = decodeUtil->Send(pkt);
        if (!result)
        {
            audioLock.unlock();
            msleep(1);
            continue;
        }
        // 有可能一次Send多次Receive
        while (!isExit)
        {
            AVFrame* frame = decodeUtil->Receive();
            if (!frame)
            {
                break;
            }
            // 减去缓冲中未播放的时间
            standard_pts = decodeUtil->standard_pts - audioPlay->GetPtsDifference();
            // 重采样
            int resample_size = resampleUtil->Resample(frame, pcm);
            // 播放音频
            while (!isExit)
            {
                if (resample_size <= 0)
                {
                    break;
                }
                // 缓冲未播完，空间不够
                if (audioPlay->GetFree() < resample_size || isPause)
                {
                    msleep(1);
                    continue;
                }
                audioPlay->Write(pcm, resample_size);
                break;
            }
        }
        audioLock.unlock();
    }
    delete [] pcm;
}

bool AudioThreadUtil::Open(AVCodecParameters* codec_param)
{
    if(!codec_param)
    {
        return false;
    }
    Clear();
    audioLock.lock();
    standard_pts = 0;
    bool result = true;
    if (!resampleUtil->Open(codec_param, false))
    {
        qDebug() << QString::fromLocal8Bit("重采样打开失败");
        result = false;
    }
    if (!audioPlay->Open(codec_param->sample_rate, 16, codec_param->channels))
    {
        qDebug() << QString::fromLocal8Bit("音频播放打开失败");
        result = false;
    }
    if (!decodeUtil->Open(codec_param))
    {
        qDebug() << QString::fromLocal8Bit("解码打开失败");
        result = false;
    }
    audioLock.unlock();
    return result;
}

void AudioThreadUtil::Clear()
{
    DecodeThreadUtil::Clear();
    // 此处加速 暂停时无法快进
//    audioLock.lock();
    if (audioPlay)
    {
        audioPlay->Clear();
    }
//    audioLock.unlock();
}

void AudioThreadUtil::Close()
{
    DecodeThreadUtil::Close();
    if (resampleUtil)
    {
        resampleUtil->Close();
        audioLock.lock();
        delete resampleUtil;
        resampleUtil = nullptr;
        audioLock.unlock();
    }
    if (audioPlay)
    {
        audioLock.lock();
        audioPlay->Close();
        audioPlay = nullptr;
        audioLock.unlock();
    }
}

void AudioThreadUtil::SetPause(bool isPause)
{
    qDebug() << QString::fromLocal8Bit("AudioThreadUtil::SetPause") << isPause;
    this->isPause = isPause;
    if (audioPlay)
    {
        audioPlay->SetPause(isPause);
    }
}

