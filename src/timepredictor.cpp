#include "timepredictor.h"

TimePredictor::TimePredictor(QObject *parent)
{

}

TimePredictor::~TimePredictor()
{

}

void TimePredictor::estimateAverageTime(QPair<double,double> _origin,QPair<double,double> _destination,int leave,Dataset *_data)
{
    neibors.clear();
    origin = _origin;
    destination = _destination;
    data = _data;

    int startId = int((_origin.first - 103.9084075) / 0.0313637) + int((_origin.second - 30.52408195) / 0.02698) * 10;
    int sum = 0;
    qint64 init = 1477929600;

    //search similar orders
    for(int i = 0;i<data[startId].Leaveltt.size();++i)
    {//Choose Time and Space
        if(qAbs(data[startId].Leaveltt[i] - origin.first) < 0.001
      && qAbs(data[startId].leavelat[i] - origin.second) < 0.0009
      && qAbs(data[startId].Endltt[i] - destination.first) < 0.001
      && qAbs(data[startId].Endlat[i] - destination.second) <0.0009
      &&(qAbs((data[startId].LeaveTime[i] - init)/60 - leave)%1440 < 30 ||
         qAbs((data[startId].LeaveTime[i] - init)/60 - leave)%1440 > 1410))
            neibors.append(data[startId].EndTime[i] - data[startId].LeaveTime[i]);
    }

    //compute average travel time
    if(neibors.size() == 0) emit averageTimeFinished(0);
    else{
        for(int i = 0;i<neibors.size();++i)
        {
            sum += neibors[i];
        }
        emit averageTimeFinished(sum/neibors.size());
    }

    return;
}
