#include "heartbeatController.h"
HeartBeatController::HeartBeatController(BaseController* parent):BaseController(parent) {
    netManager = new QNetworkAccessManager(this);
    statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &HeartBeatController::heartbeat);
    statusTimer->start(2 * 60 * 1000);  // Update online status every 5 minutes
}

void HeartBeatController::setOnlineStatus(const QString &username, bool online) {
    currentUsername = username;

    QJsonObject json;
    json["username"] = username;

    QNetworkRequest request(QUrl("http://192.168.16.202:5000/heartbeat"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    netManager->post(request, QJsonDocument(json).toJson());
}

void HeartBeatController::heartbeat() {
    if (!currentUsername.isEmpty()) {
        setOnlineStatus(currentUsername, true);
    }
}
void HeartBeatController::handleViewSignal(const QJsonObject &data) {
    if(data.contains("operation_type")){
        QString operation_type = data["operation_type"].toString();
        if(operation_type == "login"){
            if(data.contains("username")){
                QString username = data["username"].toString();
                setOnlineStatus(username, true);
            }
        }
    }
}