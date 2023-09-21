#ifndef RESAMPLEUTIL_H
#define RESAMPLEUTIL_H

#include <QDebug>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

class ResampleUtil {
public:
  ResampleUtil();

  virtual ~ResampleUtil();

  // 输入输出参数一致，除了采样格式输出为S16
  virtual bool Open(AVCodecParameters *codec_param,
                    bool is_param_clear = false);

  // 返回重采样后大小，不管成功与否都释放空间
  virtual int Resample(AVFrame *frame, unsigned char *data);

  // 关闭
  virtual void Close();

  // 1 为 AV_SAMPLE_FMT_S16
  int outFormat = 1;

protected:
  // 互斥锁
  std::mutex lock;
  // 重采样上下文
  SwrContext *swr_ctx = nullptr;
};

#endif // RESAMPLEUTIL_H
