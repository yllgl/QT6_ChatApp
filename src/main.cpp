#include <QApplication>
// #include "mainwindow.h"
#include "ClientApp.h"
#include "loginWidget.h"
#include "MainController.h"
#include "heartbeatController.h"
#include <QSharedPointer>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientApp clientApp;
    return a.exec();
}


// #include <QCoreApplication>
// #include <QUdpSocket>
// #include <QDebug>

// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);

//     QUdpSocket udpSocket;
//     udpSocket.bind(QHostAddress::LocalHost, 12345);
    
//     QObject::connect(&udpSocket, &QUdpSocket::readyRead, [&]() {
//         while (udpSocket.hasPendingDatagrams()) {
//             QByteArray datagram;
//             datagram.resize(int(udpSocket.pendingDatagramSize()));
//             udpSocket.readDatagram(datagram.data(), datagram.size());
//             qDebug() << "Received:" << datagram;
//         }
//     });

//     return a.exec();
// }

// #include <iostream>
// #include "utils.h"
// #include <QString>
// int main() {
//     int obj = 40;

//     QString strPtr = pointerToString(&obj);
//     std::cout << "Pointer as QString: " << strPtr.toStdString() << std::endl;

//     int* restoredPtr = reinterpret_cast<int*>(stringToPointer(strPtr));
//     std::cout << "Restored pointer value: " << *restoredPtr << std::endl;

//     return 0;
// }
// // int main(int argc, char *argv[]) {
// //     QApplication app(argc, argv);
// //     MainWindow window;
// //     window.show();
// //     return app.exec();
// // }

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);
//     LoginWidget w;
//     w.show();
//     return a.exec();
// }
// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);
//     ClientApp w;
//     return a.exec();
// }
// #include <QApplication>
// #include <QMainWindow>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QPushButton>
// #include <QListWidget>
// #include <QTextEdit>
// #include <QLabel>
// #include <QSplitter>

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);

//     QMainWindow mainWindow;
//     mainWindow.setWindowTitle("Chat Application");

//     QWidget *centralWidget = new QWidget();
//     QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

//     // Top menu bar with Chat and Files buttons
//     QHBoxLayout *menuBarLayout = new QHBoxLayout();
//     QPushButton *chatButton = new QPushButton("Chat");
//     QPushButton *fileButton = new QPushButton("File");
//     menuBarLayout->addWidget(chatButton);
//     menuBarLayout->addWidget(fileButton);
//     mainLayout->addLayout(menuBarLayout);

//     // Splitter to handle the main left and right sections
//     QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);

//     // Left section (chat area) - within a vertical splitter
//     QSplitter *leftSplitter = new QSplitter(Qt::Vertical);

//     // Message list
//     QListWidget *messageList = new QListWidget();
//     for (int i = 0; i < 5; i++)
//     {
//         QListWidgetItem *item = new QListWidgetItem(messageList);
//         QWidget *messageWidget = new QWidget();
//         QVBoxLayout *messageLayout = new QVBoxLayout(messageWidget);
        
//         QHBoxLayout *userLayout = new QHBoxLayout();
//         QLabel *avatar = new QLabel("🙂");
//         QLabel *username = new QLabel(QString("User %1").arg(i+1));
//         userLayout->addWidget(avatar);
//         userLayout->addWidget(username);

//         QLabel *messageContent = new QLabel("Hello, this is a message.");

//         messageLayout->addLayout(userLayout);
//         messageLayout->addWidget(messageContent);
//         messageWidget->setLayout(messageLayout);

//         item->setSizeHint(messageWidget->sizeHint());
//         messageList->setItemWidget(item, messageWidget);
//     }
//     leftSplitter->addWidget(messageList);

//     // Text edit and send button in a single widget with white background
//     QWidget *sendWidget = new QWidget();
//     QVBoxLayout *sendWidgetLayout = new QVBoxLayout(sendWidget);
//     sendWidgetLayout->setSpacing(0); // No space between text edit and send button
//     sendWidget->setStyleSheet("background-color: white; border: none;");

//     QTextEdit *sendTextEdit = new QTextEdit();
//     sendTextEdit->setStyleSheet("border: none;");
//     sendWidgetLayout->addWidget(sendTextEdit, 1);

//     QHBoxLayout *sendButtonLayout = new QHBoxLayout();
//     QPushButton *sendButton = new QPushButton("Send");
//     sendButtonLayout->addStretch(1);
//     sendButtonLayout->addWidget(sendButton);
//     sendButtonLayout->setContentsMargins(0, 0, 0, 0);
//     sendWidgetLayout->addLayout(sendButtonLayout);

//     leftSplitter->addWidget(sendWidget);

//     // Set initial sizes for the left splitter (5:1 ratio)
//     leftSplitter->setSizes({500, 100});

//     mainSplitter->addWidget(leftSplitter);

//     // Right section (list of users with avatars and usernames)
//     QListWidget *userList = new QListWidget();
//     for (int i = 0; i < 10; i++)
//     {
//         QListWidgetItem *item = new QListWidgetItem(userList);
//         QWidget *userWidget = new QWidget();
//         QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
//         QLabel *avatar = new QLabel("🙂");
//         QLabel *username = new QLabel(QString("User %1").arg(i+1));
//         userLayout->addWidget(avatar);
//         userLayout->addWidget(username);
//         userWidget->setLayout(userLayout);

//         item->setSizeHint(userWidget->sizeHint());
//         userList->setItemWidget(item, userWidget);
//     }
//     mainSplitter->addWidget(userList);

//     mainSplitter->setStretchFactor(0, 5); // Set left section to take up 5/6 of the width
//     mainSplitter->setStretchFactor(1, 1); // Set right section to take up 1/6 of the width

//     mainLayout->addWidget(mainSplitter);
//     centralWidget->setLayout(mainLayout);
//     mainWindow.setCentralWidget(centralWidget);
//     QString buttonStyle = "QPushButton {"
//                       "    background-color: #D4D4D4;" // Example color
//                       "    border: 1px solid #AAAAAA;"
//                       "    border-radius: 5px;" // Rounded corners
//                       "    padding: 5px 15px;" // Padding around text
//                       "}"
//                       "QPushButton:hover {"
//                       "    background-color: #C4C4C4;" // Slightly different color when hovered
//                       "}";

//     chatButton->setStyleSheet(buttonStyle);
//     fileButton->setStyleSheet(buttonStyle);
//     sendButton->setStyleSheet(buttonStyle);
//     mainWindow.show();

//     return app.exec();
// }

