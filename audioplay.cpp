#include "audioplay.h"

class AudioPlayUtil : public AudioPlay
{
public:

    std::mutex lock;
    QAudioOutput* output = nullptr;
    QIODevice* device = nullptr;

    virtual bool Open(int sampleRate, int sampleSize, int channels)
    {
        Close();
        this->sampleRate = sampleRate;
        this->sampleSize = sampleSize;
        this->channels = channels;
        QAudioFormat audioFormat;
        audioFormat.setSampleRate(sampleRate);
        audioFormat.setSampleSize(sampleSize);
        audioFormat.setChannelCount(channels);
        audioFormat.setCodec("audio/pcm");
        audioFormat.setByteOrder(QAudioFormat::LittleEndian);
        audioFormat.setSampleType(QAudioFormat::UnSignedInt);
        lock.lock();
        output = new QAudioOutput(audioFormat);
        device = output->start();
        lock.unlock();
        if (device)
        {
            return true;
        }
        return false;
    }

    virtual void Clear()
    {
        lock.lock();
        if (device)
        {
            device->reset();
        }
        lock.unlock();
    }

    virtual void Close()
    {
        lock.lock();
        if (device)
        {
            device->close();
            device = nullptr;
        }
        if (output)
        {
            output->stop();
            delete output;
            output = nullptr;
        }
        lock.unlock();
    }
    virtual bool Write(const unsigned char* data, int dataSize)
    {
        if (!data || dataSize <= 0)
        {
            return false;
        }
        lock.lock();
        if (!output || !device)
        {
            lock.unlock();
            return false;
        }
        int size = device->write((char*)data, dataSize);
        lock.unlock();
        if (dataSize != size)
        {
            return false;
        }
        return true;
    }
    virtual int GetFree()
    {
        lock.lock();
        if (!output)
        {
            lock.unlock();
            return 0;
        }
        int free = output->bytesFree();
        lock.unlock();
        return free;
    }

    virtual long long GetPtsDifference()
    {
        lock.lock();
        if (!output)
        {
            lock.unlock();
            return 0;
        }
        long long pts_difference = 0;
        // 还未播放的字节数
        double pts_difference_size = output->bufferSize() - output->bytesFree();
        // 一秒音频字节大小
        double sec_pts_difference_size = sampleRate * (sampleSize / 8) * channels;
        if (sec_pts_difference_size <= 0)
        {
            pts_difference = 0;
        }
        else
        {
            pts_difference = (pts_difference_size / sec_pts_difference_size) * 1000;
        }
        lock.unlock();
//        qDebug() << QString::fromLocal8Bit("GetPtsDifference") << pts_difference;
        return pts_difference;
    }

    virtual void SetPause(bool isPause)
    {
        lock.lock();
        if (!output)
        {
            lock.unlock();
            return;
        }
        if (isPause)
        {
            qDebug() << QString::fromLocal8Bit("output开始暂停");
            output->suspend();
            qDebug() << QString::fromLocal8Bit("output成功暂停");
        }
        else
        {
            qDebug() << QString::fromLocal8Bit("output开始恢复");
            output->resume();
            qDebug() << QString::fromLocal8Bit("output成功恢复");
        }
        lock.unlock();
    }
};

AudioPlay::AudioPlay()
{
}

AudioPlay::~AudioPlay()
{
}

AudioPlay* AudioPlay::Get()
{
    static AudioPlayUtil audioPlayUtil;
    return &audioPlayUtil;
}
