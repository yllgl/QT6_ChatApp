#ifndef USER_H
#define USER_H
#include <QString>
class User {
public:
    User(const QString id,const QString username, const QString nickname)
        : m_id(id),m_username(username), m_nickname(nickname) {}

    QString id() const { return m_id; }
    QString nickname() const { return m_nickname; }
    QString username() const { return m_username; }
    operator == (const User &other) const {
        return m_id == other.m_id;
    }
private:
    QString m_id;
    QString m_username;
    QString m_nickname;
};
#endif // USER_H