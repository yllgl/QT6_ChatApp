#include "chatItemController.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QScopedPointer>
#include "NetworkHelper.h"
#include <QSharedPointer>
#include "abstractChatItem.h"
#include "utils.h"
#include "userChatItem.h"
#include "groupChatItem.h"
#include <QMessageBox>
#include <qDebug>
#include "MainController.h"
ChatItemController::ChatItemController(BaseController* parent):BaseController(parent){
    helper = QSharedPointer<NetworkHelper>::create(static_cast<QObject*>(this));
}
QSharedPointer<AbstractChatItem> ChatItemController::getChatItem(QString id,bool isgroup){
    for(auto it = chatItems.begin();it != chatItems.end();it++){
        if((*it)->id() == id && (*it)->isGroup()==isgroup){
            return *it;
        }
    }
}
void ChatItemController::initChatItem(const QJsonObject &data){
    QJsonObject data2 = data["data"].toObject();
    int userId = data2["user_id"].toInt();
    QString nickname = data2["nickname"].toString();
    QJsonArray friendsArray = data2["friends"].toArray();
    QJsonObject groupInfo = data2["group_info"].toObject();
    QJsonObject sendToGetMessageObj;
    QJsonArray sendToGetMessageGroupArr;
    QJsonArray sendToGetMessageFriendArr;
    for (auto it = groupInfo.constBegin(); it != groupInfo.constEnd(); ++it) {
        int groupID =  it.key().toInt();
        sendToGetMessageGroupArr.append(groupID);
        QJsonObject group_info = it.value().toObject();
        QString groupName = group_info["name"].toString();
        QJsonArray groupMembers = group_info["member"].toArray();
        GroupChatItem* groupchatitem = new GroupChatItem(QString::number(groupID), groupName);
        for(auto it2 = groupMembers.constBegin(); it2 != groupMembers.constEnd(); ++it2){
            QJsonObject user_obj = it2->toObject();
            int user_id = user_obj["user_id"].toInt();
            QString nickname = user_obj["nickname"].toString();
            QString username = user_obj["username"].toString();
            groupchatitem->addMember(QString::number(user_id),username, nickname);
        }
        addChatItem(groupchatitem);
    }
    for(auto it = friendsArray.constBegin(); it != friendsArray.constEnd(); ++it){
        QJsonObject friend_obj = it->toObject();
        int friend_id = friend_obj["user_id"].toInt();
        sendToGetMessageFriendArr.append(friend_id);
        QString nickname = friend_obj["nickname"].toString();
        QString username = friend_obj["username"].toString();
        UserChatItem* userchatitem = new UserChatItem(QString::number(friend_id),username, nickname);
        addChatItem(userchatitem);
    }
    sendToGetMessageObj["operation_type"] = "messageController_initMessage";
    sendToGetMessageObj["group_ids"] = sendToGetMessageGroupArr;
    sendToGetMessageObj["friend_ids"] = sendToGetMessageFriendArr;
    MainController* maincontroller = dynamic_cast<MainController*>(this->getMainController());
    if(maincontroller == nullptr)
        qDebug() << "maincontroller is nullptr";
    else{
        qDebug() << "maincontroller is not nullptr";
        maincontroller->handleViewSignal(sendToGetMessageObj);
    }
}
void ChatItemController::ReceiveMessage(const QJsonObject &data){
    if(data["isgroup"].toBool()){
        int group_id = data["group_id"].toInt();
        QString sender_id;
        QString sender_nickname;
        QString sender_username;
        if(data.contains("author_id") && data.contains("nickname") && data.contains("username")){
            sender_id = QString::number(data["author_id"].toInt());
            sender_nickname = data["nickname"].toString();
            sender_username = data["username"].toString();
        }   
        else{
            sender_id = user->id();
            sender_nickname = user->nickname();
            sender_username = user->username();
        }
        QString content = data["content"].toString();
        QString time = data["timestamp"].toString();
        QDateTime timestamp = QDateTime::fromString(time, Qt::ISODate);
        Message msg(User(sender_id,sender_username,sender_nickname),timestamp,content);
        QSharedPointer<AbstractChatItem> p =  getChatItem(QString::number(group_id),true);
        p->addMessage(msg);
        emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("updateChatWindow_receive_message"),p));
        return;
    }
    else{
        QString friend_id = QString::number(data["from_friend_id"].toInt());
        QString sender_id;
        QString sender_nickname;
        QString sender_username;
        if(data.contains("author_id") && data.contains("nickname") && data.contains("username")){
            sender_id = QString::number(data["author_id"].toInt());
            sender_nickname = data["nickname"].toString();
            sender_username = data["username"].toString();
        }   
        else{
            sender_id = user->id();
            sender_nickname = user->nickname();
            sender_username = user->username();
        }
        QString content = data["content"].toString();
        QString time = data["timestamp"].toString();
        QDateTime timestamp = QDateTime::fromString(time, Qt::ISODate);
        Message msg(User(sender_id,sender_username,sender_nickname),timestamp,content);
        qDebug()<<"ChatItemController::ReceiveMessage"<<"friend_id:"<<friend_id<<"content:"<<content<<"user_id:"<<user->id();
        QSharedPointer<AbstractChatItem> p =  getChatItem(friend_id,false);
        p->addMessage(msg);
        emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("updateChatWindow_receive_message"),p));
        return;
    }
}
void ChatItemController::SendMessage(const QJsonObject &data){
    if(data["isgroup"].toBool()){
        int group_id = data["group_id"].toInt();
        QSharedPointer<AbstractChatItem> p = getChatItem(QString::number(group_id),true);
        QString sender_id;
        QString sender_nickname;
        QString sender_username;
        sender_id = user->id();
        sender_nickname = user->nickname();
        sender_username = user->username();
        QString content = data["content"].toString();
        QDateTime timestamp = QDateTime::currentDateTime();
        Message msg(User(sender_id,sender_username,sender_nickname),timestamp,content);
        qDebug() << "ChatItemController::handleViewSignal"<< "group_id:" << group_id << "content:" << content << "user_id:" << user->id();
        p->addMessage(msg);
        qDebug() << "pointer:" << pointerToString(p.data()) << "get_pointer:" << data["pointer"].toString();
        emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("updateChatWindow_send_message"),p));
        MainController* maincontroller = dynamic_cast<MainController*>(this->getMainController());
        if(maincontroller == nullptr)
            qDebug() << "maincontroller is nullptr";
        else{
            qDebug() << "maincontroller is not nullptr";
            maincontroller->handleViewSignal(QJsonObject({
                {"operation_type","messageController_send_message"},
                {"group_id",group_id},
                {"content",content},
                {"user_id",user->id().toInt()},
                {"isgroup",true}
            }));
        }
        return;
    }
    else{
        QString friend_id = QString::number(data["friend_id"].toInt());
        QString sender_id;
        QString sender_nickname;
        QString sender_username;
        sender_id = user->id();
        sender_nickname = user->nickname();
        sender_username = user->username();
        QString content = data["content"].toString();
        QDateTime timestamp = QDateTime::currentDateTime();
        Message msg(User(sender_id,sender_username,sender_nickname),timestamp,content);
        QSharedPointer<AbstractChatItem> p = getChatItem(friend_id,false);
        p->addMessage(msg);
        emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("updateChatWindow_send_message"),p));
        qDebug() << "ChatItemController::handleViewSignal"<< "friend_id:" << friend_id << "content:" << content << "user_id:" << user->id();
        qDebug() << "pointer:" << pointerToString(p.data()) << "get_pointer:" << data["pointer"].toString();
        dynamic_cast<MainController*>(this->getMainController())->handleViewSignal(QJsonObject({
            {"operation_type","messageController_send_message"},
            {"friend_id",friend_id.toInt()},
            {"content",content},
            {"user_id",user->id().toInt()},
            {"isgroup",false}
        }));
        return;
    }
}
void ChatItemController::initMessage(const QJsonObject &data){
    if(data["isgroup"].toBool()){
        QSharedPointer<AbstractChatItem> p =  getChatItem(QString::number(data["group_id"].toInt()),true);
        if(p == nullptr){
            qDebug() << "ChatItemController::initMessage" << "p is nullptr";
            return;
        }
        QJsonArray messages = data["group_message_list"].toArray();
        for(int i = 0;i < messages.size();i++){
            QJsonObject message = messages[i].toObject();
            QString sender_id = QString::number(message["author_id"].toInt());
            QString sender_nickname = message["nickname"].toString();
            QString sender_username = message["username"].toString();
            QString content = message["content"].toString();
            QString time = message["timestamp"].toString();
            QDateTime timestamp = QDateTime::fromString(time, Qt::ISODate);
            Message msg(User(sender_id,sender_username,sender_nickname),timestamp,content);
            p->addMessage(msg);
        }
        emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("updateChatWindow_message"),p));
    }
    else{
        QSharedPointer<AbstractChatItem> p =  getChatItem(QString::number(data["friend_id"].toInt()),false);
        if(p == nullptr){
            qDebug() << "ChatItemController::initMessage" << "p is nullptr";
            return;
        }
        QJsonArray messages = data["friend_message_list"].toArray();
        for(int i = 0;i < messages.size();i++){
            QJsonObject message = messages[i].toObject();
            QString sender_id = QString::number(message["author_id"].toInt());
            QString sender_nickname = message["nickname"].toString();
            QString sender_username = message["username"].toString();
            QString content = message["content"].toString();
            QString time = message["timestamp"].toString();
            QDateTime timestamp = QDateTime::fromString(time, Qt::ISODate);
            Message msg(User(sender_id,sender_username,sender_nickname),timestamp,content);
            p->addMessage(msg);
        }
        emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("updateChatWindow_message"),p));
    }
}
void ChatItemController::handleViewSignal(const QJsonObject &data){
    if(data.contains("operation_type")){
        QString operation_type = data["operation_type"].toString();
        qDebug() << "ChatItemController::handleViewSignal" << "operation_type:" << operation_type;
        if(operation_type=="chatItemController_login"){
            if(data["operation_status"].toString().contains("success")){
                int user_id = data["user_id"].toInt();
                QString nickname = data["nickname"].toString();
                QString username = data["username"].toString();
                qDebug() << "user_id:" << user_id << "nickname:" << nickname << "username:" << username;
                user = new User(QString::number(user_id),username,nickname);
            }
        }
        else if(operation_type == "chatItemController_initChatItem"){
            initChatItem(data);
        }
        else if(operation_type == "chatItemController_receive_message"){
            ReceiveMessage(data);
        }
        else if(operation_type == "chatItemController_send_message"){
            SendMessage(data);
        }
        else if(operation_type == "chatItemController_init_message"){
            initMessage(data);
        }
        else if(operation_type == "chatItemChontroller_add_group" || operation_type == "chatItemChontroller_add_user"){
            
            helper = QSharedPointer<NetworkHelper>::create(static_cast<QObject*>(this));
            connect(helper.data(), &NetworkHelper::requestFinished, this, [this](const QString &identifier, const QJsonObject &response){
                    this->RequestSuccess(identifier, response);
            });
            connect(helper.data(), &NetworkHelper::requestFailed, this, [this](const QString &identifier, const QString &errorMessage){
                this->RequestFailed(identifier, errorMessage);
            });
            if(operation_type == "chatItemChontroller_add_user"){
                QString username = data["username"].toString();
                int user_id = user->id().toInt();
                QJsonObject send_data = QJsonObject({
                    {"friend_username",username},
                    {"user_id",user_id}
                });
                helper->sendRequest(QUrl("http://192.168.16.202:5000/add_friend"), send_data, "add_user");
            }
            else if(operation_type == "chatItemChontroller_add_group"){
                int group_id = data["group_id"].toInt();
                int user_id = user->id().toInt();
                qDebug() << "group_id:" << group_id <<"user_id:"<< user_id;
                QJsonObject send_data = QJsonObject({
                    {"group_id",group_id},
                    {"user_id",user_id}
                });
                helper->sendRequest(QUrl("http://192.168.16.202:5000/join_group"),send_data,"add_group");
            }
        }
        else if(operation_type == "chatItemController_other_user_join_group"){
            int group_id = data["group_id"].toInt();
            dynamic_cast<GroupChatItem*>(getChatItem(QString::number(group_id),true).data())->addMember(QString::number(data["user_id"].toInt()),data["username"].toString(),data["nickname"].toString());
            emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("updateChatWindow_other_user_join_group"),getChatItem(QString::number(group_id),true)));
        }
        else if(operation_type == "chatItemController_other_user_add_friend"){
            int friend_id = data["user_id"].toInt();
            bool isfound = false;
            for(auto it = chatItems.begin();it != chatItems.end();it++){
                if((*it)->id().toInt() == friend_id && !(*it)->isGroup()){
                    return;
                }
            }
            QString nickname = data["nickname"].toString();
            QString username = data["username"].toString();
            UserChatItem* userchatitem = new UserChatItem(QString::number(friend_id),username, nickname);
            addChatItem(userchatitem);
        }
    }
}
QJsonObject ChatItemController::getChatItemInfoJson(QString operation_type,QSharedPointer<AbstractChatItem> ptr){
    GroupChatItem* groupchatitem = dynamic_cast<GroupChatItem*>(ptr.data());
    QString join_user_group_id;
    QString join_user_group_username;
    QString join_user_group_nickname;
    if(groupchatitem !=nullptr && groupchatitem->getMembers().size() > 0){
        join_user_group_id = groupchatitem->getMembers().back().id();
        join_user_group_username = groupchatitem->getMembers().back().username();
        join_user_group_nickname = groupchatitem->getMembers().back().nickname();
    }
    if(ptr->messages()->size() == 0)
        return QJsonObject({
            {"operation_type",operation_type},
            {"id",ptr->id().toInt()},
            {"name",ptr->name()},
            {"isgroup",ptr->isGroup()},
            {"pointer",pointerToString(ptr.data())},
            {"last_message",""},
            {"last_message_time",""},
            {"total_messages",ptr->messages()->size()},
            {"username",""},
            {"nickname",""},
            {"content",""},
            {"join_user_group_id",join_user_group_id.toInt()},
            {"join_user_group_username",join_user_group_username},
            {"join_user_group_nickname",join_user_group_nickname}
        });
    Message msg = ptr->messages()->at(ptr->messages()->size()-1);
    return QJsonObject({
        {"operation_type",operation_type},
        {"id",ptr->id().toInt()},
        {"name",ptr->name()},
        {"isgroup",ptr->isGroup()},
        {"pointer",pointerToString(ptr.data())},
        {"last_message",msg.content()},
        {"last_message_time",msg.timestamp().toString("hh:mm:ss")},
        {"total_messages",ptr->messages()->size()},
        {"username",msg.sender().username()},
        {"nickname",msg.sender().nickname()},
        {"content",msg.content()},
        {"join_user_group_id",join_user_group_id},
        {"join_user_group_username",join_user_group_username},
        {"join_user_group_nickname",join_user_group_nickname}
    });
}

