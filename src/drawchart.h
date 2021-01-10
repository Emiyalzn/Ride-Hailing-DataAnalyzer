#ifndef DRAWCHART_H
#define DRAWCHART_H

#include "dataset.h"

#include <QObject>
#include <QtCharts>
#include <QDateTime>
#include <QPainter>
#include <QMutex>

Q_DECLARE_METATYPE(QList<QPointF>);

class DrawChart : public QObject
{
    Q_OBJECT

public:
    DrawChart(QObject *parent = nullptr,QVector<int> _fields={0});
    ~DrawChart();

signals:
    void orderChartReady(QList<QPointF> _data1,QList<QPointF> _data2, int max);
    void travelTimeChartReady(QMap<int,int> _data, int max);
    void feeChartReady(QMap<int,int> _data, int max);
    void revenueChartReady(QList<QPointF> _data, int max);

public slots:
    void drawOrderChart(QDateTime startTime, QDateTime endTime,
                      int timeStep, QVariant, Dataset * data);
    void drawTravelTimeChart(QDateTime startTime, QDateTime endTime,
                             QVariant, Dataset * data);
    void drawFeeChart(QDateTime startTime, QDateTime endTime,
                      QVariant, Dataset * data);
    void drawRevenueChart(QDateTime startTime, QDateTime endTime,
                          int timeStep, QVariant, Dataset * data);

private:
    QVector<int> fields;
};

#endif // DRAWCHART_H
