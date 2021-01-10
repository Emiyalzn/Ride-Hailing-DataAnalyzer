
//
//Created on 2020-12-22 to shrink Mainwinow.Cpp
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

//set radio buttons based on the chart request
void MainWindow::handelOrder(){
    ui->dailyButton->setEnabled(true);
    ui->hourButton->setEnabled(true);
    ui->minButton->setEnabled(true);
    ui->stepSlider->setEnabled(true);
}

void MainWindow::handelTravelTime(){
    ui->dailyButton->setEnabled(false);
    ui->hourButton->setEnabled(false);
    ui->minButton->setEnabled(false);
    ui->stepSlider->setEnabled(false);
}

void MainWindow::startDrawing()
{
    if(grids.isEmpty()) {QMessageBox::warning(this,"Select Grids","Please Select Grids first!"); return;}

    qint64 _start = startTime.toSecsSinceEpoch();
    qint64 _end = endTime.toSecsSinceEpoch();

    if((_end - _start) / (60 * timeStep) < 3) {QMessageBox::warning(this,"Too Large Step","The time step is too large"); return;}

    m_chartThread = new QThread;

    dc = new DrawChart(this,fields);

    //do chart connections
    connect(m_chartThread,&QThread::finished,dc,&QObject::deleteLater);
    connect(this,&MainWindow::drawOrderChartRequest,dc,&DrawChart::drawOrderChart);
    connect(dc,&DrawChart::orderChartReady,this,&MainWindow::handelOrderChartResult);
    connect(this,&MainWindow::drawTravelTimeChartRequest,dc,&DrawChart::drawTravelTimeChart);
    connect(dc,&DrawChart::travelTimeChartReady,this,&MainWindow::handelTravelTimeChartResult);
    connect(this,&MainWindow::drawFeeChartRequest,dc,&DrawChart::drawFeeChart);
    connect(dc,&DrawChart::feeChartReady,this,&MainWindow::handelFeeChartResult);
    connect(this,&MainWindow::drawRevenueChartRequest,dc,&DrawChart::drawRevenueChart);
    connect(dc,&DrawChart::revenueChartReady,this,&MainWindow::handelRevenueChartResult);

    //move handeler to the child thread
    dc->moveToThread(m_chartThread);
    m_chartThread->start();

    QVariant dataVar;
    dataVar.setValue(grids);

    //emit signals based on the radio button
    if(ui->orderButton->isChecked()) emit drawOrderChartRequest(startTime,endTime,timeStep,dataVar,data);
    else if(ui->timeButton->isChecked()) emit drawTravelTimeChartRequest(startTime,endTime,dataVar,data);
    else if(ui->feesButton->isChecked()) emit drawFeeChartRequest(startTime,endTime,dataVar,data);
    else emit drawRevenueChartRequest(startTime,endTime,timeStep,dataVar,data);

    ui->queryButton->setEnabled(false);
}

void MainWindow::handelOrderChartResult(QList<QPointF> _data1,QList<QPointF> _data2, int max)
{
    //delete former chartView
    QLayoutItem *child;
    while ((child = ui->frameChart->layout()->takeAt(0)) != 0)
    {
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }
            delete child;
    }

    QSplineSeries *series_1 = new QSplineSeries(this);
    QSplineSeries *series_2 = new QSplineSeries(this);

    series_1->replace(_data1);
    series_2->replace(_data2);

    series_1->setName("Leave");
    series_2->setName("Enter");

    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    //add series
    if(fields[0] == 1) chart->addSeries(series_1);
    if(fields[0] == 2 || fields[1] == 2) chart->addSeries(series_2);

    //visualize chart
    //Set Axis
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTitleFont(QFont("Myriad Pro"));
    axisX->setLabelsFont(QFont("Myriad Pro"));
    axisX->setTickCount(12);
    axisX->setMin(startTime);
    axisX->setMax(endTime);
    axisX->setFormat("MM-dd HH:mm");
    axisX->setLabelsAngle(-90);
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelsFont(QFont("Myriad Pro"));
    axisY->setTitleFont(QFont("Myriad Pro"));
    axisY->setLabelFormat("%i");
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setTitleText("Number");
    axisY->setRange(0,max*1.1);//avoid out of range

    //Chart settings
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);
    chart->legend()->setFont(QFont("Myriad Pro"));

    chart->setTitle("Order Analysis");
    chart->setTitleFont(QFont("Myriad Pro"));

    chartView = new ChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    series_1->attachAxis(axisX);
    series_2->attachAxis(axisX);

    if(fields[0] == 1) {series_1->attachAxis(axisY);}
    if(fields[0] == 2 || fields[1] == 2) {series_2->attachAxis(axisY);}

    ui->frameChart->layout()->addWidget(chartView);

    ui->queryButton->setEnabled(true);
    m_chartThread->terminate();

    return;
}

