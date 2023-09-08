#include "ChatWidgetItem.h"
#include "mainwindow.h"
#include "abstractChatItem.h"
#include "utils.h"
#include <qDebug>
ChatWidgetItem::ChatWidgetItem(AbstractChatItem* chatItem,QWidget *parent) : m_chatItem(chatItem),QWidget(parent) {
    initUI();
}
void ChatWidgetItem::initUI(){
    avatarLabel = new QLabel(this);
    nameLabel = new QLabel(this);
    contentLabel = new QLabel(this);
    timeLabel = new QLabel(this);
    messageCountLabel = new QLabel(this);
    setAttribute(Qt::WA_Hover, true);
    // Set styles
    nameLabel->setStyleSheet("color: red; font-size: 16px;");
    contentLabel->setStyleSheet("color: gray; font-size: 12px;");

    // Layouts
    textLayout = new QVBoxLayout;
    textLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);
    textLayout->addWidget(nameLabel,1, Qt::AlignmentFlag::AlignLeft);
    textLayout->addWidget(contentLabel,1, Qt::AlignmentFlag::AlignLeft);

    rightLayout = new QVBoxLayout;
    rightLayout->addWidget(timeLabel, 0, Qt::AlignmentFlag::AlignRight);
    rightLayout->addWidget(messageCountLabel, 0, Qt::AlignmentFlag::AlignRight);

    mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(avatarLabel);
    mainLayout->addLayout(textLayout);
    mainLayout->addLayout(rightLayout);
    mainLayout->setSpacing(0); // 将主布局的间距设置为0
    textLayout->setContentsMargins(5, 0, 0, 0); // 设置左边距为5像素，其他边距为0
    mainLayout->setStretchFactor(avatarLabel, 0);  // 头像没有弹性
    mainLayout->setStretchFactor(textLayout, 1);   // 文本布局有弹性，因此它会尽量占据更多空间
    mainLayout->setStretchFactor(rightLayout, 0);
}
void ChatWidgetItem::setAvatar(const QPixmap &pixmap) {
    avatarLabel->setPixmap(pixmap.scaled(50, 50, Qt::AspectRatioMode::KeepAspectRatio));
}

void ChatWidgetItem::setChatName(const QString &name) {
    nameLabel->setText(name);
    nameLabel->setAlignment(Qt::AlignLeft);
    
}

void ChatWidgetItem::setChatContent(const QString &content) {
    contentLabel->setText(content);
    contentLabel->setAlignment(Qt::AlignLeft);
}

void ChatWidgetItem::setMessageTime(const QString &time) {
    timeLabel->setText(time);
}

void ChatWidgetItem::setTotalMessages(int count) {
    messageCountLabel->setText(QString::number(count));
}

void ChatWidgetItem::onDoubleClick(){
    qDebug()<<"double clicked";
    QMainWindow* qmainwindow = findMainWindow(this);
    if(qmainwindow == nullptr){
        qDebug()<<"qmainwindow is nullptr";
    }
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(qmainwindow);
    if(mainwindow == nullptr){
        qDebug()<<"mainwindow is nullptr";
    }
    emit mainwindow->signal->sendToController(QJsonObject({
        {"operation_type","process_openChatWindow"},
        {"id",m_chatItem->id().toInt()},
        {"isgroup",m_chatItem->isGroup()},
        {"name",m_chatItem->name()},
        {"pointer",pointerToString(m_chatItem)},
        {"parent_pointer",pointerToString(mainwindow)}
    }));
    qDebug()<<"double clicked finished";
}