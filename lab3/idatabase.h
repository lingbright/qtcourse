#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include<Qtsql>
#include<qsqldatabase>
#include<qdatawidgetmapper.h>

class IDatabase : public QObject
{
    Q_OBJECT
public:
    static IDatabase& getInstance()
    {
        static IDatabase    instance;
        return instance;
    }

    QString userLogin(QString userName,QString password);

private:
    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const&)    = delete;
    void operator=(IDatabase const&)  = delete;

    QSqlDatabase database;

    void ininDatabase();

signals:
public:
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    bool delectcurrentPatient();
    bool submitPatientEdit();
    bool reverPatientEdit();

    QSqlTableModel *patientTabModel;
    QItemSelectionModel *thePatientSelection;


};

#endif // IDATABASE_H
