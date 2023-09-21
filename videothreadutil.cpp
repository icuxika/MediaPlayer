#include "videothreadutil.h"

VideoThreadUtil::VideoThreadUtil() {}

VideoThreadUtil::~VideoThreadUtil() {}

// 视频播放
void VideoThreadUtil::run() {
  while (!isExit) {
    videoLock.lock();
    if (isPause) {
      videoLock.unlock();
      msleep(5);
      continue;
    }
    qDebug() << "Video sync_pts值" << sync_pts << "decodeUtil standard_pts值"
             << decodeUtil->standard_pts;
    // 音画同步
    if (sync_pts > 0 && sync_pts < decodeUtil->standard_pts) {
      videoLock.unlock();
      msleep(1);
      continue;
    }
    AVPacket *pkt = PopPacket();
    bool result = decodeUtil->Send(pkt);
    if (!result) {
      videoLock.unlock();
      msleep(1);
      continue;
    }
    // 有可能一次Send对应多次Receive
    while (!isExit) {
      AVFrame *frame = decodeUtil->Receive();
      if (!frame) {
        break;
      }
      // 显示视频
      if (widget) {
        widget->Repaint(frame);
      }
    }
    videoLock.unlock();
  }
}

// 初始化
bool VideoThreadUtil::Open(AVCodecParameters *codec_param,
                           VideoPlayWidget *widget, int width, int height) {
  if (!codec_param) {
    return false;
  }
  Clear();
  videoLock.lock();
  sync_pts = 0;
  // 初始化显示窗口
  this->widget = widget;
  if (widget) {
    widget->Init(width, height);
  }
  videoLock.unlock();
  bool result = true;
  if (!decodeUtil->Open(codec_param)) {
    result = false;
  }
  return result;
}

// 快进后刷新显示画面
bool VideoThreadUtil::RepaintSeekPts(AVPacket *pkt, long long seekPts) {
  videoLock.lock();
  bool result = decodeUtil->Send(pkt);
  if (!result) {
    videoLock.unlock();
    return true;
  }
  AVFrame *frame = decodeUtil->Receive();
  if (!frame) {
    videoLock.unlock();
    return false;
  }
  if (decodeUtil->standard_pts >= seekPts) {
    qDebug() << "到达位置";
    if (widget) {
      widget->Repaint(frame);
    }
    qDebug() << "到达位置绘画完成";
    videoLock.unlock();
    qDebug() << "解锁完成";
    return true;
  }
  av_frame_free(&frame);
  videoLock.unlock();
  return false;
}

// 设置暂停
void VideoThreadUtil::SetPause(bool isPause) {
  qDebug() << "VideoThreadUtil::SetPause" << isPause;
  videoLock.lock();
  this->isPause = isPause;
  videoLock.unlock();
}
