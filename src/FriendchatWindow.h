#ifndef FRIENDCHATWINDOW_H
#define FRIENDCHATWINDOW_H
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QJsonObject>
#include<QCloseEvent>
#include "View.h"
class AbstractChatItem;
class UserChatItem;
class FriendChatWindow : public QWidget, public View {
    Q_OBJECT

public:
    FriendChatWindow(AbstractChatItem* chatItem,QWidget *parent = nullptr);
    void addChatMessage(const QString &username, const QString &message);
    void removeChatMessage(int index);
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
    UserChatItem* m_chatItem;
    void initUI();
    void sendMessages();
    void handleCleanup();
    
};
#endif // FRIENDCHATWINDOW_H