#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QByteArray>
#include <QMessageBox>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void GetHeatPixMap(QUrl u);
    void GetRoutePixMap(QUrl u,QUrl _u);
    void GetTips(QUrl u);
    void GetRoute(QUrl u);

signals:
    void heatPixMapReady(QPixmap _mpixmap);
    void routePixMapReady(QPixmap _mpixmap);
    void tipsReady(QStringList tips,QVector<QPair<double,double>> locations);
    void instructionsReady(QStringList instructions, int time);

public slots:
    //Call API slots
    void on_Line_Edit(QString key);
    void on_Mapbutton_clicked(QString beginltt, QString beginlat, QString endltt, QString endlat);
    void on_Routebutton_clicked(QString beginltt, QString beginlat, QString endltt, QString endlat);

    //procession after getting API's results
    void finishedHeatMap();
    void finishedRouteMap();
    void finishedTips();
    void finishedRoute();

private:
    QNetworkAccessManager manager;
    QUrl url;
    QUrl _url;
    QNetworkReply *reply;
    QPixmap mpixmap;
};

#endif // NETWORKMANAGER_H
