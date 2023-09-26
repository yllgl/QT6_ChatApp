#include "ClientApp.h"
#include "loginWidget.h"
#include "MainController.h"
#include "heartbeatController.h"
#include <QSharedPointer>
#include "processController.h"
#include "chatItemController.h"
#include "messageController.h"

ClientApp::ClientApp() {
    maincontroller = new MainController();
    HeartBeatController* heartbeatController = new HeartBeatController(maincontroller);
    maincontroller->appendController(QSharedPointer<BaseController>(heartbeatController));
    processController* processcontroller = new processController(maincontroller);
    maincontroller->appendController(QSharedPointer<BaseController>(processcontroller));
    ChatItemController* chatitemcontroller = new ChatItemController(maincontroller);
    maincontroller->appendController(QSharedPointer<BaseController>(chatitemcontroller));
    if(!udpSocket.bind(QHostAddress::AnyIPv4, udpSocket.localPort(), QUdpSocket::ShareAddress))
    {
        qDebug() << "Error: Could not bind to port";
        return;
    }
    MessageController* messagecontroller = new MessageController(maincontroller,&udpSocket);
    
    maincontroller->appendController(QSharedPointer<BaseController>(messagecontroller));
    LoginWidget *loginWidget = new LoginWidget(nullptr,udpSocket.localPort());
    maincontroller->appendView(QSharedPointer<View>(loginWidget));
    loginWidget->show();
}