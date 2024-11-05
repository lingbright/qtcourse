#include "ui_replacedialog.h"
#include "replacedialog.h"
#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    connect(ui->btFindNext, &QPushButton::clicked, this, &ReplaceDialog::on_btFindNext_clicked);
    connect(ui->btReplace, &QPushButton::clicked, this, &ReplaceDialog::on_btReplace_clicked);
    connect(ui->btReplaceall, &QPushButton::clicked, this, &ReplaceDialog::on_btReplaceall_clicked);
    connect(ui->btCancel, &QPushButton::clicked, this, &ReplaceDialog::on_btCancel_clicked);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::on_btFindNext_clicked()
{
    QString searchText = ui->searchtext->text();
    bool caseSensitive = ui->checkBox->isChecked();
    bool searchDown = ui->rbDown->isChecked();

    emit rreplaceText(searchText, QString(), caseSensitive, false, searchDown);
}

void ReplaceDialog::on_btReplace_clicked()
{
    QString searchText = ui->searchtext->text();
    QString replaceText = ui->lineEdit->text();
    bool caseSensitive = ui->checkBox->isChecked();
    bool searchDown = ui->rbDown->isChecked();

    emit rreplaceText(searchText, replaceText, caseSensitive, false, searchDown);
}

void ReplaceDialog::on_btReplaceall_clicked()
{
    QString searchText = ui->searchtext->text();
    QString replaceText = ui->lineEdit->text();
    bool caseSensitive = ui->checkBox->isChecked();

    emit rreplaceText(searchText, replaceText, caseSensitive, true, false);
}


void ReplaceDialog::on_btCancel_clicked()
{
    close();
}
