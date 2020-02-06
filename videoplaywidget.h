#ifndef VIDEOPLAYWIDGET_H
#define VIDEOPLAYWIDGET_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QTimer>
#include <QDebug>
#include <mutex>

extern "C" {
#include <libavutil/frame.h>
}

class VideoPlayWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    VideoPlayWidget(QWidget* parent = nullptr);
    virtual ~VideoPlayWidget();

    virtual void Init(int width, int height);
    virtual void Repaint(AVFrame* frame);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    std::mutex lock;

    // Shader程序
    QGLShaderProgram shaderProgram;

    // shader中yuv变量地址
    GLuint uniformLocations[3] = {0};
    // OpenGL的texture地址
    GLuint textures[3] = {0};

    unsigned char* pixels[3] = {nullptr};

    int width = 0;
    int height = 0;
};

#endif // VIDEOPLAYWIDGET_H
