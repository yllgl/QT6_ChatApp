#ifndef ABSTRACTCHATITEM_H
#define ABSTRACTCHATITEM_H
#include <QString>
#include "message.h"
#include <QList>
#include <QObject>
class AbstractChatItem :public QObject {
public:
    AbstractChatItem(QString id, QString name,QList<Message>* messages,bool isgroup = false,QPixmap* avatar = nullptr)
        : m_id(id), m_name(name),m_messages(messages),m_avatar(avatar),m_isgroup(isgroup) {}

    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QList<Message>* messages() const { return m_messages; }
    QPixmap* avatar() const { return m_avatar; }
    bool isGroup() const { return m_isgroup; }
    void addMessage(Message message){m_messages->append(message);}
    operator==(const AbstractChatItem& other) const {
        return m_id == other.m_id && m_isgroup == other.m_isgroup;
    }
private:
    QString m_id;
    QString m_name;
    QPixmap* m_avatar;
    QList<Message>* m_messages;
    bool m_isgroup;
};
#endif // ABSTRACTCHATITEM_H