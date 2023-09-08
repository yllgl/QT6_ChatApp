#include "mainwindow.h"
#include "ChatWidgetItem.h"
#include "abstractChatItem.h"
#include <QToolButton>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QResizeEvent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QScrollBar>
#include <QInputDialog>
#include <QFileDialog>
#include <QLabel>
#include "utils.h"
#include "userChatItem.h"
void MainWindow::initUI(){
// Set window title and size
    setWindowTitle("Sample Qt6 App");
    resize(200, 500);

    // Create a central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create a vertical layout for the central widget
    mainLayout = new QVBoxLayout(centralWidget);

    // Add a title label at the top
    titleLabel = new QLabel("Your Title Here", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; padding: 20px;");
    mainLayout->addWidget(titleLabel);

    // Add a search box
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Search...");
    mainLayout->addWidget(searchBox);

    // Add a list below the search box
    listWidget = new QListWidget(this);
    
    mainLayout->addWidget(listWidget);

    QToolButton *menuButton = new QToolButton(this);
    menuButton->setStyleSheet("QToolButton { border: none; background-color: transparent; }");
    menuButton->setIcon(createMenuIcon());
    menuButton->setIconSize(QSize(24, 24));  // Adjust size as needed

    // Create context menu for the button
    QMenu *menu = new QMenu(this);
    QAction *addUserAction = menu->addAction("Add User");
    QAction *addGroupAction = menu->addAction("Add Group");

    connect(addUserAction, &QAction::triggered, this, &MainWindow::add_user);
    connect(addGroupAction, &QAction::triggered, this, &MainWindow::add_group);

    menuButton->setMenu(menu);
    menuButton->setPopupMode(QToolButton::InstantPopup);

    // Add the button to the layout
    mainLayout->addWidget(menuButton, 0, Qt::AlignLeft); // This will align the button to the left

    // // Add a status bar at the bottom
    // statusBar = new QStatusBar(this);
    // setStatusBar(statusBar);
    // // statusBar->showMessage("Ready");
    // QToolButton *menuButton = new QToolButton(this);
    // menuButton->setStyleSheet("QToolButton { border: none; background-color: transparent; }");
    // menuButton->setIcon(createMenuIcon());
    // menuButton->setIconSize(QSize(24, 24));  // Adjust size as needed

    // // Create context menu for the button
    // QMenu *menu = new QMenu(this);
    // QAction *addUserAction = menu->addAction("Add User");
    // QAction *addGroupAction = menu->addAction("Add Group");

    // connect(addUserAction, &QAction::triggered, this, &MainWindow::add_user);
    // connect(addGroupAction, &QAction::triggered, this, &MainWindow::add_group);

    // menuButton->setMenu(menu);
    // menuButton->setPopupMode(QToolButton::InstantPopup);

    // // Position button at bottom-left
    // menuButton->move(10, this->height() - menuButton->height() -(10));
    // menuButton->show();

    // // Ensure the button remains at the bottom-left corner even when the window is resized
    // connect(this, &MainWindow::resized, [menuButton, this]() {
    //     menuButton->move(10, this->height() - menuButton->height() -(10));
    // });
}

// Create an icon with three horizontal lines
QIcon MainWindow::createMenuIcon() {
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(Qt::gray);

    for (int i = 0; i < 3; ++i) {
        painter.drawLine(2, 8 * (i + 1), 22, 8 * (i + 1));
    }

    return QIcon(pixmap);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    emit resized();
}

// Placeholder functions
void MainWindow::add_user() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("请输入想添加的用户"),
                                         tr("User name:"), QLineEdit::Normal,
                                          QString(),&ok);
    if (ok && !text.isEmpty()){
        QJsonObject obj;
        obj["username"] = text;
        obj["operation_type"] = "chatItemChontroller_add_user";
        emit this->signal->sendToController(obj);
    }
}

