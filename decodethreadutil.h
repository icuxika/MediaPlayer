#ifndef DECODETHREADUTIL_H
#define DECODETHREADUTIL_H

#include <QDebug>
#include <QThread>
#include <list>
#include <mutex>

#include "decodeutil.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class DecodeThreadUtil : public QThread {
public:
  DecodeThreadUtil();

  virtual ~DecodeThreadUtil();

  // 推送AVPacket* pkt到队列
  virtual void Push(AVPacket *pkt);

  // 从队列中取出AVPacket* pkt
  virtual AVPacket *PopPacket();

  // 清理
  virtual void Clear();

  // 关闭
  virtual void Close();

  // AVPacket* pkt队列最大容量
  int maxSize = 100;
  bool isExit = false;

protected:
  // 互斥锁
  std::mutex lock;

  std::list<AVPacket *> packets;

  DecodeUtil *decodeUtil = nullptr;
};

#endif // DECODETHREADUTIL_H