void ChatItemController::addChatItem(QString id, QString name,QList<Message>* messages,bool isgroup,QPixmap* avatar){
    QSharedPointer<AbstractChatItem> chatItem = QSharedPointer<AbstractChatItem>(new AbstractChatItem(id,name,messages,isgroup,avatar));
    qDebug()<< "ChatItemController::addChatItem1" << chatItem->id() << chatItem->name() << chatItem->isGroup();
    chatItems.append(chatItem);
    emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("addChatWidgetItem"),chatItem));
}

void ChatItemController::addChatItem(AbstractChatItem* chatItem){
    QSharedPointer<AbstractChatItem> chatItem2 = QSharedPointer<AbstractChatItem>(chatItem);
    qDebug()<< "ChatItemController::addChatItem2" << chatItem2->id() << chatItem2->name() << chatItem2->isGroup();
    chatItems.append(chatItem2);
    emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("addChatWidgetItem"),chatItem2));
}

void ChatItemController::removeChatItem(QString id,bool isgroup){
    for(int i = 0;i < chatItems.size();i++){
        if(chatItems[i]->id() == id && chatItems[i]->isGroup() == isgroup){
            emit dynamic_cast<MainController*>(this->getMainController())->sendToView(getChatItemInfoJson(QString("removeChatWidgetItem"),chatItems[i]));
            chatItems.removeAt(i);
            return;
        }
    }
}

