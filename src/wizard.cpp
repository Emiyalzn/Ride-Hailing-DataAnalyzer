//
//Created on 2020-12-08
//

#include "wizard.h"
#include "ui_wizard.h"

Wizard::Wizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Wizard)
{
    ui->setupUi(this);

    //window settings
    setWindowTitle("Wizard");
    setWindowIcon(QIcon(":/My/figures/Taxi_1.ico"));

    qRegisterMetaType<QVector<int>>("QVector<int>");

    connect(ui->startTimeEdit,&QDateTimeEdit::dateTimeChanged,ui->endTimeEdit,&QDateTimeEdit::setMinimumDateTime);
    connect(ui->endTimeEdit,&QDateTimeEdit::dateTimeChanged,ui->startTimeEdit,&QDateTimeEdit::setMaximumDateTime);
    connect(ui->startTimeEdit,&QDateTimeEdit::dateTimeChanged,this,&Wizard::setStartTime);
    connect(ui->endTimeEdit,&QDateTimeEdit::dateTimeChanged,this,&Wizard::setEndTime);
    connect(ui->pushButtonOpen,&QPushButton::clicked,this,&Wizard::buttonClicked);
    connect(ui->buttonBox,&QDialogButtonBox::clicked,this,&Wizard::dealWithOK);
}

Wizard::~Wizard()
{
    delete ui;
}

void Wizard::buttonClicked()
{
    path = QFileDialog::getExistingDirectory(this,"Select dataset folder");
    ui->buttonBox->setEnabled(configure(path));
}

bool Wizard::configure(QString path) {
    if(path == "") return false;
    ui->lineEditPath->setText(path);
    QDir directory(path);
    if (!directory.exists("dataset")) {
        QMessageBox::warning(this, "Not Found", "Dataset folder not found!");
        return false;
    }
    return true;
}

void Wizard::setStartTime(QDateTime _start)
{
    startTime = _start;
}

void Wizard::setEndTime(QDateTime _end)
{
    endTime = _end;
}

void Wizard::dealWithOK()
{
    QVector<int> checkBoxes;

    if(!ui->checkBoxDT->isChecked() && !ui->checkBoxET->isChecked())
    {QMessageBox::warning(this, "Need Time", "You need to choose at least one time!"); return;}

    //add fields choices
    if(ui->checkBoxDT->isChecked()) checkBoxes.append(1);
    if(ui->checkBoxET->isChecked()) checkBoxes.append(2);
    if(ui->checkBoxOLong->isChecked()) checkBoxes.append(3);
    if(ui->checkBoxOLa->isChecked()) checkBoxes.append(4);
    if(ui->checkBoxELong->isChecked()) checkBoxes.append(5);
    if(ui->checkBoxELa->isChecked()) checkBoxes.append(6);
    if(ui->checkBoxFee->isChecked()) checkBoxes.append(7);

    emit resultReady(path, startTime.toString("yyyyMMdd"), endTime.toString("yyyyMMdd"), checkBoxes);
    this->accept();
}

