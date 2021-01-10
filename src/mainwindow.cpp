#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Window settings
    setWindowTitle("Ride-Hailing Data Analyser");
    setWindowIcon(QIcon(":/My/figures/Taxi_2.ico"));

    //register meta type to emit signals
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<QMap<int,int>>("QMap<int,int>");

    //set radio button groups
    QButtonGroup *timeStepButtonGroup = new QButtonGroup(this);
    QButtonGroup *chartButtonGroup = new QButtonGroup(this);
    timeStepButtonGroup->addButton(ui->minButton);
    timeStepButtonGroup->addButton(ui->hourButton);
    timeStepButtonGroup->addButton(ui->dailyButton);
    chartButtonGroup->addButton(ui->orderButton);
    chartButtonGroup->addButton(ui->feesButton);
    chartButtonGroup->addButton(ui->timeButton);
    chartButtonGroup->addButton(ui->revenueButton);

    //initialize
    ui->minButton->setChecked(true);
    ui->orderButton->setChecked(true);
    ui->tabWidget->setStyleSheet("QTabWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }");
    com->setModel(slm);
    com->setCaseSensitivity(Qt::CaseInsensitive);
    ui->routeFromEdit->setCompleter(com);
    ui->routeToEdit->setCompleter(com);

    //Connection
    connect(ui->orderButton,&QRadioButton::clicked,this,&MainWindow::handelOrder);
    connect(ui->timeButton,&QRadioButton::clicked,this,&MainWindow::handelTravelTime);
    connect(ui->feesButton,&QRadioButton::clicked,this,&MainWindow::handelTravelTime);
    connect(ui->revenueButton,&QRadioButton::clicked,this,&MainWindow::handelOrder);
    connect(ui->startButton,&QPushButton::clicked,this,&MainWindow::startLoading);
    connect(ui->queryButton,&QPushButton::clicked,this,&MainWindow::startDrawing);
    connect(ui->startTime,&QDateTimeEdit::dateTimeChanged,this,&MainWindow::setStartTime);
    connect(ui->timeEditDeparture,&QDateTimeEdit::dateTimeChanged,this,&MainWindow::setLeaveTime);
    connect(ui->endTime,&QDateTimeEdit::dateTimeChanged,this,&MainWindow::setEndTime);
    connect(ui->startTimeEdit,&QDateTimeEdit::dateTimeChanged,this,&MainWindow::setTDStartTime);
    connect(ui->startTimeEdit,&QDateTimeEdit::dateTimeChanged,this,&MainWindow::setTDSliderDately);
    connect(ui->timeSlider,&QSlider::valueChanged,this,&MainWindow::setTDTimeEditSliderly);
    connect(ui->comboBoxWeekday,SIGNAL(currentIndexChanged(int)),this,SLOT(handelWithWeekday(int)));
    connect(ui->dailyButton,&QRadioButton::clicked,this,&MainWindow::setSliderDaily);
    connect(ui->hourButton,&QRadioButton::clicked,this,&MainWindow::setSliderHourly);
    connect(ui->minButton,&QRadioButton::clicked,this,&MainWindow::setSliderMinly);
    connect(ui->stepSlider,&QSlider::valueChanged,this,&MainWindow::setTimeStep);
    connect(ui->stepComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setTDTimeStep(int)));
    connect(ui->stepSlider,&QSlider::valueChanged,ui->timeStepBox,&QSpinBox::setValue);
    connect(ui->timeStepBox,SIGNAL(valueChanged(int)),ui->stepSlider,SLOT(setValue(int)));
    connect(ui->selectButton,&QPushButton::clicked,this,&MainWindow::beginSelect);
    connect(ui->startTime,&QDateTimeEdit::dateTimeChanged,ui->endTime,&QDateTimeEdit::setMinimumDateTime);
    connect(ui->endTime,&QDateTimeEdit::dateTimeChanged,ui->startTime,&QDateTimeEdit::setMaximumDateTime);
    connect(ui->cancelButton,&QPushButton::clicked,this,&MainWindow::dealWithCancel);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::handelWithThermalButton);
    connect(ui->routeFromEdit,&QLineEdit::textChanged,manager,&NetworkManager::on_Line_Edit);
    connect(ui->routeFromEdit,&QLineEdit::editingFinished,this,&MainWindow::selectStartLocation);
    connect(ui->routeToEdit,&QLineEdit::textChanged,manager,&NetworkManager::on_Line_Edit);
    connect(ui->routeToEdit,&QLineEdit::editingFinished,this,&MainWindow::selectEndLocation);
    connect(ui->pushButtonRoutePlanning,&QPushButton::clicked,this,&MainWindow::handelWithRoute);
    connect(ui->estimateTimeButton,&QPushButton::clicked,this,&MainWindow::handelWithPredict);
    connect(this,&MainWindow::accessForRoute,manager,&NetworkManager::on_Routebutton_clicked);
    connect(manager,&NetworkManager::routePixMapReady,this,&MainWindow::handelRoutePixMap);
    connect(manager,&NetworkManager::tipsReady,this,&MainWindow::handelWithTips);
    connect(manager,&NetworkManager::instructionsReady,this,&MainWindow::handelWithInstructions);
    connect(tp,&TimePredictor::averageTimeFinished,this,&MainWindow::handelWithAverageTime);
    connect(nn,&NN::NNResultReady,this,&MainWindow::handelWithNNTime);
    connect(this,&MainWindow::accessForAverageTime,tp,&TimePredictor::estimateAverageTime);
    connect(this,&MainWindow::accessForNNTime,nn,&NN::transform);
    connect(ui->routeTable,SIGNAL(itemChanged(QTableWidgetItem*)),ui->routeTable,SLOT(resizeRowsToContents()));
}

