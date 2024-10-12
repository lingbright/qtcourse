#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 连接数字按钮的信号和槽
    connect(ui->pushButton_0, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_1, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_7, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_8, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnNumClicked()
{
    QString str = ui->display->text();
    str += qobject_cast<QPushButton*>(sender())->text();

    ui->display->setText(str);
    ui->statusbar->showMessage(qobject_cast<QPushButton*>(sender())->text() + " 按钮被点击");
}

void MainWindow::on_dian_clicked()
{
    QString str = ui->display->text();

    // 检查当前字符串是否已经包含小数点
    if (!str.contains(".")) {
        str += "."; // 只在没有小数点的情况下添加小数点
    }

    ui->display->setText(str);
    ui->statusbar->showMessage("小数点按钮被点击");
}

void MainWindow::on_shanchu_clicked()
{
    QString str = ui->display->text(); // 获取当前显示的文本

    if (!str.isEmpty()) { // 检查字符串是否为空
        str.chop(1); // 删除最后一个字符
    }

    ui->display->setText(str); // 更新显示框
    ui->statusbar->showMessage("删除按钮被点击");
}

double currentValue = 0.0;
QString pendingOperation; // 存储待执行的操作
bool lastOperationValid = true; // 标志上一个运算是否有效
bool waitingForNewValue = false; // 标志是否在等待新的操作数

void MainWindow::performPendingOperation()
{
    if (!pendingOperation.isEmpty() && lastOperationValid) {
        QString str = ui->display->text();
        double newValue = str.toDouble();
        double result = 0.0;

        if (pendingOperation == "add") {
            result = currentValue + newValue;
        } else if (pendingOperation == "subtract") {
            result = currentValue - newValue;
        } else if (pendingOperation == "multiply") {
            result = currentValue * newValue;
        } else if (pendingOperation == "divide") {
            if (newValue != 0) {
                result = currentValue / newValue;
            } else {
                ui->display->setText("不能除以0");
                lastOperationValid = false;
                return;
            }
        }

        ui->display->setText(QString::number(result, 'g', 12)); // 保持精度
        currentValue = result;
        pendingOperation.clear();
        waitingForNewValue = true;
    }
}

void MainWindow::on_chu_clicked()
{
    if (!lastOperationValid) {
        ui->display->setText("无效运算");
        return;
    }

    performPendingOperation(); // 执行上一次操作

    QString str = ui->display->text();
    if (!str.isEmpty()) {
        currentValue = str.toDouble();
        pendingOperation = "divide";
        ui->display->clear();
    }
}

void MainWindow::on_chen_clicked()
{
    if (!lastOperationValid) {
        ui->display->setText("无效运算");
        return;
    }

    performPendingOperation();

    QString str = ui->display->text();
    if (!str.isEmpty()) {
        currentValue = str.toDouble();
        pendingOperation = "multiply";
        ui->display->clear();
    }
}

void MainWindow::on_jian_clicked()
{
    if (!lastOperationValid) {
        ui->display->setText("无效运算");
        return;
    }

    performPendingOperation();

    QString str = ui->display->text();
    if (!str.isEmpty()) {
        currentValue = str.toDouble();
        pendingOperation = "subtract";
        ui->display->clear();
    }
}

void MainWindow::on_jia_clicked()
{
    if (!lastOperationValid) {
        ui->display->setText("无效运算");
        return;
    }

    performPendingOperation();

    QString str = ui->display->text();
    if (!str.isEmpty()) {
        currentValue = str.toDouble();
        pendingOperation = "add";
        ui->display->clear();
    }
}

void MainWindow::on_dengyu_clicked()
{
    performPendingOperation();
}

void MainWindow::on_zhengfu_clicked()
{
    QString str = ui->display->text();
    if (!str.isEmpty()) {
        double value = str.toDouble();
        value = -value; // 取反
        ui->display->setText(QString::number(value, 'g', 12)); // 保持精度
    }
}

void MainWindow::on_baifenhao_clicked()
{
    QString str = ui->display->text();
    if (!str.isEmpty()) {
        double value = str.toDouble();
        value /= 100; // 转换为百分比
        ui->display->setText(QString::number(value, 'g', 12)); // 保持精度
    }
}

void MainWindow::on_pushButton_CE_clicked()
{
    ui->display->clear();           // 清空显示框
    currentValue = 0.0;
    pendingOperation.clear();       // 清除待执行的操作
    lastOperationValid = true;      // 重置有效状态，允许新的运算
    waitingForNewValue = false;
    ui->statusbar->clearMessage();
}


void MainWindow::on_pushButton_C_clicked()
{
    ui->display->clear(); // 清空显示框
}

void MainWindow::on_daoshu_clicked()
{
    QString str = ui->display->text();
    if (!str.isEmpty()) {
        double value = str.toDouble();
        if (value != 0) {
            value = 1.0 / value; // 计算倒数
            ui->display->setText(QString::number(value, 'g', 12)); // 保持精度
        } else {
            ui->statusbar->showMessage("错误：不能除以零！");
        }
    }
}

void MainWindow::on_pingfang_clicked()
{
    QString str = ui->display->text();
    if (!str.isEmpty()) {
        double value = str.toDouble();
        value = value * value; // 计算平方
        ui->display->setText(QString::number(value, 'g', 12)); // 保持精度
    }
}

void MainWindow::on_genhao_clicked()
{
    QString str = ui->display->text();
    if (!str.isEmpty()) {
        double value = str.toDouble();
        if (value >= 0) {
            value = std::sqrt(value); // 计算平方根
            ui->display->setText(QString::number(value, 'g', 12)); // 保持精度
        } else {
            ui->statusbar->showMessage("错误：负数没有平方根！");
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_0:
        ui->pushButton_0->click(); // 模拟按下 0 按钮
        break;
    case Qt::Key_1:
        ui->pushButton_1->click(); // 模拟按下 1 按钮
        break;
    case Qt::Key_2:
        ui->pushButton_2->click();
        break;
    case Qt::Key_3:
        ui->pushButton_3->click();
        break;
    case Qt::Key_4:
        ui->pushButton_4->click();
        break;
    case Qt::Key_5:
        ui->pushButton_5->click();
        break;
    case Qt::Key_6:
        ui->pushButton_6->click();
        break;
    case Qt::Key_7:
        ui->pushButton_7->click();
        break;
    case Qt::Key_8:
        ui->pushButton_8->click();
        break;
    case Qt::Key_9:
        ui->pushButton_9->click();
        break;
    case Qt::Key_Plus:
        ui->jia->click(); // 模拟加号
        break;
    case Qt::Key_Minus:
        ui->jian->click(); // 模拟减号
        break;
    case Qt::Key_Asterisk:
        ui->chen->click(); // 模拟乘号
        break;
    case Qt::Key_Slash:
        ui->chu->click(); // 模拟除号
        break;
    case Qt::Key_Period:
        ui->dian->click(); // 小数点
        break;
    case Qt::Key_Equal:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        ui->dengyu->click(); // 等号
        break;
    case Qt::Key_Backspace:
        ui->shanchu->click(); // 删除按钮
        break;
    case Qt::Key_Escape:
        ui->pushButton_CE->click(); // 清除
        break;
    default:
        QMainWindow::keyPressEvent(event); // 其他按键保持默认行为
    }
}

