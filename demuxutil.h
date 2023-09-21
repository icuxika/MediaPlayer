#ifndef DEMUXUTIL_H
#define DEMUXUTIL_H

#include <QDebug>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class DemuxUtil {
public:
  DemuxUtil();

  virtual ~DemuxUtil();

  // 打开本地媒体文件或者流媒体（rtmp HTTP rstp）
  virtual bool Open(const char *url);

  // 调用者使用av_packet_free()释放对象空间和数据空间
  virtual AVPacket *Read();

  // 只读视频，释放音频
  virtual AVPacket *ReadOnlyVideo();

  // 判断是否是音频
  virtual bool IsAudio(AVPacket *pkt);

  // 获取视频参数，使用avcodec_parameters_free()清理空间
  virtual AVCodecParameters *getVideoParameters();

  // 获取音频参数，使用avcodec_parameters_free()清理空间
  virtual AVCodecParameters *getAudioParameters();

  // seek 位置 pos 0.0
  virtual bool Seek(double pos);

  // 清空读取缓存
  virtual void Clear();

  // 关闭
  virtual void Close();

  // 媒体总时长（毫秒）
  int totalMilliseconds = 0;

protected:
  // 互斥锁
  std::mutex lock;
  // 解封装上下文
  AVFormatContext *fmt_ctx = nullptr;

  // 音视频索引，读取时区分音视频
  int videoStream = 0;
  int audioStream = 0;
};

#endif // DEMUXUTIL_H
