#ifndef VIEW_H
#define VIEW_H
#include <QObject>
#include <QJsonObject>
class ViewSignal : public QObject
{
    Q_OBJECT
signals:
    void sendToController(const QJsonObject &data);
};
class View
{
public:
    View(): signal(new ViewSignal()) {};
    virtual void updateUI(const QJsonObject &data) = 0;
    virtual ~View() {};
    ViewSignal* signal;
};
#endif // VIEW_H