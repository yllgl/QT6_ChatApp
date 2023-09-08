#ifndef GROUPCHATITEM_H
#define GROUPCHATITEM_H
#include "abstractChatItem.h"
#include "user.h"
class GroupChatItem : public AbstractChatItem {
public:
    GroupChatItem(const QString &groupid, const QString &groupname)
        : AbstractChatItem(groupid, groupname, new QList<Message>(),true){}
    void addMember(const User &user){
        users.append(user);
    }
    void addMember(const QString &userid,const QString &username, const QString &nickname){
        User user = User(userid,username, nickname);
        users.append(user);
    }
    void removeMember(const User &user){
        users.removeOne(user);
    }
    QList<User> getMembers() const{
        return users;
    }
    private:
        QList<User> users;
};
#endif // GROUPCHATITEM_H