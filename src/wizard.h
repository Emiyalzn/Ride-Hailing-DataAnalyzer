#ifndef WIZARD_H
#define WIZARD_H

#include <QDialog>
#include <QCheckBox>
#include <QMap>
#include <QDateTime>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <QDateTimeEdit>

Q_DECLARE_METATYPE(QVector<int>);

namespace Ui {
class Wizard;
}

class Wizard : public QDialog
{
    Q_OBJECT

public:
    explicit Wizard(QWidget *parent = nullptr);
    ~Wizard();

signals:
    void resultReady(QString path, QString startTime, QString endTime, QVector<int> fields);

private slots:
    void buttonClicked();
    void setStartTime(QDateTime _start);
    void setEndTime(QDateTime _end);
    void dealWithOK();

private:
    Ui::Wizard *ui;
    QString path;
    QDateTime startTime = QDateTime::fromString(QString("2016-11-01 08:00"), "yyyy-MM-dd hh:mm");
    QDateTime endTime = QDateTime::fromString(QString("2016-11-15 08:00"), "yyyy-MM-dd hh:mm");
    bool configure(QString path);
};

#endif // WIZARD_H
