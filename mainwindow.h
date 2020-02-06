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

    void timerEvent(QTimerEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void SetPause(bool isPause);

    virtual void OpenFile();
    virtual void PlayOrPause();
    virtual void progressBarPress();
    virtual void progressBarRelease();

    VideoPlayWidget* videoPlayWidget = nullptr;
    DemuxThreadUtil demuxThreadUtil;

    QSlider* progressBar = nullptr;
    QPushButton* openFileButton = nullptr;
    QPushButton* playOrPauseButton = nullptr;
private:
    bool isProcessBarPress = false;
};
#endif // MAINWINDOW_H
