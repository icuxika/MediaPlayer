#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QHBoxLayout>
#include <QPushButton>
#include <QOpenGLWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QSlider>
#include <QScreen>
#include <QRect>
#include <QList>
#include <QGuiApplication>

#include "demuxutil.h"
#include "decodeutil.h"
#include "resampleutil.h"
#include "audiothreadutil.h"
#include "videothreadutil.h"
#include "videoplaywidget.h"
#include "demuxthreadutil.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    // 定时刷新播放器进度条
    void timerEvent(QTimerEvent* event);
    // 界面尺寸改变事件
    void resizeEvent(QResizeEvent* event);
    // 进入与退出播放器全屏
    void mouseDoubleClickEvent(QMouseEvent* event);
    // 设置播放和暂停按钮状态
    void SetPause(bool isPause);

    // 打开播放目标文件
    virtual void OpenFile();
    // 播放和暂停
    virtual void PlayOrPause();
    // 点击进度条（不松）
    virtual void progressBarPress();
    // 快进 点击进度条（松开）
    virtual void progressBarRelease();

    // 视频显示组件
    VideoPlayWidget* videoPlayWidget = nullptr;
    // 音视频控制线程
    DemuxThreadUtil demuxThreadUtil;
    // 进度条
    QSlider* progressBar = nullptr;
    // 打开文件按钮
    QPushButton* openFileButton = nullptr;
    // 播放和暂停按钮
    QPushButton* playOrPauseButton = nullptr;
private:
    // 判断进度条是否被点击（不松）
    bool isProcessBarPress = false;
};
#endif // MAINWINDOW_H
