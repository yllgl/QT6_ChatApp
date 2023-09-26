#include "loginWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "utils.h"
#include "NetworkHelper.h"
#include <QScopedPointer>
LoginWidget::LoginWidget(QWidget *parent,int port) : QWidget(parent),View(){
    initUI();
    helper = QSharedPointer<NetworkHelper>::create(this);
    this->port = port;
}
void LoginWidget::initUI()
{
    // Create UI elements
    auto *titleLabel = new QLabel("登录", this);
    auto *usernameLabel = new QLabel("用户名:", this);
    auto *passwordLabel = new QLabel("密码:", this);
    auto *nicknameLabel = new QLabel("昵称:", this);
    usernameLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    nicknameLineEdit = new QLineEdit(this);
    nicknameLineEdit->setPlaceholderText("登录时不需要填写");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("登录", this);
    registerButton = new QPushButton("注册", this);

    // Setup layout
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameLineEdit);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(nicknameLabel);
    layout->addWidget(nicknameLineEdit);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    setLayout(layout);

    // Connect login button (example)
    QObject::connect(loginButton, &QPushButton::clicked, this, &LoginWidget::login);
    QObject::connect(registerButton, &QPushButton::clicked, this, &LoginWidget::registerUser);
}


void LoginWidget::login()
{
    helper = QSharedPointer<NetworkHelper>::create(this);
    // Connect signals
    connect(helper.data(), &NetworkHelper::requestFinished, this, [this](const QString &identifier, const QJsonObject &response){
        if (identifier == "login") {
            QJsonObject loginData;
            loginData["user_id"] = response["user_id"].toInt();
            loginData["port"] = QString::number(this->port);
            helper->sendRequest(QUrl("http://192.168.16.202:5000/registerPort"), loginData, "port");
            if (response.contains("message")){
                if(response["message"].toString().contains("success")) {
                    QJsonObject obj = response;
                    obj["operation_type"] = "messageController_login";
                    obj["operation_status"] = "success";
                    QJsonArray arr;
                    arr.append(pointerToString(this));
                    obj["RemoveView"] = arr;
                    emit this->signal->sendToController(obj);
                    obj["operation_type"] = "chatItemController_login";
                    emit this->signal->sendToController(obj);
                    obj["operation_type"] = "login";
                    emit this->signal->sendToController(obj);
                    QMessageBox::information(this, "Success", "User login successfully!");
                } else {
                    QString message = response["message"].toString();
                    // Display an error message to the user
                    QMessageBox::warning(this, "Login failed", message);
                }
            }
        }
        else if(identifier == "port"){
            qDebug() << "LoginWidget port" << response;
        }
    });
    connect(helper.data(), &NetworkHelper::requestFailed, this, [this](const QString &identifier, const QString &errorMessage){
        if(identifier == "login") {
            QMessageBox::warning(this, "Network error", errorMessage);
        }
    });

    // Send requests
    QJsonObject loginData;
    loginData["username"] = usernameLineEdit->text();
    loginData["password"] = passwordLineEdit->text();
    helper->sendRequest(QUrl("http://192.168.16.202:5000/login"), loginData, "login");
}


void LoginWidget::registerUser()
{
    helper = QSharedPointer<NetworkHelper>::create(this);

    // Connect signals
    connect(helper.data(), &NetworkHelper::requestFinished, this, [this](const QString &identifier, const QJsonObject &response){
        if (identifier == "register") {
            QMessageBox::information(this, "Success", "User registered successfully!");
        }
    });
    connect(helper.data(), &NetworkHelper::requestFailed, this, [this](const QString &identifier, const QString &errorMessage){
        if(identifier == "register") {
            QMessageBox::warning(this, "Failed", "Registration failed!");
        }
    });

    // Send requests
    QJsonObject registerData;
    registerData["username"] = usernameLineEdit->text();
    registerData["password"] = passwordLineEdit->text();
    registerData["nickname"] = nicknameLineEdit->text();
    helper->sendRequest(QUrl("http://192.168.16.202:5000/register"), registerData, "register");
}
void LoginWidget::updateUI(const QJsonObject &data)
{
}