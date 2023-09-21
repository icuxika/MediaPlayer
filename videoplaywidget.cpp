#include "videoplaywidget.h"

#define GET_STR(x) #x
// 顶点shader
const char *vertexStr =
    GET_STR(attribute vec4 vertexIn; attribute vec2 textureIn;
            varying vec2 textureOut; void main(void) {
              gl_Position = vertexIn;
              textureOut = textureIn;
            });

// 片元shader
const char *fragmentStr =
    GET_STR(varying vec2 textureOut; uniform sampler2D tex_y;
            uniform sampler2D tex_u; uniform sampler2D tex_v; void main(void) {
              vec3 yuv;
              vec3 rgb;
              yuv.x = texture2D(tex_y, textureOut).r;
              yuv.y = texture2D(tex_u, textureOut).r - 0.5;
              yuv.z = texture2D(tex_v, textureOut).r - 0.5;
              rgb = mat3(1.0, 1.0, 1.0, 0.0, -0.39465, 2.03211, 1.13983,
                         -0.58060, 0.0) *
                    yuv;
              gl_FragColor = vec4(rgb, 1.0);
            });

VideoPlayWidget::VideoPlayWidget(QWidget *parent) : QOpenGLWidget(parent) {}

VideoPlayWidget::~VideoPlayWidget() {}

void VideoPlayWidget::Init(int width, int height) {
  lock.lock();
  this->width = width;
  this->height = height;
  delete pixels[0];
  delete pixels[1];
  delete pixels[2];
  // 分配材质内存空间
  pixels[0] = new unsigned char[width * height];
  pixels[1] = new unsigned char[width * height / 4];
  pixels[2] = new unsigned char[width * height / 4];
  if (textures[0]) {
    glDeleteTextures(3, textures);
  }
  // 创建材质
  glGenTextures(3, textures);
  // Y
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  // 放大过滤，线性插值
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // 创建材质显卡空间
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, nullptr);
  // U
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  // 放大过滤，线性插值
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // 创建材质显卡空间
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED,
               GL_UNSIGNED_BYTE, nullptr);
  // V
  glBindTexture(GL_TEXTURE_2D, textures[2]);
  // 放大过滤，线性插值
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // 创建材质显卡空间
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED,
               GL_UNSIGNED_BYTE, nullptr);
  lock.unlock();
}

void VideoPlayWidget::Repaint(AVFrame *frame) {
  if (!frame) {
    return;
  }
  lock.lock();
  if (!pixels[0] || this->width * this->height == 0 ||
      frame->width != this->width || frame->height != this->height) {
    av_frame_free(&frame);
    lock.unlock();
    return;
  }
  if (width == frame->linesize[0]) {
    memcpy(pixels[0], frame->data[0], width * height);
    memcpy(pixels[1], frame->data[1], width * height / 4);
    memcpy(pixels[2], frame->data[2], width * height / 4);
  } else {
    // 行对齐问题
    for (int i = 0; i < height; i++) {
      memcpy(pixels[0] + width * i, frame->data[0] + frame->linesize[0] * i,
             width);
    }
    for (int i = 0; i < height / 2; i++) {
      memcpy(pixels[1] + width / 2 * i, frame->data[1] + frame->linesize[1] * i,
             width / 2);
    }
    for (int i = 0; i < height / 2; i++) {
      memcpy(pixels[2] + width / 2 * i, frame->data[2] + frame->linesize[2] * i,
             width / 2);
    }
  }
  lock.unlock();
  av_frame_free(&frame);
  update();
}

void VideoPlayWidget::initializeGL() {
  qDebug() << "initializeGL执行";
  lock.lock();
  // 初始化OpenGL
  initializeOpenGLFunctions();
  // 加载shader脚本（顶点和片元）
  shaderProgram.addShaderFromSourceCode(QGLShader::Vertex, vertexStr);
  shaderProgram.addShaderFromSourceCode(QGLShader::Fragment, fragmentStr);
  // 设置顶点坐标
  shaderProgram.bindAttributeLocation("vertexIn", 3);
  // 设置材质坐标
  shaderProgram.bindAttributeLocation("textureIn", 4);
  qDebug() << "shaderProgram.link()" << shaderProgram.link();
  qDebug() << "shaderProgram.bind()" << shaderProgram.bind();
  // 传递顶点和材质坐标
  //  顶点坐标
  static const GLfloat vertexLocation[] = {-1.0f, -1.0f, 1.0f, -1.0f,
                                           -1.0f, 1.0f,  1.0f, 1.0f};
  // 材质坐标
  static const GLfloat textureLocation[] = {0.0f, 1.0f, 1.0f, 1.0f,
                                            0.0f, 0.0f, 1.0f, 0.0f};
  // 顶点
  glVertexAttribPointer(3, 2, GL_FLOAT, 0, 0, vertexLocation);
  glEnableVertexAttribArray(3);
  // 材质
  glVertexAttribPointer(4, 2, GL_FLOAT, 0, 0, textureLocation);
  glEnableVertexAttribArray(4);
  // 从shader获取材质
  uniformLocations[0] = shaderProgram.uniformLocation("tex_y");
  uniformLocations[1] = shaderProgram.uniformLocation("tex_u");
  uniformLocations[2] = shaderProgram.uniformLocation("tex_v");
  lock.unlock();
}
void VideoPlayWidget::resizeGL(int w, int h) {
  lock.lock();
  qDebug() << "重绘" << w << "x" << h;
  lock.unlock();
}
void VideoPlayWidget::paintGL() {
  lock.lock();
  glActiveTexture(GL_TEXTURE0);
  // 0层绑定到Y材质
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  // 修改材质内容
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED,
                  GL_UNSIGNED_BYTE, pixels[0]);
  // 与shader uniform关联
  glUniform1i(uniformLocations[0], 0);
  glActiveTexture(GL_TEXTURE0 + 1);
  // 1层绑定到U材质
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  // 修改材质内容
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED,
                  GL_UNSIGNED_BYTE, pixels[1]);
  // 与shader uniform关联
  glUniform1i(uniformLocations[1], 1);
  glActiveTexture(GL_TEXTURE0 + 2);
  // 2层绑定到V材质
  glBindTexture(GL_TEXTURE_2D, textures[2]);
  // 修改材质内容
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED,
                  GL_UNSIGNED_BYTE, pixels[2]);
  // 与shader uniform关联
  glUniform1i(uniformLocations[2], 2);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  lock.unlock();
}
