#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(QString::fromLocal8Bit("Media Player"));
//    this->setFixedSize(1280, 720);
    // 获取屏幕宽度和高度
    QList<QScreen*> list_screen = QGuiApplication::screens();
    QRect rect = list_screen.at(0)->geometry();
    this->setGeometry((rect.width() - 1280) / 2, (rect.height() - 760) / 2, 1280, 760);
    videoPlayWidget = new VideoPlayWidget();
//    videoPlayWidget->setFixedSize(1280, 720);
//    videoPlayWidget->setGeometry(0, 0, 1280, 720);
    this->setCentralWidget(videoPlayWidget);
    // 菜单栏部分--------------------------------------------------
    QMenu* fileMenu = menuBar()->addMenu(QString::fromLocal8Bit("文件"));
    QAction* openAction = new QAction(QString::fromLocal8Bit("打开"), this);
    fileMenu->addAction(openAction);
    // 状态栏部分--------------------------------------------------
    progressBar = new QSlider(this);
    progressBar->setOrientation(Qt::Horizontal);
    progressBar->setMinimum(0);
    progressBar->setMaximum(1000);
    progressBar->setSingleStep(1);
    openFileButton = new QPushButton(QString::fromLocal8Bit("打开文件"));
    playOrPauseButton = new QPushButton(QString::fromLocal8Bit("播放"));
    playOrPauseButton->setEnabled(false);
    statusBar()->addWidget(progressBar);
    statusBar()->addWidget(openFileButton);
    statusBar()->addWidget(playOrPauseButton);
    connect(openAction, &QAction::triggered, this, &MainWindow::OpenFile);
    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::OpenFile);
    connect(playOrPauseButton, &QPushButton::clicked, this, &MainWindow::PlayOrPause);
    connect(progressBar, &QSlider::sliderPressed, this, &MainWindow::progressBarPress);
    connect(progressBar, &QSlider::sliderReleased, this, &MainWindow::progressBarRelease);
    // 更新播放进度
    startTimer(40);
}

MainWindow::~MainWindow()
{
    demuxThreadUtil.Close();
}

void MainWindow::OpenFile()
{
    QString fileUrl = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件："));
    if (fileUrl.isEmpty())
    {
        return;
    }
    demuxThreadUtil.StartThreads();
    this->setWindowTitle(fileUrl);
    if (!demuxThreadUtil.Open(fileUrl.toLocal8Bit(), videoPlayWidget))
    {
        QMessageBox::information(nullptr, "ERROR", QString::fromLocal8Bit("无法打开文件"));
        return;
    }
    playOrPauseButton->setEnabled(true);
    SetPause(demuxThreadUtil.isPause);
}

void MainWindow::PlayOrPause()
{
    bool isPause = !demuxThreadUtil.isPause;
    SetPause(isPause);
    demuxThreadUtil.SetPause(isPause);
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    if (isProcessBarPress)
    {
        return;
    }
    long long totalMilliseconds = demuxThreadUtil.totalMilliseconds;
    if (totalMilliseconds > 0)
    {
//        qDebug() << QString::fromLocal8Bit("totalMilliseconds:") << totalMilliseconds << QString::fromLocal8Bit("demuxThreadUtil.pts:") << demuxThreadUtil.pts;
        double pos = (double)demuxThreadUtil.pts / (double)totalMilliseconds;
        int value = progressBar->maximum() * pos;
        progressBar->setValue(value);
    }
}
void MainWindow::resizeEvent(QResizeEvent* event)
{
}
void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (isFullScreen())
    {
        this->showNormal();
    }
    else
    {
        this->showFullScreen();
    }
}
void MainWindow::SetPause(bool isPause)
{
    if (isPause)
    {
        playOrPauseButton->setText(QString::fromLocal8Bit("播放"));
    }
    else
    {
        playOrPauseButton->setText(QString::fromLocal8Bit("暂停"));
    }
}

void MainWindow::progressBarPress()
{
    isProcessBarPress = true;
}
void MainWindow::progressBarRelease()
{
    isProcessBarPress = false;
    double pos = 0.0;
    pos = (double)progressBar->value() / (double)progressBar->maximum();
    qDebug() << QString::fromLocal8Bit("进度条目标位置:") << pos;
    demuxThreadUtil.Seek(pos);
}

