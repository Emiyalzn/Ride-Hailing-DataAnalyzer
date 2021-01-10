//
//Created on 2020-12-02
//

#include "dataloader.h"
#include "dataset.h"

//Use function pointer array to implement elective loading
inline void add1(Dataset *_data,int id,QStringList tempbar){
    _data[id].LeaveTime.append(tempbar.at(1).toLongLong());
}
inline void add2(Dataset *_data,int id,QStringList tempbar){
    _data[id].EndTime.append(tempbar.at(2).toLongLong());
}
inline void add3(Dataset *_data,int id,QStringList tempbar){
    _data[id].Leaveltt.append(tempbar.at(3).toDouble());
}
inline void add4(Dataset *_data,int id,QStringList tempbar){
    _data[id].leavelat.append(tempbar.at(4).toDouble());
}
inline void add5(Dataset *_data,int id,QStringList tempbar){
    _data[id].Endltt.append(tempbar.at(5).toDouble());
}
inline void add6(Dataset *_data,int id,QStringList tempbar){
    _data[id].Endlat.append(tempbar.at(6).toDouble());
}
inline void add7(Dataset *_data,int id,QStringList tempbar){
    _data[id].fee.append(tempbar.at(7).toDouble());
}

void (*add[7])(Dataset *_data,int id,QStringList tempbar);

DataLoader::DataLoader(QObject *parent, QString filePath, QString _startTime, QString _endTime,
                       QVector<int> _fields) : path(filePath), startTime(_startTime), endTime(_endTime), fields(_fields)
{

}

DataLoader::~DataLoader()
{

}

void DataLoader::startLoading(){
    Dataset *_data;
    _data = new Dataset[100];

    int i = (startTime.toInt() - 20161101)*5;
    int j = (endTime.toInt() - 20161101)*5;

    int size = fields.size();

    int n=0;
//add functions into the pointer array
    for(int m=0;m<size;++m)
    {
       switch(fields[m]){
       case 1 :
           add[n++] = add1;
           break;
       case 2 :
           add[n++] = add2;
           break;
       case 3 :
           add[n++] = add3;
           break;
       case 4 :
           add[n++] = add4;
           break;
       case 5 :
           add[n++] = add5;
           break;
       case 6 :
           add[n++] = add6;
           break;
       case 7:
           add[n++] = add7;
       }
    }
    //read in datasets
    const QString DATASET_PATH = QString("%1/dataset/").arg(path);
    QDir directory(DATASET_PATH);
    QStringList _datasets = directory.entryList(QStringList()<<"*.csv"<<"*.CSV",QDir::Files);
    QStringList datasets;

    for(int m=i;m<j+5;++m){
        QString dataset = _datasets[m];
        datasets << dataset;
    }

    int id;
    int cnt = 0;
    int percent = 0;
    qint64 init = 1477929600;

    QDateTime start,end;
    QString _start,_end;

    //try open file
    for(QString filename : datasets) {
        percent = int(double(++cnt) / datasets.length() * 100);
        emit progress(percent);
        QFile file(QString("%1%2").arg(DATASET_PATH).arg(filename));
        if(!file.open(QIODevice::ReadOnly)) QMessageBox::warning(nullptr, "Warning","Open file failed!");

        //load and parse file
        QTextStream in(&file);
        QStringList tempOption = in.readAll().split("\n");

        for(int i = 1;i < tempOption.count() - 1; i++){
            QStringList tempbar = tempOption.at(i).split(",");

            //remove orders out of the area
            if(tempbar.at(3).toDouble() < 103.908407 || tempbar.at(3).toDouble() > 104.2220445
                    || tempbar.at(4).toDouble() < 30.52408 || tempbar.at(4).toDouble() > 30.79388) continue;

            //compute grid for mapping
            id = int((tempbar.at(4).toDouble() - 30.52408)/0.0269796)*10 + int((tempbar.at(3).toDouble() - 103.9084)/0.0313637);

            //electively load data
            for(int j=0;j<size;++j)
            {
                add[j](_data,id,tempbar);
            }

            //Mapping start time
            _data[id].startID[(tempbar.at(1).toLongLong()-init)/60].append(_data[id].Leaveltt.size()-1);

            //Mapping end time
            _data[id].endID[(tempbar.at(2).toLongLong()-init)/60].append(_data[id].Leaveltt.size()-1);
        }
    }

    //move dataset to the main thread
    _data->moveToThread(QApplication::instance()->thread());
    emit resultReady(_data);
}
