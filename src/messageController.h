#ifndef MESSAGECONTROLLER_H
#define MESSAGECONTROLLER_H

#include "BaseController.h"
#include <QUdpSocket>
#include "user.h"
#include <QSharedPointer>
#include "NetworkHelper.h"
class MessageController : public BaseController {
    Q_OBJECT
public:
    MessageController(BaseController* parent):BaseController(parent){
        udpSocket.bind(QHostAddress::AnyIPv4, 12345);
        connect(&udpSocket, &QUdpSocket::readyRead, this, &MessageController::receiveMessage);
    };
    void handleViewSignal(const QJsonObject &data) override;
private:
    void receiveMessage();
    void sendMessage(const QJsonObject &data);
    void RequestSuccess(const QString &identifier, const QJsonObject &response);
    void RequestFailed(const QString &identifier, const QString &errorMessage);
    void initMessage(const QJsonObject &data);
    QUdpSocket udpSocket;
    User* m_user;
    QSharedPointer<NetworkHelper> helper;
};

#endif // MESSAGECONTROLLER_H