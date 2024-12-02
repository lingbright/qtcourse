#ifndef PATIENTVIEW_H
#define PATIENTVIEW_H

#include <QWidget>

namespace Ui {
class PatientView;
}

class PatientView : public QWidget
{
    Q_OBJECT

public:
    explicit PatientView(QWidget *parent = nullptr);
    ~PatientView();

private slots:
    void on_PatientAdd_clicked();

    void on_PatientSearch_clicked();

    void on_PatientDelete_clicked();

    void on_PatientEdit_clicked();

signals:
    void goPatientEditView(int);

private:
    Ui::PatientView *ui;
};

#endif // PATIENTVIEW_H
