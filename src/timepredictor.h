#ifndef TIMEPREDICTOR_H
#define TIMEPREDICTOR_H

#include <QObject>
#include <QtDebug>
#include <QtAlgorithms>

#include "dataset.h"

class TimePredictor : public QObject
{
    Q_OBJECT
public:
    TimePredictor(QObject *parent = nullptr);
    ~TimePredictor();

signals:
    void averageTimeFinished(int average);

public slots:
    void estimateAverageTime(QPair<double,double> _origin,QPair<double,double> _destination,int leave,Dataset *_data);

private:
    QPair<double,double> origin;
    QPair<double,double> destination;
    Dataset *data;
    QVector<int> neibors;
};

#endif // TIMEPREDICTOR_H
