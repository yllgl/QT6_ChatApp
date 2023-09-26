#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSharedPointer>
#include "NetworkHelper.h"
#include "View.h"
class LoginWidget : public QWidget,public View
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = nullptr,int port = 0);
    void updateUI(const QJsonObject &data) override;
private:
QLineEdit* usernameLineEdit;
QLineEdit* passwordLineEdit;
QLineEdit* nicknameLineEdit;
QPushButton* loginButton;
QPushButton* registerButton;
void login();
void registerUser();
void initUI();
QSharedPointer<NetworkHelper> helper;
int port;
};
#endif // LOGINWIDGET_H

