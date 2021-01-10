//
//Created on 2020-12-11
//

#include "networkmanager.h"

//initialize networkmanager with a request for quick interaction later
NetworkManager::NetworkManager(QObject *parent)
{
    reply = Q_NULLPTR;
    qRegisterMetaType<QVector<QPair<double,double>>>("QVector<QPair<double,double>>");
    QNetworkRequest request;
    request.setUrl(QUrl("http://restapi.amap.com/v3/direction/driving?origin=116.45925,39.910031"
                   "&destination=116.587922,40.081577&output=xml&key=f49999863fefdfec71f191a1be54e7a2"));
    reply = manager.get(request);
}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::GetHeatPixMap(QUrl u)
{
    QNetworkRequest request;
    url=u;
    request.setUrl(url);
    if(reply != Q_NULLPTR) {
    reply->deleteLater();
    }
    reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, this, &NetworkManager::finishedHeatMap);
}

void NetworkManager::GetRoute(QUrl u)
{
    QNetworkRequest request;
    url = u;
    request.setUrl(url);
    if(reply != Q_NULLPTR) {
        reply->deleteLater();
    }
    reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, this, &NetworkManager::finishedRoute);
}

void NetworkManager::GetRoutePixMap(QUrl u,QUrl _u)
{
    QNetworkRequest request;
    url=u;
    _url = _u;
    request.setUrl(url);
    if(reply != Q_NULLPTR) {
    reply->deleteLater();
    }
    reply = manager.get(request);

    connect(reply, &QNetworkReply::finished, this, &NetworkManager::finishedRouteMap);
}

void NetworkManager::GetTips(QUrl u)
{
    QNetworkRequest request;
    url = u;
    request.setUrl(url);
    if(reply != Q_NULLPTR) {
        reply->deleteLater();
    }
    reply = manager.get(request);

    connect(reply,&QNetworkReply::finished, this, &NetworkManager::finishedTips);
}

//Use my own key to call Gaode API
void NetworkManager::on_Line_Edit(QString key)
{
    GetTips(QUrl("http://restapi.amap.com/v3/assistant/inputtips?output=json&location=104.0652,30.6590&city=028"
                 "&keywords="+key+"&key=f49999863fefdfec71f191a1be54e7a2"));
}

void NetworkManager::on_Mapbutton_clicked(QString beginltt, QString beginlat, QString endltt, QString endlat)
{
    QString list = beginltt + "," + beginlat + ";" + endltt + "," + beginlat + ";"
                 + endltt + "," + endlat + ";" + beginltt + "," + endlat;
    GetHeatPixMap(QUrl("http://restapi.amap.com/v3/staticmap?location=104.0652,30.659"
             "&zoom=10&size=1024*710&paths=2,0x000000,1,0x000000,0:"
             +list+
             "&key=f49999863fefdfec71f191a1be54e7a2"));
}

void NetworkManager::on_Routebutton_clicked(QString beginltt, QString beginlat, QString endltt, QString endlat)
{
    QUrl u = QUrl("http://restapi.amap.com/v3/direction/driving?"
                  "origin="+beginltt+","+beginlat+
                  "&destination="+endltt+","+endlat+
                  "&extensions=all&output=json&key=f49999863fefdfec71f191a1be54e7a2");

    GetRoutePixMap(QUrl("http://restapi.amap.com/v3/staticmap?location=104.0652,30.6590&zoom=10&size=1024*735"
                   "&markers=mid,,起:"+beginltt+","+beginlat+
                   "|mid,,终:"+endltt+","+endlat+
                   "&key=f49999863fefdfec71f191a1be54e7a2"),u);
}

void NetworkManager::finishedHeatMap()
{
    QByteArray bytes = reply->readAll();
    reply->deleteLater();
    reply = Q_NULLPTR;

    mpixmap.loadFromData(bytes);
    emit heatPixMapReady(mpixmap);
}

void NetworkManager::finishedRouteMap()
{
    QByteArray bytes = reply->readAll();
    reply->deleteLater();
    reply = Q_NULLPTR;

    mpixmap.loadFromData(bytes);
    emit routePixMapReady(mpixmap);
    GetRoute(_url);
}

//process json files from the API to get useful information
void NetworkManager::finishedTips()
{
    QStringList tips;
    QByteArray allData = reply->readAll();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug()<<"Json Error!";
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    QJsonObject subObj;
    QJsonArray arr = rootObj.value("tips").toArray();
    QVector<QPair<double,double>> locations;

    //Store location information in tips
    for(int i = 0; i < arr.size();++i){
        subObj = rootObj.value("tips")[i].toObject();
        if(subObj.value("location").toString().isEmpty()) continue;
        QStringList tempbar = subObj.value("location").toString().split(",");
        locations.append(QPair<double,double>(tempbar[0].toDouble(),tempbar[1].toDouble()));
        tips.append(subObj.value("name").toString());
    }

    emit tipsReady(tips,locations);
}

void NetworkManager::finishedRoute()
{
    QByteArray allData = reply->readAll();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug()<<"Json Error!";
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    QJsonObject route = rootObj.value("route").toObject();
    QJsonArray paths = route.value("paths").toArray();

    QJsonObject path = paths[0].toObject();
    QString timeString = path.value("duration").toString();
    int time = timeString.toInt();

    QJsonArray steps = path.value("steps").toArray();
    QJsonObject step;
    QStringList instructions;

    //store instruction information in instructions
    for(int i = 0;i < steps.size();++i)
    {
        step = steps[i].toObject();
        instructions.append(step.value("instruction").toString());
    }

    emit instructionsReady(instructions,time);
}




