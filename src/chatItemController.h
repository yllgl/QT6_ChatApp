#ifndef CHATITEMCONTROLLER_H
#define CHATITEMCONTROLLER_H

#include "BaseController.h"
#include <QList>
#include "abstractChatItem.h"
#include <QSharedPointer>
#include <QJsonObject>
#include "NetworkHelper.h"
class ChatItemController : public BaseController {
    Q_OBJECT
public:
    ChatItemController(BaseController* parent);
    void handleViewSignal(const QJsonObject &data) override;
    void addChatItem(QString id, QString name,QList<Message>* messages,bool isgroup = false,QPixmap* avatar = nullptr);
    void addChatItem(AbstractChatItem* chatItem);
    void addChatItem(QSharedPointer<AbstractChatItem> chatItem){chatItems.append(chatItem);};
    void removeChatItem(QString id,bool isgroup = false);
    void clearChatItems();
    void RequestSuccess(const QString &identifier, const QJsonObject &response);
    void RequestFailed(const QString &identifier, const QString &errorMessage);
private:
    QList<QSharedPointer<AbstractChatItem>> chatItems;
    User* user;
    QJsonObject getChatItemInfoJson(QString operation_type,QSharedPointer<AbstractChatItem> ptr);
    QSharedPointer<NetworkHelper> helper;
    void initChatItem(const QJsonObject &data);
    void ReceiveMessage(const QJsonObject &data);
    void SendMessage(const QJsonObject &data);
    void initMessage(const QJsonObject &data);
    QSharedPointer<AbstractChatItem> getChatItem(QString id,bool isgroup);
};

#endif // CHATITEMCONTROLLER_H