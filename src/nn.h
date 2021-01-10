#ifndef NN_H
#define NN_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class NN : public QObject
{
    Q_OBJECT
public:
    NN(QObject *parent = nullptr);
    ~NN();

signals:
    void NNResultReady(int res);

public slots:
    void transform(double beginltt,double beginlat,double endltt,double endlat,
                   int weekday, int startTime);
    void caculate();

private:
    QVector<double> calc(QVector<double>input,QVector<QVector<double>> parms);
    QVector<double> Relu(QVector<double>input);

private:
    //parameters used for feature transform
    QVector<double> radius;
    QVector<double> input;

    //Network weights and bias
    QVector<QVector<double>> layer1;
    QVector<QVector<double>> layer2;
    QVector<QVector<double>> layer3;
    QVector<QVector<double>> layer4;
    QVector<QVector<double>> layer5;
    QVector<QVector<double>> layer6;

    int output;
};

#endif // NN_H
