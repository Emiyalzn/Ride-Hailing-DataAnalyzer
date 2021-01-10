//
//Created on 2020-12-16
//
#include "nn.h"
#include <cmath>

NN::NN(QObject *parent)
{
    //spatial classification we do for model training
    radius = {0.019667478, 0.029940668, 0.040076348, 0.0611749, 0.080543666, 0.091776206, 0.11, 0.133019267};

    //read parameters we train from the NN model
    QFile file(":/My/NN_model/model2.json");
    file.open(QIODevice::ReadOnly);
    QByteArray allData = file.readAll();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug()<<"Json Error!";
        return;
    }

    QVector<double> tmp;

    QJsonObject rootObj = jsonDoc.object();

    //store weights and bias
    for(int i = 0;i <= 10;i += 2)
    {
        QJsonArray weights = rootObj.value("features."+QString::number(i)+".weight").toArray();
        for(int j=0;j<weights.size();++j)
        {
            QJsonArray weight = weights[j].toArray();
            for(int m = 0;m<weight.size();++m)
            {
                tmp.append(weight[m].toDouble());
            }

            switch(i){
            case 0:
                layer1.append(tmp);
                break;
            case 2:
                layer2.append(tmp);
                break;
            case 4:
                layer3.append(tmp);
                break;
            case 6:
                layer4.append(tmp);
                break;
            case 8:
                layer5.append(tmp);
                break;
            case 10:
                layer6.append(tmp);
            }
            tmp.clear();
        }

        QJsonArray bias = rootObj.value("features."+QString::number(i)+".bias").toArray();
        for(int j=0;j<bias.size();++j)
        {
            tmp.append(bias[j].toDouble());
        }

        switch(i){
        case 0:
            layer1.append(tmp);
            break;
        case 2:
            layer2.append(tmp);
            break;
        case 4:
            layer3.append(tmp);
            break;
        case 6:
            layer4.append(tmp);
            break;
        case 8:
            layer5.append(tmp);
            break;
        case 10:
            layer6.append(tmp);
        }

        tmp.clear();
    }
}

//do feature transformation
void NN::transform(double beginltt,double beginlat,double endltt,double endlat,
                   int weekday, int startTime)
{
    input.clear();

    //model constants
    int init = 1477929600;
    const qreal bottomLat = 30.524081949676;
    const qreal topLat = 30.7938780503239;
    const qreal leftLon = 103.908407474531;
    const qreal rightLon = 104.222044525468;
    const qreal origLonMean = 0.161370855045317;
    const qreal origLonStd = 0.0391185328560802;
    const qreal origLatMean = 0.150093485852388;
    const qreal origLatStd = 0.0353629847333074;
    const qreal destLonMean = 0.161292627300344;
    const qreal destLonStd = 0.0398233707683038;
    const qreal destLatMean = 0.149508261654321;
    const qreal destLatStd = 0.0359904735316399;
    const qreal midLtt = (leftLon + rightLon)/2;
    const qreal midLat = (bottomLat + topLat)/2;

    //datetime one-hot
    if((startTime - init) % 86400 > 25200 && (startTime - init) % 86400 < 32400)
        input.append({1,0,0,0,0});
    else if((startTime - init) % 86400 > 61200 && (startTime - init) % 86400 < 68400)
        input.append({0,1,0,0,0});
    else if((startTime - init) % 86400 > 32400 && (startTime - init) % 86400 < 61200)
        input.append({0,0,1,0,0});
    else if((startTime - init) % 86400 > 68400 && (startTime - init) % 86400 < 79200)
        input.append({0,0,0,1,0});
    else input.append({0,0,0,0,1});

    //weekday one-hot
    if(weekday == 0 || weekday == 6)
        input.append({0,1});
    else input.append({1,0});

    //location standardization
    input.append(((beginltt - leftLon)-origLonMean)/origLonStd);
    input.append(((beginlat - bottomLat)-origLatMean)/origLatStd);

    //spatial one-hot
    double r = sqrt((beginlat-midLat)*(beginlat-midLat)+(beginltt-midLtt)*(beginltt-midLtt));
    int tmp = 0;
    for(int i = 0;i<radius.size();++i){
        if(r < radius[i])
            break;
        else tmp++;
    }
    for(int i=0;i<tmp;++i)
    {
        input.append(0);
    }
    input.append(1);
    for(int i=0;i<radius.size()-tmp;++i)
    {
        input.append(0);
    }

    input.append(((endltt - leftLon)-destLonMean)/destLonStd);
    input.append(((endlat - bottomLat)-destLatMean)/destLatStd);

    r = sqrt((endlat-midLat)*(endlat-midLat)+(endltt-midLtt)*(endltt-midLtt));
    tmp = 0;
    for(int i = 0;i<radius.size();++i){
        if(r < radius[i])
            break;
        else tmp++;
    }
    for(int i=0;i<tmp;++i)
    {
        input.append(0);
    }
    input.append(1);
    for(int i=0;i<radius.size()-tmp;++i)
    {
        input.append(0);
    }

    //get predict result from the NN model
    caculate();
}

void NN::caculate()
{
    const qreal travelTimeMean = 1280.0604648255412;
    const qreal travelTimeStd = 761.0889825884295;

    QVector<double> tmp;
    double res = 0;

    //layer settings
    tmp = calc(input,layer1);
    tmp = Relu(tmp);
    tmp = calc(tmp,layer2);
    tmp = Relu(tmp);
    tmp = calc(tmp,layer3);
    tmp = Relu(tmp);
    tmp = calc(tmp,layer4);
    tmp = Relu(tmp);
    tmp = calc(tmp,layer5);
    tmp = Relu(tmp);
    tmp = calc(tmp,layer6);

    for(int i=0;i<tmp.size();++i)
    {
        res += tmp[i];
    }

    double result = res * travelTimeStd + travelTimeMean;
    emit NNResultReady(int(result));
}

//vector multiplication
QVector<double> NN::calc(QVector<double>input,QVector<QVector<double>> params)
{
    QVector<double> tmp;
    double res=0;
    for(int i = 0;i<params.size()-1;++i)
    {
        for(int j=0;j<params[i].size();++j)
        {
            res += input[j] * params[i][j];
        }
        tmp.append(res+params[58][i]);
        res = 0;
    }
    return tmp;
}

//activation function
QVector<double> NN::Relu(QVector<double>input)
{
    for(int i=0;i<input.size();++i)
    {
        if(input[i] >= 0) continue;
        else input[i] = 0;
    }
    return input;
}

NN::~NN()
{

}
