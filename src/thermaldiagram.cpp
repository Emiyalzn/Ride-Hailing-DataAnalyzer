//
//Created on 2020/12/11
//
#include "thermaldiagram.h"

ThermalDiagram::ThermalDiagram(QWidget *parent)
{

}

ThermalDiagram::~ThermalDiagram()
{

}

void ThermalDiagram::counting(double beginltt, double beginlat, double endltt,
                              double endlat, QVariant dataVar, Dataset * data, int startTime,
                              int timeStep, QPixmap sourceMap)
{
    //Distributing points to Pixels
    Counter.clear();
    QVector<int> grids = dataVar.value<QVector<int>>();

    double tmp;
    int x,y;
    QPair<int,int> position;
    _maxCount /= 2;

    if(beginltt > endltt) { tmp = endltt; endltt = beginltt; beginltt = tmp;}
    if(beginlat < endlat) { tmp = endlat; endlat = beginlat; beginlat = tmp;}

    //count order number in each pixel
    for(int i = 0; i < grids.size();++i){
        for(int j = 0; j < timeStep; ++j)
            for(int m = 0; m <data[grids[i]].startID[startTime + j].size();++m)
        if( data[grids[i]].Leaveltt[data[grids[i]].startID[startTime + j][m]] < endltt
         && data[grids[i]].Leaveltt[data[grids[i]].startID[startTime + j][m]] > beginltt
         && data[grids[i]].leavelat[data[grids[i]].startID[startTime + j][m]] > endlat
         && data[grids[i]].leavelat[data[grids[i]].startID[startTime + j][m]] < beginlat)
        {
            x = int((data[grids[i]].Leaveltt[data[grids[i]].startID[startTime + j][m]] - 104.065226) / 0.000690749) + 512;
            y = 355 - int((data[grids[i]].leavelat[data[grids[i]].startID[startTime + j][m]] - 30.65898) / 0.000589836);
            position.first = x; position.second = y;
            if(!Counter[position]) Counter[position] = 1;
            else{
               if(++Counter[position] > _maxCount) _maxCount = Counter[position];
            }
        }
    }
    drawMap(sourceMap);
}

void ThermalDiagram::drawMap(QPixmap sourceMap)
{
    _dataImg =  new QImage(1024,800,QImage::Format_Alpha8);
    _dataImg->fill(Qt::transparent);

    _heatImg = new QImage(1024,800,QImage::Format_ARGB32);

    if(!flag){
    //set color list by linear gradient
    QLinearGradient linear = QLinearGradient(QPoint(0,0),QPoint(255,0));
    linear.setColorAt(0, Qt::blue);
    linear.setColorAt(0.4, Qt::blue);
    linear.setColorAt(0.5, Qt::cyan);
    linear.setColorAt(0.6, Qt::green);
    linear.setColorAt(0.8, Qt::yellow);
    linear.setColorAt(0.95, Qt::red);

    //paint Color to the img for color selecting
    QImage img(256,1,QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.fillRect(img.rect(),linear);

    int heatAlpha = 200;
    quint32 alpha = 0;

    //Setting colorlist to argb
    for(quint32 i = 0;i<256;i++){
        alpha = heatAlpha/255.0*i;
        _colorList[i] = img.pixel(i,0)&0x00FFFFFF|(alpha<<24);
    }
    }

    flag = true;

    QPainter dataPainter(&*_dataImg);
    dataPainter.setPen(Qt::transparent);
    //Counting weighting and draw transparent image
    QMap<QPair<int,int>,int>::iterator iter = Counter.begin();
    while(iter != Counter.end()){
        uchar alpha = uchar(iter.value()/(double)_maxCount*255);
        //set radius to 1
        QRadialGradient gradient(iter.key().first,iter.key().second,8);
        gradient.setColorAt(0,QColor(0,0,0,alpha));
        gradient.setColorAt(1,QColor(0,0,0,0));
        dataPainter.setBrush(gradient);
        dataPainter.drawEllipse(QPointF(iter.key().first,iter.key().second),8,8);
        iter++;
    }

    //drawHeatImg
    for(int row = 0;row < _dataImg->height();row++)
    {
        uchar *line_data = _dataImg->scanLine(row);
        QRgb *line_heat = reinterpret_cast<QRgb*>(_heatImg->scanLine(row));
        for(int col = 0;col<_dataImg->width();col++)
        {
            //Selecting color from the color list
            line_heat[col] = _colorList[line_data[col]];
        }
    }

    QImage *sourceImg = new QImage(sourceMap.toImage());
    QPainter imagePainter(sourceImg);
    imagePainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    imagePainter.drawImage(0,0,*_heatImg);
    QPixmap heatMap = QPixmap::fromImage(*sourceImg);
    emit finishedImage(heatMap);
}
