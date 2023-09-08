#include "GroupchatWindow.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include "groupChatItem.h"
#include <QJsonArray>
#include <QJsonObject>
#include "utils.h"
#include <qDebug>
GroupChatWindow::GroupChatWindow(AbstractChatItem* chatItem,QWidget *parent) : QWidget(parent) {
    initUI();
    qDebug() << "GroupChatWindow::GroupChatWindow start";
    GroupChatItem* chatitem = dynamic_cast<GroupChatItem*>(chatItem);
    if(chatitem != nullptr){
        m_chatItem = chatitem;
        qDebug() << "GroupChatWindow::GroupChatWindow chatitem != nullptr";
        qDebug() << "GroupChatWindow::GroupChatWindow" << pointerToString(chatitem);
        QList<Message>* messages = chatitem->messages();
        const QList<User> users = chatitem->getMembers();
        for(int i = 0; i < messages->size(); i++){
            Message message = messages->at(i);
            addChatMessage(message.sender().username(),message.content());
        }
        for(int i = 0; i < users.size(); i++){
            User user = users.at(i);
            addUser(user.username(),user.nickname());
        }
    }
}
void GroupChatWindow::initUI(){
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Top menu bar with Chat and Files buttons
    QHBoxLayout *menuBarLayout = new QHBoxLayout();
    chatButton = new QPushButton("Chat");
    fileButton = new QPushButton("File");
    menuBarLayout->addWidget(chatButton);
    menuBarLayout->addWidget(fileButton);
    mainLayout->addLayout(menuBarLayout);

    // Splitter to handle the main left and right sections
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);

    // Left section (chat area) - within a vertical splitter
    QSplitter *leftSplitter = new QSplitter(Qt::Vertical);

    // Message list
    messageList = new QListWidget();
    leftSplitter->addWidget(messageList);

    // Text edit and send button in a single widget with white background
    QWidget *sendWidget = new QWidget();
    QVBoxLayout *sendWidgetLayout = new QVBoxLayout(sendWidget);
    sendWidgetLayout->setSpacing(0); // No space between text edit and send button
    sendWidget->setStyleSheet("background-color: white; border: none;");

    sendTextEdit = new QTextEdit();
    sendTextEdit->setStyleSheet("border: none;");
    sendWidgetLayout->addWidget(sendTextEdit, 1);

    QHBoxLayout *sendButtonLayout = new QHBoxLayout();
    sendButton = new QPushButton("Send");
    sendButtonLayout->addStretch(1);
    sendButtonLayout->addWidget(sendButton);
    sendButtonLayout->setContentsMargins(0, 0, 0, 0);
    sendWidgetLayout->addLayout(sendButtonLayout);
    connect(sendButton,&QPushButton::clicked,this,&GroupChatWindow::sendMessages);
    leftSplitter->addWidget(sendWidget);

    // Set initial sizes for the left splitter (5:1 ratio)
    leftSplitter->setSizes({500, 100});

    mainSplitter->addWidget(leftSplitter);

    // Right section (list of users with avatars and usernames)
    userList = new QListWidget();
    mainSplitter->addWidget(userList);

    mainSplitter->setStretchFactor(0, 5); // Set left section to take up 5/6 of the width
    mainSplitter->setStretchFactor(1, 1); // Set right section to take up 1/6 of the width

    mainLayout->addWidget(mainSplitter);
    QString buttonStyle = "QPushButton {"
                    "    background-color: #D4D4D4;" // Example color
                    "    border: 1px solid #AAAAAA;"
                    "    border-radius: 5px;" // Rounded corners
                    "    padding: 5px 15px;" // Padding around text
                    "}"
                    "QPushButton:hover {"
                    "    background-color: #C4C4C4;" // Slightly different color when hovered
                    "}";

    chatButton->setStyleSheet(buttonStyle);
    fileButton->setStyleSheet(buttonStyle);
    sendButton->setStyleSheet(buttonStyle);
    setLayout(mainLayout);
}
void GroupChatWindow::addChatMessage(const QString &username, const QString &message)    // Add a chat message
    {
        qDebug() << "GroupChatWindow::addChatMessage start";
    QListWidgetItem *item = new QListWidgetItem(messageList);
    QWidget *messageWidget = new QWidget();
    QVBoxLayout *messageLayout = new QVBoxLayout(messageWidget);
    
    QHBoxLayout *userLayout = new QHBoxLayout();
    QLabel *avatar = new QLabel("🙂");
    QLabel *userLabel = new QLabel(username);
    userLayout->addWidget(avatar);
    userLayout->addWidget(userLabel);

    QLabel *messageContent = new QLabel(message);

    messageLayout->addLayout(userLayout);
    messageLayout->addWidget(messageContent);
    messageWidget->setLayout(messageLayout);

    item->setSizeHint(messageWidget->sizeHint());
    messageList->setItemWidget(item, messageWidget);
    qDebug() << "GroupChatWindow::addChatMessage end";
}