void MainWindow::add_group() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("请输入想添加的组的ID"),
                                         tr("group ID:"), QLineEdit::Normal,
                                          QString(),&ok);
    if (ok && !text.isEmpty() && text.toInt() > 0){
        QJsonObject obj;
        obj["group_id"] = text.toInt();
        obj["operation_type"] = "chatItemChontroller_add_group";
        emit this->signal->sendToController(obj);
    }
}
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),View() {
    initUI();
}
void MainWindow::addChatItem(AbstractChatItem &item){
    ChatWidgetItem *chatItem = new ChatWidgetItem(&item);
    if(item.avatar()!=nullptr){
        chatItem->setAvatar(*(item.avatar())); // 设置头像
    }
    else{
        chatItem->setAvatar(QPixmap("ic_launcher.png")); // 设置头像
    }
    if(item.isGroup()){
        chatItem->setChatName(item.name());
    }
    else{
        chatItem->setChatName(QString("%1-%2").arg((reinterpret_cast<UserChatItem*>(&item)->username()),item.name()));
    }
    // chatItem->setChatName(item.name());
    if(item.messages()!=nullptr&&item.messages()->size()>0){
        Message lastMessage=item.messages()->at(item.messages()->size()-1);
        chatItem->setChatContent(lastMessage.content());
        chatItem->setMessageTime(lastMessage.timestamp().toString("hh:mm:ss"));
        chatItem->setTotalMessages(item.messages()->size());
    }
    else{
        chatItem->setChatContent(QString(""));
        chatItem->setMessageTime(QString(""));
        chatItem->setTotalMessages(0);
    }
    // 将自定义小部件添加到QListWidget
    QListWidgetItem *listWidgetItem = new QListWidgetItem(listWidget);
    listWidgetItem->setSizeHint(chatItem->sizeHint()); // 调整大小以适应ChatWidgetItem
    listWidget->addItem(listWidgetItem);
    listWidget->setItemWidget(listWidgetItem, chatItem);
}
void MainWindow::removeChatItem(const int index){
    if(index<0||index>=listWidget->count()){
        return;
    }
    QListWidgetItem *item=listWidget->takeItem(index);
    delete item;
}

void MainWindow::updateUI(const QJsonObject &data){
    if(data.contains("operation_type")){
        QString operation_type = data["operation_type"].toString();
        if(operation_type == "addChatWidgetItem"){
            QString pointer_str = data["pointer"].toString();
            AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer_str);
            addChatItem(*ptr);
            return;
        }
        // else if(operation_type == "mainWindow_initChatWidgetItem"){
        //     QString pointer_str = data["pointer"].toString();
        //     AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer_str);
        //     addChatItem(*ptr);
        //     return;
        // }
        else if(operation_type == "removeChatWidgetItem"){
            QString pointer = data["pointer"].toString();
            AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer);
            for(int i = 0;i < listWidget->count();i++){
                ChatWidgetItem* item = (ChatWidgetItem*)listWidget->itemWidget(listWidget->item(i));
                if(item->getChatItem() == ptr){
                    removeChatItem(i);
                    return;
                }
            }
        }
        else if(operation_type.contains("updateChatWindow") && operation_type.contains("message")){
            QString pointer = data["pointer"].toString();
            AbstractChatItem* ptr = (AbstractChatItem*)stringToPointer(pointer);
            for(int i = 0;i < listWidget->count();i++){
                ChatWidgetItem* item = (ChatWidgetItem*)listWidget->itemWidget(listWidget->item(i));
                if(item->getChatItem() == ptr && ptr->messages()->size()>0){
                    item->setChatContent(ptr->messages()->at(ptr->messages()->size()-1).content());
                    item->setMessageTime(ptr->messages()->at(ptr->messages()->size()-1).timestamp().toString("hh:mm:ss"));
                    item->setTotalMessages(ptr->messages()->size());
                    return;
                }
            }
        }
    }
}