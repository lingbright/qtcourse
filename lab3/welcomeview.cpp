#include "welcomeview.h"
#include "ui_welcomeview.h"

WelcomeView::WelcomeView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeView)
{
    ui->setupUi(this);
}

WelcomeView::~WelcomeView()
{
    delete ui;
}

void WelcomeView::on_keshiguanli_clicked()
{
    emit goDepartmentView();
}


void WelcomeView::on_yishengguanli_clicked()
{
    emit goDoctorView();
}


void WelcomeView::on_huanzheguanli_clicked()
{
    emit goPatientView();
}

