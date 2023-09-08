#ifndef PROCESSCONTROLLER_H
#define PROCESSCONTROLLER_H
#include "BaseController.h"
#include <QJsonObject>
class processController : public BaseController
{
    Q_OBJECT
    public:
        processController(BaseController* parent):BaseController(parent){}
        void handleViewSignal(const QJsonObject &data) override;
    private:
        void loginProcess(const QJsonObject &data);
        void openChatWindow(const QJsonObject &data);
};
#endif // PROCESSCONTROLLER_H