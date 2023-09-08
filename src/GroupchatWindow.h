#ifndef GROUPCHATWINDOW_H
#define GROUPCHATWINDOW_H
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QJsonObject>
#include<QCloseEvent>
#include "View.h"
class AbstractChatItem;
class GroupChatItem;
class GroupChatWindow : public QWidget, public View {
    Q_OBJECT

public:
    GroupChatWindow(AbstractChatItem* chatItem,QWidget *parent = nullptr);
    void addChatMessage(const QString &username, const QString &message);
    void addUser(const QString &username,const QString &nickname);
    void removeChatMessage(int index);
    void removeUser(int index);
    void clearChatMessages();
    void updateUI(const QJsonObject &data) override;
    void closeEvent(QCloseEvent *event) override {
        handleCleanup();
        QWidget::closeEvent(event);
    }
private:
    QPushButton *chatButton;
    QPushButton *fileButton;
    QPushButton *sendButton;
    QTextEdit *sendTextEdit;
    QListWidget *messageList;
    QListWidget *userList;
    GroupChatItem* m_chatItem;
    void initUI();
    void sendMessages();
    void handleCleanup();
    
};
#endif // GROUPCHATWINDOW_H