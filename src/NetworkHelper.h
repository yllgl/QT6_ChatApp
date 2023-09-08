#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QObject>

class NetworkHelper : public QObject {
    Q_OBJECT
public:
    NetworkHelper(QObject *parent = nullptr);
    ~NetworkHelper();

    void sendRequest(const QUrl &url, const QJsonObject &data, const QString &customIdentifier = "");

signals:
    void requestFinished(const QString &customIdentifier, const QJsonObject &response);
    void requestFailed(const QString &customIdentifier, const QString &errorMessage);

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *manager;
};
#endif // NETWORKHELPER_H