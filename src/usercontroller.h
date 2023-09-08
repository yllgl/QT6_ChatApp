#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H
#include <QObject>
#include "user.h"
#include "BaseController.h"
class UserController : public BaseController {
    Q_OBJECT

public:
    UserController(QObject *parent = nullptr);
    void login(const QString &username, const QString &password);
    void logout();

signals:
    void loggedIn();
    void loggedOut();
private:
    User *m_user;
};
#endif // USERCONTROLLER_H