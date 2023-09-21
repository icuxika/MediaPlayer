# MediaPlayer

一个基于 FFmpeg、Qt 和 OpenGL 实现的简单播放器。

## 目录结构

-   **MediaPlayer**
    -   **ffmpeg**
        -   **bin** &ensp;&ensp;&ensp;&ensp;&thinsp;ffmpeg-4.2.2-win64-shared\bin
        -   **include** &ensp;ffmpeg-4.2.2-win64-dev\include
        -   **lib** &ensp;&ensp;&ensp;&ensp;&ensp;&thinsp;ffmpeg-4.2.2-win64-dev\lib
    -   \***\*.h**
    -   \***\*.cpp**
    -   **.gitignore**
    -   **MediaPlayer.pro**

## **TODO**

-   **v0.9.0**
    -   [✔]可以播放普通的 H264 视频
    -   [✔]支持暂停、快进（拖动滑块快进和点击进度条快进）
    -   [✖]支持打开 URL 播放
    -   [✖]支持打开文件夹
    -   [✖]UI 美化
    -   [✖]待解决问题
        -   yuv420p 播放部分视频播放器会崩溃
        -   yuv420p10le 10bit 暂不支持

## **笔记**

-   **Qt Creator 配置 MSVC 的 Debugger**
    -   控制面板-程序-程序和功能-Windows Software Development Kit 右击更改配置

```
cmake -S . -B build -DCMAKE_CXX_FLAGS="/utf-8"
cmake --build build --config Release
cp C:\CommandLineTools\ffmpeg-6.0-full_build-shared\bin\*.dll .\build\Release\
cd .\build\Release\
set-path -Path D:\Programs\Qt\5.15.2\msvc2019_64\bin
windeployqt.exe .\MediaPlayer.exe
```
