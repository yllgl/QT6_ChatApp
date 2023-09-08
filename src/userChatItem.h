#ifndef USERCHATITEM_H
#define USERCHATITEM_H
#include "abstractChatItem.h"
#include "user.h"
class UserChatItem : public AbstractChatItem {
public:
    UserChatItem(QString userid, QString username,QString nickname)
        : AbstractChatItem(userid, nickname, new QList<Message>(),false),m_username(username) {}
    UserChatItem(User *user)
        : AbstractChatItem(user->id(), user->nickname(), new QList<Message>(),false),m_username(user->username()) {}
    QString username() const { return m_username; }
    private:
    QString m_username;
};
#endif // USERCHATITEM_H