void GroupChatWindow::addUser(const QString &username,const QString &nickname)// Add a user
    {
    QListWidgetItem *item = new QListWidgetItem(userList);
    QWidget *userWidget = new QWidget();
    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    QLabel *avatar = new QLabel("🙂");
    QLabel *userLabel = new QLabel(QString("%1(%2)").arg(username,nickname));
    userLayout->addWidget(avatar);
    userLayout->addWidget(userLabel);
    userWidget->setLayout(userLayout);
    item->setSizeHint(userWidget->sizeHint());
    userList->setItemWidget(item, userWidget);
}

void GroupChatWindow::removeChatMessage(int index)// Remove a chat message
    {
    if (index >= 0 && index < messageList->count()) {
        delete messageList->takeItem(index);
    }
}

// Remove a user
void GroupChatWindow::removeUser(int index) {
    if (index >= 0 && index < userList->count()) {
        delete userList->takeItem(index);
    }
}

// Clear all chat messages
void GroupChatWindow::clearChatMessages() {
    messageList->clear();
}
void GroupChatWindow::updateUI(const QJsonObject &data){
    qDebug() << "GroupChatWindow::updateUI";
    if(data.contains("operation_type")){
        QString operation_type = data["operation_type"].toString();
        if(operation_type.contains("updateChatWindow") && operation_type.contains("message")){
            QString pointer = data["pointer"].toString();
            qDebug() << "GroupChatWindow::updateUI" << pointer;
            AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer);
            qDebug() << "GroupChatWindow::updateUI" << "id:"<<ptr->id()<<"isgroup:"<<ptr->isGroup();
            qDebug() << "ptr:" << pointer << "m_chatItem:" << pointerToString(m_chatItem);
            if(m_chatItem == ptr)
                addChatMessage(data["username"].toString(),data["content"].toString());
            return;
        }
        else if(operation_type == "updateChatWindow_other_user_join_group"){
            QString pointer = data["pointer"].toString();
            AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer);
            if(m_chatItem == ptr)
                addUser(data["join_user_group_username"].toString(),data["join_user_group_nickname"].toString());
            return;
        }
        // else if(operation_type == "chatItemController_init_message"){
        //     QString pointer = data["pointer"].toString();
        //     AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer);
        //     if(m_chatItem == ptr){
        //         clearChatMessages();
        //         QJsonArray messages = data["messages"].toArray();
        //         for(int i = 0; i < messages.size(); i++){
        //             QJsonObject message = messages.at(i).toObject();
        //             addChatMessage(message["username"].toString(),message["content"].toString());
        //         }
        //     }
        //     return;
        // }
    }
}
void GroupChatWindow::sendMessages(){
    if(sendTextEdit->toPlainText().isEmpty())
        return;
    QJsonObject jsonObj;
    jsonObj["operation_type"] = QString("chatItemController_send_message");
    jsonObj["isgroup"] = true;
    jsonObj["group_id"] = m_chatItem->id().toInt();
    jsonObj["content"] = sendTextEdit->toPlainText();
    jsonObj["pointer"] = pointerToString(m_chatItem);
    sendTextEdit->clear();
    emit signal->sendToController(jsonObj);   
}

void GroupChatWindow::handleCleanup(){
    QJsonObject jsonObj;
    jsonObj["isRemoveView"] = true;
    QJsonArray arr;
    arr.append(pointerToString(this));
    jsonObj["RemoveView"] = arr;
    emit signal->sendToController(jsonObj);
}