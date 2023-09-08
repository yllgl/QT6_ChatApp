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
    MessageController* messagecontroller = new MessageController(maincontroller);
    maincontroller->appendController(QSharedPointer<BaseController>(messagecontroller));
    LoginWidget *loginWidget = new LoginWidget();
    maincontroller->appendView(QSharedPointer<View>(loginWidget));
    loginWidget->show();
}