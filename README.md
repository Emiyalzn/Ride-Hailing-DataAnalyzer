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

## Select Grids

A screen-shot style grids selection method has been implemented.  
Click the "Select Area" button, your desktop will be in dark mode and simple press and move your mouse to choose grids(highlighted) you're interested in.    
Press "Enter" key to quit after selection, "Esc" key to force quit.  
![Select Grids](GIF/SelectAndLoad.gif)

## Chart Analysis

Click "Query" button to access for your chart.(It will be shown on the right)   
Edit two lineEdits above to tune start and end time.  
Choose your timeStep either click the spinbox or drag the slider.    
![Chart Analysis](GIF/OrderChart.gif)

Switch radio buttons to change your chart type.
![Mutiple Chart Type](GIF/MultipleCharts.gif)

We also realized rubberband to analyze the chart in details.
![Rubberband](GIF/ChartView.gif)

