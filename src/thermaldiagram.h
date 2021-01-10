#ifndef THERMALDIAGRAM_H
#define THERMALDIAGRAM_H

#include "dataset.h"

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QRadialGradient>

class ThermalDiagram : public QWidget
{
    Q_OBJECT

public:
    ThermalDiagram(QWidget *parent = nullptr);
    ~ThermalDiagram();

signals:
    void finishedImage(QPixmap heatMap);

public slots:
    void counting(double beginltt, double beginlat, double endltt,
                  double endlat, QVariant, Dataset * data, int startTime,
                  int timeStep, QPixmap sourceMap);
    void drawMap(QPixmap sourceMap);

private:
    int _maxCount = 1;
    QMap<QPair<int,int>,int> Counter;
    QImage *_dataImg;
    QImage *_heatImg;
    QRgb _colorList[256];
    bool flag = false;
};

#endif // THERMALDIAGRAM_H
