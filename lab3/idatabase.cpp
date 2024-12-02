#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database=QSqlDatabase::addDatabase("QSQLITE");
    QString aFile ="../../lab3.db";
    database.setDatabaseName(aFile);

    if(!database.open()){
        qDebug()<<"fail to open database";
    }
    else
        qDebug()<<"open database is ok";
}

bool IDatabase::initPatientModel()
{
    patientTabModel =new QSqlTableModel(this,database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(
        QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("name"),Qt::AscendingOrder);
    if(!(patientTabModel->select()))
        return false;

    thePatientSelection=new QItemSelectionModel(patientTabModel);
    return true;


}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRows(patientTabModel->rowCount(), 1);  // 正确的插入方法，插入一行数据

    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1, 1);  // 获取新插入的行的索引

    int curRecNo =curIndex.row();
    QSqlRecord curRec=patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID",QUuid::createUuid().toString(QUuid::WithoutBraces));
    patientTabModel->setRecord(curRecNo,curRec);

    return curIndex.row();  // 返回插入行的行号
}


bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::delectcurrentPatient()
{
    QModelIndex curIndex=
        thePatientSelection->currentIndex();
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

bool IDatabase::reverPatientEdit()
{
    patientTabModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString password)
{
    QSqlQuery query;
    query.prepare("SELECT username, password FROM user WHERE username = :USER");
    query.bindValue(":USER", userName);

    if (query.exec()) {
        if (query.first()) {
            QString passwd = query.value("password").toString();
            if (passwd == password) {
                return "loginOK";
            } else {
                return "wrongPassword";
            }
        } else {
            qDebug() << "No such user found.";
            return "loginFailed";
        }
    } else {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return "queryFailed";
    }
}


IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    ininDatabase();
}