void MainWindow::dealWithWizard(QString _path, QString _startTime, QString _endTime, QVector<int> _fields)
{
    //initialize from the wizard
    path = _path;
    startTimeChosen = _startTime;
    endTimeChosen = _endTime;
    fields = _fields;
    QDateTime start = QDateTime::fromString(startTimeChosen,"yyyyMMdd");
    QDateTime end = QDateTime::fromString(endTimeChosen+" 23:59","yyyyMMdd hh:mm");
    ui->startTime->setMinimumDateTime(start);
    ui->endTime->setMaximumDateTime(end);

    int Fields[8] = {0};
    for(int m=0;m<fields.size();++m){
        Fields[fields[m]] = 1;
    }

    //set radio buttons
    if(!(Fields[1]&&Fields[2])) ui->timeButton->setEnabled(false);
    if(!Fields[7]) {ui->feesButton->setEnabled(false); ui->revenueButton->setEnabled(false);}
}

//Set DateTimes
void MainWindow::setStartTime(QDateTime _start)
{
    startTime = _start;
}

void MainWindow::setEndTime(QDateTime _end)
{
    endTime = _end;
}

void MainWindow::setTDStartTime(QDateTime _start)
{
    TDstartTime = _start;
}

void MainWindow::setLeaveTime(QDateTime _start)
{
    leaveTime = _start;
}

void MainWindow::setTimeStep(int step)
{
    timeStep = step * _step;
}

//set TimeStep based on the combo box
void MainWindow::setTDTimeStep(int id)
{
    switch(id){
    case 0:
        TDtimeStep = 1;
        ui->timeSlider->setSingleStep(1);
        break;
    case 1:
        TDtimeStep = 5;
        ui->timeSlider->setSingleStep(5);
        break;
    case 2 :
        TDtimeStep = 10;
        ui->timeSlider->setSingleStep(10);
        break;
    case 3:
        TDtimeStep = 30;
        ui->timeSlider->setSingleStep(30);
        break;
    case 4:
        TDtimeStep = 60;
        ui->timeSlider->setSingleStep(60);
    }
}

//Set sliders
void MainWindow::setTDSliderDately(QDateTime start)
{
    int begin = (start.toSecsSinceEpoch() - 1477929600) / 60;
    ui->timeSlider->setValue(begin + 1);
}

void MainWindow::setTDTimeEditSliderly(int value)
{
    qint64 start = (value - 1)*60 + 1477929600;
    QDateTime startTime = QDateTime::fromSecsSinceEpoch(start);
    ui->startTimeEdit->setDateTime(startTime);
    handelWithThermalButton();
}

void MainWindow::setSliderDaily()
{
    ui->stepSlider->setValue(1);
    ui->timeStepBox->setValue(1);
    ui->stepSlider->setEnabled(false);
    ui->timeStepBox->setEnabled(false);
    timeStep = 24 * 60;
}

