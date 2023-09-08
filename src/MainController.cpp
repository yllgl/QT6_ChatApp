#include "MainController.h"
#include<QObject>
#include <cstdint>
#include<QJsonArray>
#include "View.h"
#include "utils.h"
void MainController::handleViewSignal(const QJsonObject &data){
    mainProcess(data);
    for (auto controller : *controllers) {
        controller->handleViewSignal(data);
    }
}
void MainController::appendController(QSharedPointer<BaseController> controller){
    controllers->append(controller);
    // QObject::connect(controller.data(), &BaseController::sendToView, this, &emitSendToView);
}
void MainController::removeController(QSharedPointer<BaseController> controller){
    controllers->removeOne(controller);
}
void MainController::appendView(QSharedPointer<View> view){
    views->append(view);
    QObject::connect(this, &BaseController::sendToView, [this, view](const QJsonObject &data){
    if(view) {
        view->updateUI(data);
    }
});
    QObject::connect(view->signal, &ViewSignal::sendToController, this, &BaseController::handleViewSignal);
}
void MainController::removeView(QSharedPointer<View> view){
    views->removeOne(view);
    QObject::disconnect(view->signal, &ViewSignal::sendToController, this, &BaseController::handleViewSignal);
}
void MainController::mainProcess(const QJsonObject &data){
    if (data.contains("isRemoveController") && data["RemoveController"].isArray()) {
        QJsonArray extractedArray = data["RemoveController"].toArray();
        for (const QJsonValue& value : extractedArray) {
            removeController(QSharedPointer<BaseController>(static_cast<BaseController*>(stringToPointer(value.toString()))));
        }
    }
    if (data.contains("isRemoveView") && data["RemoveView"].isArray()) {
        QJsonArray extractedArray = data["RemoveView"].toArray();
        for (const QJsonValue& value : extractedArray) {
            removeView(QSharedPointer<View>(static_cast<View*>(stringToPointer(value.toString()))));
        }
    }
}