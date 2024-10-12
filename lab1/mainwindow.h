#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override; // 重写键盘事件处理函数


private slots:
    void btnNumClicked();
    void on_dian_clicked();
    void on_shanchu_clicked();
    void on_chu_clicked();
    void on_chen_clicked();
    void on_jian_clicked();
    void on_jia_clicked();
    void on_dengyu_clicked();
    void on_zhengfu_clicked();
    void on_baifenhao_clicked();
    void on_pushButton_CE_clicked();
    void on_pushButton_C_clicked();
    void on_daoshu_clicked();
    void on_pingfang_clicked();
    void on_genhao_clicked();


private:
    Ui::MainWindow *ui;
    QLabel *statusMsgLabel;

    // 声明 performPendingOperation 函数
    void performPendingOperation();
};

#endif // MAINWINDOW_H
