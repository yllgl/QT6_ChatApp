#ifndef HEARTBEAT_H
#define HEARTBEAT_H
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include "BaseController.h"

class HeartBeatController : public BaseController {
    Q_OBJECT
public:
    HeartBeatController(BaseController* parent);
    void handleViewSignal(const QJsonObject &data) override;
    void setOnlineStatus(const QString &username, bool online);

private slots:
    void heartbeat();

private:
    QNetworkAccessManager *netManager;
    QTimer *statusTimer;
    QString currentUsername;
};

#endif // HEARTBEAT_H