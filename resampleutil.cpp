#include "resampleutil.h"

ResampleUtil::ResampleUtil() {}

ResampleUtil::~ResampleUtil() {}

// 输入输出参数一致，除了采样格式输出为S16
bool ResampleUtil::Open(AVCodecParameters *codec_param, bool is_param_clear) {
  Close();
  if (!codec_param) {
    qDebug() << "codec_param不能为空";
    return false;
  }
  lock.lock();
  // 音频重采样，上下文初始化
  swr_ctx = swr_alloc();
  swr_ctx = swr_alloc_set_opts(
      swr_ctx,
      av_get_default_channel_layout(2), // 输出格式
      (AVSampleFormat)outFormat,        // 输出样本格式
      codec_param->sample_rate,         // 输出采样率
      av_get_default_channel_layout(codec_param->channels), // 输入格式
      (AVSampleFormat)codec_param->format, codec_param->sample_rate, 0,
      nullptr);
  if (is_param_clear) {
    avcodec_parameters_free(&codec_param);
  }
  int result = swr_init(swr_ctx);
  lock.unlock();
  if (result != 0) {
    char errbuf[64] = {0};
    qDebug() << "重采样初始化失败" << av_make_error_string(errbuf, 64, result);
    return false;
  }
  return true;
}

// 返回重采样后大小，不管成功与否都释放空间
int ResampleUtil::Resample(AVFrame *frame, unsigned char *data) {
  if (!frame) {
    return 0;
  }
  if (!data) {
    av_frame_free(&frame);
    return 0;
  }
  uint8_t *out[2] = {nullptr};
  out[0] = data;
  int result = swr_convert(swr_ctx,
                           out,                           // 输出
                           frame->nb_samples,             // 输出
                           (const uint8_t **)frame->data, // 输入
                           frame->nb_samples              // 输入
  );
  if (result < 0) {
    av_frame_free(&frame);
    char errbuf[64] = {0};
    qDebug() << "重采样失败" << av_make_error_string(errbuf, 64, result);
    return result;
  }
  int resample_size = result * frame->channels *
                      av_get_bytes_per_sample((AVSampleFormat)outFormat);
  av_frame_free(&frame);
  return resample_size;
}

// 关闭
void ResampleUtil::Close() {
  lock.lock();
  if (swr_ctx) {
    swr_free(&swr_ctx);
  }
  lock.unlock();
}
