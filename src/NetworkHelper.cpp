#include "NetworkHelper.h"
NetworkHelper::NetworkHelper(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

NetworkHelper::~NetworkHelper() {}

void NetworkHelper::sendRequest(const QUrl &url, const QJsonObject &data, const QString &customIdentifier) {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QByteArray requestData = QJsonDocument(data).toJson();

    QNetworkReply *reply = manager->post(request, requestData);
    reply->setProperty("customIdentifier", customIdentifier);  // Store the custom identifier with the reply
    connect(reply, &QNetworkReply::finished, this, &NetworkHelper::onReplyFinished);
}

void NetworkHelper::onReplyFinished() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    QString customIdentifier = reply->property("customIdentifier").toString();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject obj = doc.object();

        emit requestFinished(customIdentifier, obj);
    } else {
        emit requestFailed(customIdentifier, reply->errorString());
    }

    reply->deleteLater();
}