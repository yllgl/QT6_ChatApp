#include "processController.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include "mainwindow.h"
#include "utils.h"
#include "View.h"
#include "groupChatItem.h"
#include "userChatItem.h"
#include "MainController.h"
#include "GroupchatWindow.h"
#include "FriendchatWindow.h"

#include <qDebug>
void processController::handleViewSignal(const QJsonObject &data){
    if(data.contains("operation_type")){
        QString operation_type = data["operation_type"].toString();
        if(operation_type == "login"){
            QString operation_status = data["operation_status"].toString();
            if(operation_status == "success"){
                loginProcess(data);
            }
        }
        else if(operation_type == "process_openChatWindow"){
            openChatWindow(data);
        }
    }
}
void processController::loginProcess(const QJsonObject &data){
    if(data.contains("RemoveView") && data["RemoveView"].isArray()){
        QJsonArray arr = data["RemoveView"].toArray();
        for(auto it = arr.constBegin(); it != arr.constEnd(); ++it){
            QString pointer = it->toString();
            QWidget* view = static_cast<QWidget*>(stringToPointer(pointer));
            view->close();
        }
    }
    MainWindow* mainwindow = new MainWindow();
    QJsonObject sendToGetMessageObj;
    sendToGetMessageObj["operation_type"] = "chatItemController_initChatItem";
    sendToGetMessageObj["data"] = data;
    dynamic_cast<MainController*>(this->getMainController())->appendView(QSharedPointer<View>(static_cast<View*>(mainwindow)));
    this->getMainController()->handleViewSignal(sendToGetMessageObj);
    mainwindow->show();
}
void processController::openChatWindow(const QJsonObject &data){
    bool isgroup = data["isgroup"].toBool();
    qDebug()<<"openChatWindow";
    if(isgroup){
        qDebug()<<"openChatWindow isgroup";
        AbstractChatItem* chatitem = static_cast<AbstractChatItem*>(stringToPointer(data["pointer"].toString()));
        qDebug()<<"openChatWindow chatitem";
        GroupChatWindow* groupChatWindow = new GroupChatWindow(chatitem,nullptr);
        qDebug()<<"openChatWindow groupChatWindow";
        dynamic_cast<MainController*>(this->getMainController())->appendView(QSharedPointer<View>(static_cast<View*>(groupChatWindow)));
        qDebug()<<"openChatWindow appendView";
        groupChatWindow->show();
    }
    else{
        qDebug()<<"openChatWindow friend";
        AbstractChatItem* chatitem = static_cast<AbstractChatItem*>(stringToPointer(data["pointer"].toString()));
        qDebug()<<"openChatWindow chatitem";
        FriendChatWindow* friendChatWindow = new FriendChatWindow(chatitem,nullptr);
        qDebug()<<"openChatWindow FriendChatWindow";
        dynamic_cast<MainController*>(this->getMainController())->appendView(QSharedPointer<View>(static_cast<View*>(friendChatWindow)));
        qDebug()<<"openChatWindow appendView";
        friendChatWindow->show();
    }
}