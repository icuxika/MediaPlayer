# MediaPlayer
一个基于FFmpeg、Qt和OpenGL实现的简单播放器。

## 目录结构
- **MediaPlayer**
    - **ffmpeg**
        - **bin** &ensp;&ensp;&ensp;&ensp;&thinsp;ffmpeg-4.2.2-win64-shared\bin
        - **include** &ensp;ffmpeg-4.2.2-win64-dev\include
        - **lib** &ensp;&ensp;&ensp;&ensp;&ensp;&thinsp;ffmpeg-4.2.2-win64-dev\lib
    - ****.h**
    - ****.cpp**
    - **.gitignore**
    - **MediaPlayer.pro**

## **TODO**
- **v0.9.0**
    - [✔]可以播放普通的H264视频
    - [✔]支持暂停、快进
    - [✖]支持打开URL播放
    - [✖]支持打开文件夹
    - [✖]UI美化
    - [✖]待解决问题
        - yuv420p 播放部分视频播放器会崩溃
        - yuv420p10le 10bit暂不支持

## **笔记**
- **Qt Creator配置MSVC的Debugger**
    - 控制面板-程序-程序和功能-Windows Software Development Kit右击更改配置