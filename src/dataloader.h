#ifndef DATALOADER_H
#define DATALOADER_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QVector>
#include <QMessageBox>
#include <QApplication>

#include "dataset.h"

class DataLoader : public QObject
{
    Q_OBJECT
public:
    DataLoader(QObject *parent = nullptr,QString filepath = "",QString startTime = "",
               QString endTime = "", QVector<int> fields = {0});
    ~DataLoader();

signals:
    void progress(int percent);
    void resultReady(Dataset *data);

public slots:
    void startLoading();

private:
    QString path;
    QString startTime;
    QString endTime;
    QVector<int> fields;
};

#endif // DATALOADER_H
