#ifndef CLIENTAPP_H
#define CLIENTAPP_H
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QUdpSocket>
class LoginWidget;
class MainController;
class MainWindow;

class ClientApp : public QObject {
    Q_OBJECT
public:
    ClientApp();
private:
    MainController* maincontroller;
    QUdpSocket udpSocket;
};

#endif // CLIENTAPP_H
