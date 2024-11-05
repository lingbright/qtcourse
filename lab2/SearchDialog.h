#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    ~SearchDialog();

signals:
    void findText(const QString &text, bool caseSensitive, bool searchDown);

private slots:
    void on_btFindNext_clicked();


private:
    Ui::SearchDialog *ui;
};

#endif // SEARCHDIALOG_H
