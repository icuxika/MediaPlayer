#ifndef PROGRESSBARSLIDER_H
#define PROGRESSBARSLIDER_H
#include <QMouseEvent>
#include <QSlider>

class ProgressBarSlider : public QSlider {
  Q_OBJECT

public:
  ProgressBarSlider(QWidget *parnet = nullptr);

  ~ProgressBarSlider();

  // 进度条实现点击快进
  void mousePressEvent(QMouseEvent *event) override;
};

#endif // PROGRESSBARSLIDER_H