void MainWindow::handelTravelTimeChartResult(QMap<int,int> _data, int max){
    //delete former chartView
    QLayoutItem *child;
    while ((child = ui->frameChart->layout()->takeAt(0)) != 0)
    {
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }
            delete child;
    }

    QChart *chart = new QChart();
    chart->setTitle("Travel Time Analysis");
    chart->setTitleFont(QFont("Myriad Pro"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarSet *set = new QBarSet("Travel Time");

    for(int i = 0;i <= 90 && i < _data.size(); ++i){
        //interpolating
        if(!_data[i]) *set<<0;
        else {*set << _data[i];}
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);
    chart->addSeries(series);

    //chart settings
    QValueAxis * axisX = new QValueAxis();
    axisX->setTickCount(10);
    axisX->setLabelsAngle(-90);
    axisX->setTitleText("Time");
    axisX->setTitleFont(QFont("Myriad Pro"));
    axisX->setLabelsFont(QFont("Myriad Pro"));
    chart->addAxis(axisX,Qt::AlignBottom);
    axisX->setRange(0,90);
    axisX->setLabelFormat("%i");

    QValueAxis * axisY = new QValueAxis();
    axisY->setLabelsFont(QFont("Myriad Pro"));
    axisY->setTitleFont(QFont("Myriad Pro"));
    axisY->setLabelFormat("%i");
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setTitleText("Number");
    axisY->setRange(0,max*1.1);//avoid out of range

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    ChartView *chartView = new ChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->frameChart->layout()->addWidget(chartView);
    ui->queryButton->setEnabled(true);
    m_chartThread->terminate();

    return;
}

void MainWindow::handelFeeChartResult(QMap<int,int> _data, int max){
    //delete former chartView
    QLayoutItem *child;
    while ((child = ui->frameChart->layout()->takeAt(0)) != 0)
    {
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }
            delete child;
    }

    QChart *chart = new QChart();
    chart->setTitle("Fee Analysis");
    chart->setTitleFont(QFont("Myriad Pro"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarSet *set = new QBarSet("Fee");
    QStringList categories;
    QString label;

    for(int i = 10;i <= 90 && i < _data.size(); ++i){
        //interpolating
        if(!_data[i]) *set << 0;
        else *set << _data[i];
        label = QString::number(i);
        categories << label;
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);
    chart->addSeries(series);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    axis->setVisible(false);
    chart->addAxis(axis,Qt::AlignBottom);

    //visualize chart
    //Set Axis
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleFont(QFont("Myriad Pro"));
    axisX->setLabelsFont(QFont("Myriad Pro"));
    axisX->setTickCount(10);
    axisX->setRange(1,9);
    axisX->setLabelsAngle(-90);
    axisX->setTitleText("Fee");
    chart->addAxis(axisX, Qt::AlignBottom);
    axisX->setLabelFormat("%.1f");

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsFont(QFont("Myriad Pro"));
    axisY->setTitleFont(QFont("Myriad Pro"));
    axisY->setLabelFormat("%i");
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setTitleText("Number");
    axisY->setRange(0,max*1.1);//avoid out of range

    series->attachAxis(axis);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->frameChart->layout()->addWidget(chartView);
    ui->queryButton->setEnabled(true);
    m_chartThread->terminate();

    return;
}

void MainWindow::handelRevenueChartResult(QList<QPointF> _data, int max){
    //delete former chartView
    QLayoutItem *child;
    while ((child = ui->frameChart->layout()->takeAt(0)) != 0)
    {
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }
            delete child;
    }

    QChart *chart = new QChart();
    chart->setTitle("Revenue Analysis");
    chart->setTitleFont(QFont("Myriad Pro"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QLineSeries *series = new QLineSeries(chart);
    series->replace(_data);

    chart->addSeries(series);

    //visualize chart
    //Set Axis
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTitleFont(QFont("Myriad Pro"));
    axisX->setLabelsFont(QFont("Myriad Pro"));
    axisX->setTickCount(12);
    axisX->setMin(startTime);
    axisX->setMax(endTime);
    axisX->setFormat("MM-dd HH:mm");
    axisX->setLabelsAngle(-90);
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelsFont(QFont("Myriad Pro"));
    axisY->setTitleFont(QFont("Myriad Pro"));
    axisY->setLabelFormat("%i");
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setTitleText("Revenue");
    axisY->setRange(0,max*1.1);//avoid out of range

    //Chart settings
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignTop);
    chart->legend()->setFont(QFont("Myriad Pro"));

    ChartView *chartView = new ChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    ui->frameChart->layout()->addWidget(chartView);
    ui->queryButton->setEnabled(true);
    m_chartThread->terminate();

    return;
}

//Predict time from different models
void MainWindow::handelWithPredict()
{
    if(Origin.first == 0 || Destination.first == 0){QMessageBox::warning(this,"Wait","Please input places first!"); return;}
    int leave = leaveTime.toSecsSinceEpoch();
    emit accessForAverageTime(Origin,Destination,(leave-1477929600)/60,data);
    emit accessForNNTime(Origin.first,Origin.second,Destination.first,Destination.second,weekday,leave);
}

//push average time result into tableWidget
void MainWindow::handelWithAverageTime(int time)
{
    AveragePredictTime = time;
    if(time == 0){
        auto timeTip = new QTableWidgetItem(QString("Avg Predict Time: None"));
        ui->routeTable->insertRow(0);
        ui->routeTable->setItem(0,0,timeTip);
    }
    else{
        auto timeTip =  new QTableWidgetItem(QString("Avg Predict Time: %1min%2s")
                                               .arg(QString::number(time/60)).arg(QString::number(time%60)));
        ui->routeTable->insertRow(0);
        ui->routeTable->setItem(0,0,timeTip);
    }
}

//push NN time result into tableWidget
void MainWindow::handelWithNNTime(int time)
{
    NNPredictTime = time;
    auto timeTip = new QTableWidgetItem(QString("NN Predict Time: %1min%2s")
                                        .arg(QString::number(time/60)).arg(QString::number(time%60)));
    ui->routeTable->insertRow(0);
    ui->routeTable->setItem(0,0,timeTip);
}
