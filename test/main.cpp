#include "mainwindow.h"

#include <QApplication>
#include<QWidget>
#include<Qlabel>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QWidget e;
    e.resize(400,400);
    e.setWindowTitle("hello world");
    QLabel textLabel;
    textLabel.resize(100,15);
    textLabel.move(150,170);
    textLabel.setText("hello world");
    textLabel.setParent(&e);
    textLabel.show();
    e.show();
    return a.exec();
}
