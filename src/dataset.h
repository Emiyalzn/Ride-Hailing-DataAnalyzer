#ifndef DATASET_H
#define DATASET_H

#include <QObject>
#include <QDateTime>
#include <QMap>

class Dataset : public QObject
{
    Q_OBJECT
public:
    Dataset();

    QVector<qint64> LeaveTime;
    QVector<qint64> EndTime;
    QVector<double> Leaveltt;
    QVector<double> leavelat;
    QVector<double> Endltt;
    QVector<double> Endlat;
    QVector<double> fee;
    QMap<int,QVector<int>> startID;
    QMap<int,QVector<int>> endID;
};

#endif // DATASET_H
