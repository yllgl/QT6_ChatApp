#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "BaseController.h"
class View;
#include <QList>
#include <QJsonObject>
#include <QSharedPointer>
class MainController : public BaseController
{
    Q_OBJECT
    public:
        MainController():BaseController(nullptr),
        controllers(new QList<QSharedPointer<BaseController>>()),views(new QList<QSharedPointer<View>>()){
        }
        void handleViewSignal(const QJsonObject &data) override;
        void appendController(QSharedPointer<BaseController> controller);
        void removeController(QSharedPointer<BaseController> controller);
        void appendView(QSharedPointer<View> view);
        void removeView(QSharedPointer<View> view);
        void mainProcess(const QJsonObject &data);
        void emitSendToView(const QJsonObject &data){
            emit sendToView(data);
        }
        ~MainController() {};
    private:
    QList<QSharedPointer<BaseController>>* controllers;
    QList<QSharedPointer<View>>* views;
};
#endif // MAINCONTROLLER_H