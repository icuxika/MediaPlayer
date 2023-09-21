#ifndef AUDIOPLAY_H
#define AUDIOPLAY_H
#include <QDebug>
#include <mutex>

#include <QAudioFormat>
#include <QAudioOutput>

class AudioPlay {
public:
  AudioPlay();

  virtual ~AudioPlay();

  // 初始化音频播放
  virtual bool Open(int sampleRate, int sampleSize, int channels) = 0;
  // 清理
  virtual void Clear() = 0;
  // 关闭
  virtual void Close() = 0;
  // 播放
  virtual bool Write(const unsigned char *data, int dataSize) = 0;
  // 计算空闲空间
  virtual int GetFree() = 0;
  // 计算音视频播放进度PTS差值
  virtual long long GetPtsDifference() = 0;
  // 获取音频播放实例
  static AudioPlay *Get();
  // 设置暂停
  virtual void SetPause(bool isPause) = 0;

protected:
  int sampleRate = 0;
  int sampleSize = 0;
  int channels = 0;
};

#endif // AUDIOPLAY_H