void ChatItemController::clearChatItems(){
    // chatItems.clear();
    // emit dynamic_cast<MainController*>(this->getMainController())->sendToView(QJsonObject({
    //     {"type","clearChatItems"}
    // }));
}
void ChatItemController::RequestSuccess(const QString &identifier, const QJsonObject &response){
    if(identifier == "add_user") {
        QString id = QString::number(response["user_id"].toInt());
        QString name = response["nickname"].toString();
        QString username = response["username"].toString();
        AbstractChatItem* chatItem = static_cast<AbstractChatItem*>(new UserChatItem(id,username,name));
        addChatItem(chatItem);
        // this->getMainController()->handleViewSignal(QJsonObject({
        //     {"operation_type","addChatWidgetItem"},
        //     {"pointer",pointerToString(chatItem)}
        // }));
    }
    else if(identifier == "add_group"){
        QString id = QString::number(response["group_id"].toInt());
        QString name = response["group_name"].toString();
        GroupChatItem* chatItem = new GroupChatItem(id,name);
        QJsonArray members = response["group_member"].toArray();
        for(int i = 0;i < members.size();i++){
            QJsonObject member = members[i].toObject();
            QString member_id = QString::number(member["user_id"].toInt());
            QString member_nickname = member["nickname"].toString();
            QString member_username = member["username"].toString();
            chatItem->addMember(member_id,member_username,member_nickname);
        }
        QJsonArray messages = response["group_message"].toArray();
        for(int i = 0;i < messages.size();i++){
            QJsonObject message = messages[i].toObject();
            QString sender_id = QString::number(message["author_id"].toInt());
            QString sender_nickname = message["nickname"].toString();
            QString sender_username = message["username"].toString();
            QString content = message["content"].toString();
            QString time = message["timestamp"].toString();
            QDateTime timestamp = QDateTime::fromString(time, Qt::ISODate);
            Message msg(User(sender_id,sender_username,sender_nickname),timestamp,content);
            chatItem->addMessage(msg);
        }
        addChatItem(chatItem);
        // this->getMainController()->handleViewSignal(QJsonObject({
        //     {"operation_type","addChatWidgetItem"},
        //     {"pointer",pointerToString(chatItem)}
        // }));
    }
    
}
void ChatItemController::RequestFailed(const QString &identifier, const QString &errorMessage){
    if(identifier == "add_user") {
        QMessageBox::warning(nullptr, "添加好友失败", errorMessage);
    }
    else if(identifier == "add_group"){
        QMessageBox::warning(nullptr, "加入群聊失败", errorMessage);
    }
}