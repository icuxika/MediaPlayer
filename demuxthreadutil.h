#ifndef DEMUXTHREADUTIL_H
#define DEMUXTHREADUTIL_H
#include "audiothreadutil.h"
#include "demuxutil.h"
#include "videoplaywidget.h"
#include "videothreadutil.h"
#include <QDebug>
#include <QThread>
#include <mutex>

class DemuxThreadUtil : public QThread {
public:
  DemuxThreadUtil();

  virtual ~DemuxThreadUtil() override;

  // 打开媒体文件，初始化视频显示组件
  virtual bool Open(const char *url, VideoPlayWidget *widget);

  // 启动音视频线程
  virtual void StartThreads();

  // 负责推送AVPacket* pkt
  virtual void run() override;

  // 清理
  virtual void Clear();

  // 关闭
  virtual void Close();

  // 快进
  virtual void Seek(double pos);

  // 设置暂停
  virtual void SetPause(bool isPause);

  bool isPause = false;

  bool isExit = false;

  // 用来判断播放进度
  long long pts = 0;

  // 播放总时长（毫秒）
  long long totalMilliseconds = 0;

protected:
  // 互斥锁
  std::mutex lock;

  DemuxUtil *demuxUtil = nullptr;
  VideoThreadUtil *videoThreadUtil = nullptr;
  AudioThreadUtil *audioThreadUtil = nullptr;
};

#endif // DEMUXTHREADUTIL_H
