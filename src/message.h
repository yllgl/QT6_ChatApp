#ifndef MESSAGE_H
#define MESSAGE_H
#include "user.h"
#include <QDateTime>
class Message {
public:
    Message(const User sender, const QDateTime timestamp, const QString content)
        : m_sender(sender), m_content(content),m_timestamp(timestamp) {}

    User sender() const { return m_sender; }
    QString content() const { return m_content; }
    QDateTime timestamp() const { return m_timestamp; }
private:
    User m_sender;
    QString m_content;
    QDateTime m_timestamp;
};
#endif // MESSAGE_H