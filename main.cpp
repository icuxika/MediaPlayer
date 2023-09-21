#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[]) {
  // 使 qDebug 输出不乱码
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForLocale(codec);

  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
