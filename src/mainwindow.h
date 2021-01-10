#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QDateTime>
#include <QMap>
#include <QtCharts>
#include <QPainter>
#include <QPoint>
#include <QMetaType>
#include <QVariant>
#include <QDebug>
#include <QFileDialog>
#include <QtCharts>
#include <QDateTimeAxis>
#include <QFont>

#include "dataloader.h"
#include "dataset.h"
#include "selectgrid.h"
#include "drawchart.h"
#include "networkmanager.h"
#include "thermaldiagram.h"
#include "chartview.h"
#include "timepredictor.h"
#include "nn.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    //load-related signals
    void startLoadRequest();
    void cancel();

    //chart-related signals
    void drawOrderChartRequest(QDateTime startTime, QDateTime endTime,
                          int timeStep, QVariant, Dataset * data);
    void drawTravelTimeChartRequest(QDateTime startTime, QDateTime endTime,
                             QVariant, Dataset * data);
    void drawFeeChartRequest(QDateTime startTime, QDateTime endTime,
                             QVariant, Dataset * data);
    void drawRevenueChartRequest(QDateTime startTime, QDateTime endTime,
                                 int timeStep, QVariant, Dataset *data);

    //map-related signals
    void accessForMap(QString beginltt, QString beginlat, QString endltt, QString endlat);
    void accessForRoute(QString beginltt, QString beginlat, QString endltt, QString endlat);
    void accessForThermalDiagram(double beginltt, double beginlat, double endltt,
                                 double endlat, QVariant, Dataset * data, int startTime,
                                 int timeStep, QPixmap sourceMap);
    void accessForAverageTime(QPair<double,double> start,QPair<double,double> end,int startTime,Dataset * data);
    void accessForNNTime(double beginltt,double beginlat,double endltt,double endlat,
                         int weekday, int startTime);

public slots:
    //move-to-thread slots
    void startLoading();
    void startDrawing();

    //result-handeling slots
    void handelWithThermalButton();
    void handelLoadResult(Dataset *_data);
    void handelOrderChartResult(QList<QPointF> _data1,QList<QPointF> _data2, int max);
    void handelTravelTimeChartResult(QMap<int,int> _data, int max);
    void handelFeeChartResult(QMap<int,int> _data, int max);
    void handelRevenueChartResult(QList<QPointF> _data, int max);
    void handelOrder();
    void handelTravelTime();
    void handelHeatPixMap(QPixmap _mpixmap);
    void handelHeatMap(QPixmap heatMap);
    void handelRoutePixMap(QPixmap _mpixmap);
    void handelWithTips(QStringList tips,QVector<QPair<double,double>> locations);
    void handelWithInstructions(QStringList instructions, int time);
    void handelWithRoute();
    void handelWithPredict();
    void handelWithAverageTime(int time);
    void handelWithNNTime(int time);
    void handelWithWeekday(int id);

    //Location select slots
    void selectStartLocation();
    void selectEndLocation();

    //slots connected to buttons
    void beginSelect();
    void dealWithGrids();
    void dealWithCancel();
    void dealWithWizard(QString path, QString startTime, QString endTime, QVector<int> fields);

    //related-set slots
    void setStartTime(QDateTime _start);
    void setLeaveTime(QDateTime _start);
    void setEndTime(QDateTime _end);
    void setTDStartTime(QDateTime _start);
    void setSliderDaily();
    void setSliderHourly();
    void setSliderMinly();
    void setTimeStep(int step);
    void setTDTimeStep(int id);
    void setTDSliderDately(QDateTime start);
    void setTDTimeEditSliderly(int value);

private:
    //Pointers to construct handlers
    Ui::MainWindow *ui;
    QThread * m_loadThread = new QThread;
    QThread * m_chartThread;
    DataLoader *dl;
    DrawChart *dc;
    SelectGrid *select;
    NetworkManager *manager = new NetworkManager(this);
    ThermalDiagram *td = new ThermalDiagram(this);
    Dataset *data = nullptr;
    TimePredictor *tp = new TimePredictor(this);
    NN *nn = new NN(this);
    ChartView * chartView;
    QCompleter *com = new QCompleter();
    QStringListModel *slm = new QStringListModel();

    //necessary members
    int APIPredictTime;
    int AveragePredictTime;
    int NNPredictTime;
    int weekday = 0;
    int timeStep = 1;
    int _step = 1;
    int TDtimeStep = 30;
    QPixmap mPixMap;
    QVector<QPair<double,double>> locations;
    QPair<double,double> Origin={0,0};
    QPair<double,double> Destination={0,0};
    QStringList tips;
    QString startTimeChosen;
    QString endTimeChosen;
    QString path;
    QPoint corner;
    QPoint begin,end;
    QVector<int> grids;
    QVector<int> fields;
    QDateTime startTime = QDateTime::fromString(QString("2016-11-01 08:00"), "yyyy-MM-dd hh:mm");
    QDateTime endTime = QDateTime::fromString(QString("2016-11-01 09:00"),"yyyy-MM-dd hh:mm");
    QDateTime TDstartTime = QDateTime::fromString(QString("2016-11-01 08:00"), "yyyy-MM-dd hh:mm");
    QDateTime leaveTime = QDateTime::fromString(QString("2016-11-01 08:00"), "yyyy-MM-dd hh:mm");
};
#endif // MAINWINDOW_H
