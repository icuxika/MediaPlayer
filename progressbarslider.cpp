#include "progressbarslider.h"

ProgressBarSlider::ProgressBarSlider(QWidget *parent) : QSlider(parent) {}

ProgressBarSlider::~ProgressBarSlider() {}

// 进度条实现点击快进
void ProgressBarSlider::mousePressEvent(QMouseEvent *event) {
  double pos = (double)event->pos().x() / (double)width();
  setValue(pos * this->maximum());
  //    QSlider::mousePressEvent(event);
  QSlider::sliderReleased();
}