void MainWindow::setSliderHourly()
{
    ui->stepSlider->setEnabled(true);
    ui->timeStepBox->setEnabled(true);
    ui->stepSlider->setRange(1,24);
    ui->timeStepBox->setRange(1,24);
    ui->stepSlider->setValue(1);
    ui->timeStepBox->setValue(1);
    _step = 60;
    timeStep = 60;
}

void MainWindow::setSliderMinly()
{
    ui->stepSlider->setEnabled(true);
    ui->timeStepBox->setEnabled(true);
    ui->stepSlider->setRange(1,60);
    ui->timeStepBox->setRange(1,60);
    ui->stepSlider->setValue(1);
    ui->timeStepBox->setValue(1);
    _step = 1;
    timeStep = 1;
}

void MainWindow::startLoading()
{
    //Load Data
    m_loadThread = new QThread;
    dl = new DataLoader(this,path,startTimeChosen,endTimeChosen,fields);

    //do connections
    connect(this,&MainWindow::startLoadRequest,dl,&DataLoader::startLoading);
    connect(dl,&DataLoader::progress,ui->progressBar,&QProgressBar::setValue);
    connect(m_loadThread,&QThread::finished,dl,&QObject::deleteLater);
    connect(dl,&DataLoader::resultReady,this,&MainWindow::handelLoadResult);

    //move handeler to the child thread
    dl->moveToThread(m_loadThread);
    m_loadThread->start();

    emit startLoadRequest();
    ui->startButton->setEnabled(false);
    ui->estimateTimeButton->setEnabled(false);
    ui->progressBar->setTextVisible(true);
}

void MainWindow::handelLoadResult(Dataset *_data){
    ui->progressBar->setTextVisible(false);
    if(data) delete data;
    data = _data;
    ui->queryButton->setEnabled(true);
    ui->estimateTimeButton->setEnabled(true);
}

void MainWindow::dealWithCancel()
{
    ui->progressBar->setValue(0);
    ui->startButton->setEnabled(true);
    m_loadThread->terminate();
    emit cancel();
}

//get navigation information by API
void MainWindow::handelWithRoute()
{
    QString beginltt = QString::number(Origin.first);
    QString beginlat = QString::number(Origin.second);
    QString endltt = QString::number(Destination.first);
    QString endlat = QString::number(Destination.second);
    emit accessForRoute(beginltt,beginlat,endltt,endlat);
}

void MainWindow::beginSelect()
{
    ui->pushButton->setEnabled(false);
    ui->centralWidget->setMouseTracking(true);
    this->setMouseTracking(true);
    grids.clear();
    select = new SelectGrid();
    connect(this,&MainWindow::accessForThermalDiagram,td,&ThermalDiagram::counting);
    connect(select,&SelectGrid::completeSelection,this,&MainWindow::dealWithGrids);
    connect(this,&MainWindow::accessForMap,manager,&NetworkManager::on_Mapbutton_clicked);
    connect(manager,&NetworkManager::heatPixMapReady,this,&MainWindow::handelHeatPixMap);
    select->show();
}

//compute selected grid ids
void MainWindow::dealWithGrids()
{
    begin = this->mapFromGlobal(select->m_beginPoint);
    end = this->mapFromGlobal(select->m_endPoint);
    grids.clear();

    //avoid outof range
    if(begin.x() > 626) begin.setX(606);else if(begin.x() < 116) begin.setX(136);
    if(begin.y() > 738) begin.setY(718);else if(begin.y() < 228) begin.setY(248);
    if(end.x() > 626) end.setX(606);else if(end.x() < 116) end.setX(136);
    if(end.y() > 738) end.setY(718);else if(end.y() < 228) end.setY(248);

    double beginltt = 103.9084 + double((begin.x() - 116))/51 * 0.0313637;
    double endltt = 103.9084 + double((end.x() - 116))/51 * 0.0313637;
    double beginlat = 30.5241 + double((738 - begin.y()))/51 * 0.02698;
    double endlat = 30.5241 + double((738 - end.y()))/51 * 0.02698;
    QString beginlttS = QString::number(beginltt);
    QString endlttS = QString::number(endltt);
    QString beginlatS = QString::number(beginlat);
    QString endlatS = QString::number(endlat);

    emit accessForMap(beginlttS,beginlatS,endlttS,endlatS);

    int startId = (begin.x() - 116)/51 + (738 - begin.y())/51*10;
    int endId = (end.x() - 116)/51 + (738 - end.y())/51*10;
    int starts = startId/10, ends = endId/10;
    int startg = startId%10, endg = endId%10;
    int tmp;
    if(starts > ends){ tmp = ends; ends = starts; starts = tmp;}
    if(startg > endg){ tmp = endg; endg = startg; startg = tmp;}
    //push grids
    for(int i = starts;i <= ends;++i)
        for(int j = startg;j <= endg;++j)
            grids.append(i*10 + j);
}

