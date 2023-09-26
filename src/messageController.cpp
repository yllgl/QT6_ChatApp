#include "messageController.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QJsonDocument>
#include <QDateTime>
#include "NetworkHelper.h"
#include <qDebug>
#include "MainController.h"
#include "user.h"
void MessageController::handleViewSignal(const QJsonObject &data){
    if(data.contains("operation_type")){
        QString operation_type = data["operation_type"].toString();
        qDebug() << "MessageController::handleViewSignal" << operation_type;
        if(operation_type == "messageController_send_message"){
            sendMessage(data);
        }
        else if(operation_type == "messageController_login"){
            if(data["operation_status"].toString().contains("success")){
                QString user_id = QString::number(data["user_id"].toInt());
                QString nickname = data["nickname"].toString();
                QString username = data["username"].toString();
                m_user = new User(user_id,username,nickname);
            }
        }
        else if(operation_type == "messageController_initMessage"){
            initMessage(data);
        }
    }
}
void MessageController::initMessage(const QJsonObject &data){
    helper = QSharedPointer<NetworkHelper>::create(this);
    connect(helper.data(), &NetworkHelper::requestFinished, this, [this](const QString &identifier, const QJsonObject &response){
            this->RequestSuccess(identifier, response);
    });
    connect(helper.data(), &NetworkHelper::requestFailed, this, [this](const QString &identifier, const QString &errorMessage){
        this->RequestFailed(identifier, errorMessage);
    });
    QJsonArray group_ids = data["group_ids"].toArray();
    QJsonArray friend_ids = data["friend_ids"].toArray();
    for(int i = 0; i < group_ids.size(); i++){
        QJsonObject send_data = QJsonObject({
            {"user_id",(m_user->id()).toInt()},
            {"group_id",group_ids[i].toInt()}
        });
        helper->sendRequest(QUrl("http://192.168.16.202:5000/get_group_message"),send_data,"get_group_message");
    }
    for(int i = 0; i < friend_ids.size(); i++){
        QJsonObject send_data = QJsonObject({
            {"user_id",(m_user->id()).toInt()},
            {"friend_id",friend_ids[i].toInt()}
        });
        helper->sendRequest(QUrl("http://192.168.16.202:5000/get_friend_message"),send_data,"get_friend_message");
    }
}
void MessageController::receiveMessage(){
    while (this->udpSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(int(this->udpSocket->pendingDatagramSize()));
            this->udpSocket->readDatagram(datagram.data(), datagram.size());

            QJsonDocument jsonDoc = QJsonDocument::fromJson(datagram);
            QJsonObject jsonObj = jsonDoc.object();

            if(jsonObj.contains("other_user_join_group")){
                jsonObj["operation_type"] = QString("chatItemController_other_user_join_group");
                dynamic_cast<MainController*>(this->getMainController())->handleViewSignal(jsonObj);
                continue;
            }
            if(jsonObj.contains("other_user_add_friend")){
                jsonObj["operation_type"] = QString("chatItemController_other_user_add_friend");
                dynamic_cast<MainController*>(this->getMainController())->handleViewSignal(jsonObj);
                continue;
            }
            jsonObj["operation_type"] = QString("chatItemController_receive_message");
            if(jsonObj.contains("receiver_id")){
                jsonObj["isgroup"] = false;
                if(jsonObj["receiver_id"].toInt() == m_user->id().toInt()){
                    dynamic_cast<MainController*>(this->getMainController())->handleViewSignal(jsonObj);
                }
            }
            else if(jsonObj.contains("group_id")){
                if(jsonObj["author_id"].toInt() == m_user->id().toInt())
                    continue;
                jsonObj["isgroup"] = true;
                dynamic_cast<MainController*>(this->getMainController())->handleViewSignal(jsonObj);
            }
        }
}
void MessageController::RequestSuccess(const QString &identifier, const QJsonObject &response){
    if(identifier == "send_group_message"){
            qDebug() << response["message"];
    }
    else if(identifier == "send_private_message"){
        qDebug() << response["message"];
    }
    else if(identifier == "get_group_message"){
        QJsonObject jsonObj = response;
        jsonObj["operation_type"] = QString("chatItemController_init_message");
        jsonObj["isgroup"] = true;
        dynamic_cast<MainController*>(this->getMainController())->handleViewSignal(jsonObj);
    }
    else if(identifier == "get_friend_message"){
        QJsonObject jsonObj = response;
        jsonObj["operation_type"] = QString("chatItemController_init_message");
        jsonObj["isgroup"] = false;
        dynamic_cast<MainController*>(this->getMainController())->handleViewSignal(jsonObj);
    }
    else{
        qDebug() << "MessageController::RequestSuccess" << identifier;
    }
}
void MessageController::RequestFailed(const QString &identifier, const QString &errorMessage){
    qDebug() << identifier << errorMessage;
}
void MessageController::sendMessage(const QJsonObject &data){
    helper = QSharedPointer<NetworkHelper>::create(this);
    connect(helper.data(), &NetworkHelper::requestFinished, this, [this](const QString &identifier, const QJsonObject &response){
            this->RequestSuccess(identifier, response);
    });
    connect(helper.data(), &NetworkHelper::requestFailed, this, [this](const QString &identifier, const QString &errorMessage){
        this->RequestFailed(identifier, errorMessage);
    });
    qDebug() << "MessageController::sendMessage";
    if(data.contains("isgroup")){
        if(data["isgroup"].toBool()){
            int group_id = data["group_id"].toInt();
            qDebug() << "group_id:"<<group_id << "content:" << data["content"].toString() << "user_id:" << m_user->id();
            QJsonObject send_data = QJsonObject({
                {"user_id",(m_user->id()).toInt()},
                {"group_id",group_id},
                {"content",data["content"].toString()}
            });
            helper->sendRequest(QUrl("http://192.168.16.202:5000/send_group_message"),send_data,"send_group_message");
        }
        else{
            int friend_id = data["friend_id"].toInt();
            QJsonObject send_data = QJsonObject({
                {"user_id",(m_user->id()).toInt()},
                {"friend_id",friend_id},
                {"content",data["content"].toString()}
            });
            helper->sendRequest(QUrl("http://192.168.16.202:5000/send_private_message"),send_data,"send_private_message");
        }
    }
}