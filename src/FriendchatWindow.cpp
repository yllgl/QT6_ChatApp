#include "FriendchatWindow.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include "userChatItem.h"
#include <QJsonArray>
#include <QJsonObject>
#include "utils.h"
#include <qDebug>
FriendChatWindow::FriendChatWindow(AbstractChatItem* chatItem,QWidget *parent) : QWidget(parent) {
    initUI();
    qDebug() << "FriendChatWindow::FriendChatWindow start";
    UserChatItem* chatitem = dynamic_cast<UserChatItem*>(chatItem);
    if(chatitem != nullptr){
        m_chatItem = chatitem;
        qDebug() << "FriendChatWindow::FriendChatWindow chatitem != nullptr";
        qDebug() << "FriendChatWindow::FriendChatWindow" << pointerToString(chatitem);
        QList<Message>* messages = chatitem->messages();
        for(int i = 0; i < messages->size(); i++){
            Message message = messages->at(i);
            addChatMessage(message.sender().username(),message.content());
        }
    }
}
void FriendChatWindow::initUI(){
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Top menu bar with Chat and Files buttons
    QHBoxLayout *menuBarLayout = new QHBoxLayout();
    chatButton = new QPushButton("Chat");
    fileButton = new QPushButton("File");
    menuBarLayout->addWidget(chatButton);
    menuBarLayout->addWidget(fileButton);
    mainLayout->addLayout(menuBarLayout);

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
    connect(sendButton,&QPushButton::clicked,this,&FriendChatWindow::sendMessages);
    leftSplitter->addWidget(sendWidget);

    // Set initial sizes for the left splitter (5:1 ratio)
    leftSplitter->setSizes({500, 100});
    mainLayout->addWidget(leftSplitter);
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
void FriendChatWindow::addChatMessage(const QString &username, const QString &message)    // Add a chat message
    {
        qDebug() << "FriendChatWindow::addChatMessage start";
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
    qDebug() << "FriendChatWindow::addChatMessage end";
}


void FriendChatWindow::removeChatMessage(int index)// Remove a chat message
    {
    if (index >= 0 && index < messageList->count()) {
        delete messageList->takeItem(index);
    }
}


// Clear all chat messages
void FriendChatWindow::clearChatMessages() {
    messageList->clear();
}
void FriendChatWindow::updateUI(const QJsonObject &data){
    qDebug() << "FriendChatWindow::updateUI";
    if(data.contains("operation_type")){
        QString operation_type = data["operation_type"].toString();
        if(operation_type.contains("updateChatWindow") && operation_type.contains("message")){
            QString pointer = data["pointer"].toString();
            qDebug() << "FriendChatWindow::updateUI" << pointer;
            AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer);
            qDebug() << "FriendChatWindow::updateUI" << "id:"<<ptr->id()<<"isgroup:"<<ptr->isGroup();
            qDebug() << "ptr:" << pointer << "m_chatItem:" << pointerToString(m_chatItem);
            if(m_chatItem == ptr)
                addChatMessage(data["username"].toString(),data["content"].toString());
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
void FriendChatWindow::sendMessages(){
    if(sendTextEdit->toPlainText().isEmpty())
        return;
    QJsonObject jsonObj;
    jsonObj["operation_type"] = QString("chatItemController_send_message");
    jsonObj["isgroup"] = false;
    jsonObj["friend_id"] = m_chatItem->id().toInt();
    jsonObj["content"] = sendTextEdit->toPlainText();
    jsonObj["pointer"] = pointerToString(m_chatItem);
    sendTextEdit->clear();
    emit signal->sendToController(jsonObj);   
}

void FriendChatWindow::handleCleanup(){
    QJsonObject jsonObj;
    jsonObj["isRemoveView"] = true;
    QJsonArray arr;
    arr.append(pointerToString(this));
    jsonObj["RemoveView"] = arr;
    emit signal->sendToController(jsonObj);
}