void MainWindow::handelHeatPixMap(QPixmap _mpixmap)
{
    mPixMap = _mpixmap;
    ui->mapLabel->setPixmap(_mpixmap);
    ui->pushButton->setEnabled(true);
}

void MainWindow::handelHeatMap(QPixmap heatMap)
{
    ui->mapLabel->setPixmap(heatMap);
}

void MainWindow::handelRoutePixMap(QPixmap _mpixmap)
{
    ui->routeMapLabel->setPixmap(_mpixmap);
}

void MainWindow::handelWithThermalButton()
{
    if(ui->progressBar->value() != 100) {QMessageBox::warning(this,"Wait","Please Load Data first!"); return;}
    if(grids.isEmpty()) {QMessageBox::warning(this,"Select","Please Select Grids first!"); return;}
    double beginltt = 103.9084 + double((begin.x() - 116))/51 * 0.0313637;
    double endltt = 103.9084 + double((end.x() - 116))/51 * 0.0313637;
    double beginlat = 30.5241 + double((738 - begin.y()))/51 * 0.02698;
    double endlat = 30.5241 + double((738 - end.y()))/51 * 0.02698;
    QVariant dataVar;
    dataVar.setValue(grids);
    int startTD = (TDstartTime.toSecsSinceEpoch() -  1477929600) / 60;
    connect(td,&ThermalDiagram::finishedImage,this,&MainWindow::handelHeatMap);
    emit accessForThermalDiagram(beginltt,beginlat,endltt,endlat,dataVar,data,startTD,TDtimeStep,mPixMap);
}

//push tips we get from API into the completer
void MainWindow::handelWithTips(QStringList tip,QVector<QPair<double,double>> location)
{
    tips = tip;
    slm->setStringList(tip);
    locations = location;
}

void MainWindow::selectStartLocation()
{
    if(!tips.contains(ui->routeFromEdit->text())) return;
    else{
        int id = tips.indexOf(ui->routeFromEdit->text());
        Origin = locations[id];
    }
    if(!ui->routeToEdit->text().isEmpty()){
        ui->pushButtonRoutePlanning->setEnabled(true);
    }
}

void MainWindow::selectEndLocation()
{
    if(!tips.contains(ui->routeToEdit->text())) return;
    else{
        int id = tips.indexOf(ui->routeToEdit->text());
        Destination = locations[id];
    }
    if(!ui->routeFromEdit->text().isEmpty()){
        ui->pushButtonRoutePlanning->setEnabled(true);
    }
}

//push navigation instructions into the tableWidget
void MainWindow::handelWithInstructions(QStringList _instructions,int time)
{
    QList<QTableWidgetItem *> instructions_list;

    for(int i = 0; i < _instructions.length();++i)
    {
        auto instruction = new QTableWidgetItem(_instructions[i]);
        instructions_list << instruction;
    }

    APIPredictTime = time;
    ui->routeTable->clear();

    //Push the instructions to the table
    ui->routeTable->setRowCount(_instructions.length());
    ui->routeTable->setColumnCount(1);

    auto timeTip = new QTableWidgetItem(QString("API Predict Time: %1min%2s")
                                        .arg(QString::number(time/60)).arg(QString::number(time%60)));

    ui->routeTable->setItem(0,0,timeTip);

    for(int i = 0;i < _instructions.length();++i)
    {
        ui->routeTable->setItem(i+1,0,instructions_list[i]);
    }
}

void MainWindow::handelWithWeekday(int id)
{
    weekday = id;
}

//quit threads before terminate the app
MainWindow::~MainWindow()
{
    m_loadThread->quit();
    m_loadThread->wait();
    m_chartThread->quit();
    m_chartThread->wait();
    delete ui;
}

