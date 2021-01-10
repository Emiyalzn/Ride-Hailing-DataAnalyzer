//
//Created on 2020-12-06
//

#include "drawchart.h"

QMutex mutex;

DrawChart::DrawChart(QObject *parent,QVector<int> _fields): fields(_fields)
{
    //register metatype to emit signals with parameters haven't register before
    qRegisterMetaType<QList<QPointF>>("QList<QPointF>");
}

DrawChart::~DrawChart()
{

}

void DrawChart::drawOrderChart(QDateTime startTime, QDateTime endTime,
                             int timeStep, QVariant dataVar, Dataset * data)
{
    mutex.lock();

    int _fields[8] = {0};
    for(int m=0;m<fields.size();++m){
        _fields[fields[m]] = 1;
    }

    QDateTime _startTime = startTime;
    QDateTime _endTime = endTime;
    int tsp = timeStep;
    QVector<int> _grids = dataVar.value<QVector<int>>();
    Dataset *_data = data;

    qint64 _start = _startTime.toSecsSinceEpoch();
    qint64 _end = _endTime.toSecsSinceEpoch();

    QList<QPointF> series_1;
    QList<QPointF> series_2;


    qint64 init = 1477929600;

    //Prepare for counting
    int num = (_end - _start)/60 + 1;

    QDateTime time;
    int count_1 = 0;
    int count_2 = 0;
    int max = 0;

    int delta = (_start - init)/60;

    //traverse and count the order number
    //draw leave and enter lines
    if(_fields[1]&&_fields[2]){
    for(int i=1;i<=num;i++){
        time = time.fromSecsSinceEpoch(_start + (i - 1) * 60);
        for(int j = 0;j < _grids.size();++j){
            count_1 += _data[_grids[j]].startID[(delta) + (i - 1)].size();
            count_2 += _data[_grids[j]].endID[(delta) + (i - 1)].size();
        }
    //add points to series when reaching a time step
        if(i % tsp == 0){
            series_1.append(QPointF(time.toMSecsSinceEpoch() - (tsp-1) * 60,count_1));
            series_2.append(QPointF(time.toMSecsSinceEpoch() - (tsp-1) * 60,count_2));
            if(count_1 > max) max = count_1;
            if(count_2 > max) max = count_2;
            count_1 = 0;
            count_2 = 0;
        }
      }
    }
    //only draw leave line
    else if(_fields[1]){
        for(int i=1;i<=num;i++){
            time = time.fromSecsSinceEpoch(_start + (i - 1) * 60);
            for(int j = 0;j < _grids.size();++j)
                count_1 += _data[_grids[j]].startID[(delta) + (i - 1)].size();
            if(i % tsp == 0){
                series_1.append(QPointF(time.toMSecsSinceEpoch() - (tsp-1) * 60,count_1));
                if(count_1 > max) max = count_1;
                count_1 = 0;
            }
        }
    }
    //only draw enter line
    else {
        for(int i=1;i<=num;i++){
            time = time.fromSecsSinceEpoch(_start + (i - 1) * 60);
            for(int j = 0;j < _grids.size();++j)
                count_2 += _data[_grids[j]].endID[(delta) + (i - 1)].size();
            if(i % tsp == 0){
                series_2.append(QPointF(time.toMSecsSinceEpoch() - (tsp-1) * 60,count_2));
                if(count_2 > max) max = count_2;
                count_2 = 0;
            }
        }
    }

    mutex.unlock();

    //move series to the main thread
    emit orderChartReady(series_1,series_2,max);
    return;
}


void DrawChart::drawTravelTimeChart(QDateTime startTime, QDateTime endTime,
                         QVariant dataVar, Dataset * data)
{
    mutex.lock();

    QVector<int> _grids = dataVar.value<QVector<int>>();
    qint64 _start = startTime.toSecsSinceEpoch();
    qint64 _end = endTime.toSecsSinceEpoch();

    QList<QPointF> series;

    qint64 init = 1477929600;

    int num = (_end - _start)/60;

    QMap<int,int> counter;
    int travelTime = 0;
    int max = 0;
    int delta = (_start - init)/60;

    //traverse and count each travel time's order number
    for(int i=1;i<=num;++i){
        for(int j = 0;j < _grids.size();j++){
            for(int m = 0;m < data[_grids[j]].startID[(delta) + (i - 1)].size();m++){
                //use startID map to accelerate
                qint64 start = data[_grids[j]].LeaveTime[data[_grids[j]].startID[(delta) + (i - 1)][m]];
                qint64 end = data[_grids[j]].EndTime[data[_grids[j]].startID[(delta) + (i - 1)][m]];

                travelTime = (end - start)/60;

                if(!counter[travelTime]) counter[travelTime] = 1;
                else counter[travelTime]++;
            }
        }
    }

    mutex.unlock();

    emit travelTimeChartReady(counter,max);
    return;
}

void DrawChart::drawFeeChart(QDateTime startTime, QDateTime endTime,
                  QVariant dataVar, Dataset * data)
{
    mutex.lock();

    QVector<int> _grids = dataVar.value<QVector<int>>();
    qint64 _start = startTime.toSecsSinceEpoch();
    qint64 _end = endTime.toSecsSinceEpoch();

    QList<QPointF> series;

    qint64 init = 1477929600;

    int num = (_end - _start)/60;

    QMap<int,int> counter;

    int fee = 0;
    int max = 0;
    int delta = (_start - init)/60;

    //traverse and count each fee's order number
    for(int i=1;i<=num;++i){
        for(int j = 0;j < _grids.size();j++){
            for(int m = 0;m < data[_grids[j]].startID[(delta) + (i - 1)].size();m++){
                fee = int(data[_grids[j]].fee[data[_grids[j]].startID[(delta) + (i - 1)][m]] * 10);
                if(!counter[fee]) counter[fee] = 1;
                else counter[fee]++;
            }
        }
    }

    mutex.unlock();

    emit feeChartReady(counter,max);
    return;
}



void DrawChart::drawRevenueChart(QDateTime startTime, QDateTime endTime,
                      int timeStep, QVariant dataVar, Dataset * data)
{
    mutex.lock();

    QVector<int> _grids = dataVar.value<QVector<int>>();
    qint64 _start = startTime.toSecsSinceEpoch();
    qint64 _end = endTime.toSecsSinceEpoch();

    QList<QPointF> series;

    qint64 init = 1477929600;

    int num = (_end - _start)/60;

    QDateTime time;
    int revenue = 0;
    int delta = (_start - init)/60;
    int max = 0;

    //traverse and compute the total revenue during a time span
    for(int i=1;i<=num;++i){
        time = time.fromSecsSinceEpoch(_start + (i - 1) * 60);
        for(int j = 0;j < _grids.size();j++){
            for(int m = 0;m < data[_grids[j]].startID[(delta) + (i - 1)].size();m++){
                revenue += data[_grids[j]].fee[data[_grids[j]].startID[(delta) + (i - 1)][m]];
            }
        }
        if(i % timeStep == 0){
            if(revenue > max) max = revenue;
            series.append(QPointF(time.toMSecsSinceEpoch() - (timeStep-1) * 60,revenue));
            revenue = 0;
        }
    }

    mutex.unlock();

    emit revenueChartReady(series,max);
    return;
}
