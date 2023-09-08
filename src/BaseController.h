#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H
#include <QObject>
#include <QString>
class BaseController : public QObject
{
    Q_OBJECT
public:
    BaseController(BaseController *mmainController = nullptr):mainController(mmainController){}
    virtual void handleViewSignal(const QJsonObject &data) = 0;
    virtual ~BaseController() {};
    BaseController * getMainController() const {return mainController;}
signals:
    void sendToView(const QJsonObject &data);
private:
    BaseController *mainController;
};
#endif // BASECONTROLLER_H