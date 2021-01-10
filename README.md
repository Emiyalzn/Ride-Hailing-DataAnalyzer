# Ride-Hailing-DataAnalyzer
🚖A Flexible and Multifunctional App for Analyzing Online Ride-hailing data

## Build Requirements

```shell
    Qt 5.15.1 MinGW 32 
    Make sure you have installed QtCharts and QtNetwork
```
Open the .pro file in Qt and build the project.

## Wizard

Set the correct path of the dataset.  
Pay attention that you should choose the parent directory of the folder "dataset".  
Choose fields you're interested in.  
Click OK and you'll enter the Mainwindow.  
![Simple Wizard](GIF/Wizard.gif)

## Load Data from .csv files

In MainWindow, click the "Load" button and wait.  
After a while(approximately 20 secs), the progress bar will increase to 100%, which is the symbol of the fin of the load task.  
Click "Cancel" button to quit to the Wizard if you have chosen the wrong fields.  
![Cancel](GIF/Cancel.gif)
