#ifndef CHATWIDGETITEM_H
#define CHATWIDGETITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPalette>
#include <QMouseEvent>
#include <QEnterEvent>
class AbstractChatItem;
class ChatWidgetItem : public QWidget {
    Q_OBJECT

public:
    ChatWidgetItem(AbstractChatItem* chatItem, QWidget *parent = nullptr);
    void setAvatar(const QPixmap &pixmap);
    void setChatName(const QString &name);
    void setChatContent(const QString &content);
    void setMessageTime(const QString &time);
    void setTotalMessages(int count);
    AbstractChatItem* getChatItem(){return m_chatItem;}
    
protected:
    // Capture double click event
    void mouseDoubleClickEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            onDoubleClick();
        }
        QWidget::mouseDoubleClickEvent(event);
    }

    // Change background color on hover enter
    void enterEvent(QEnterEvent *event) override {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, Qt::lightGray);
        setPalette(palette);
        QWidget::enterEvent(event);
    }

    // Reset background color on hover leave
    void leaveEvent(QEvent *event) override {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, Qt::transparent);  // Reset to original or any other color
        setPalette(palette);
        QWidget::leaveEvent(event);
    }

private slots:
    void onDoubleClick();
private:
    QLabel *avatarLabel;
    QLabel *nameLabel;
    QLabel *contentLabel;
    QLabel *timeLabel;
    QLabel *messageCountLabel;
    QHBoxLayout *mainLayout;
    QVBoxLayout *textLayout;
    QVBoxLayout *rightLayout;
    AbstractChatItem* m_chatItem;
    void initUI();
};

#endif // CHATWIDGETITEM_H
